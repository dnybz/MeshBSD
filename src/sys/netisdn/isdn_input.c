/*-
 * Copyright (c) 2016 Henning Matyschok
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE. 
 */
/*
 * Copyright (c) 1997, 2000 Hellmuth Michaelis. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
 
#include "opt_inet.h" 

#include "opt_isdn.h"
#include "opt_isdn_debug.h"
 
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/lock.h>
#include <sys/rwlock.h>
#include <sys/socket.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/if_types.h>
#include <net/netisr.h>

#include <netisdn/isdn.h>
#include <netisdn/isdn_var.h>

/*
 * XXX ...
 */
static void 	isdn_rxd_ack(struct isdn_softc *, int);

static void 	isdn_i_frame_input(struct isdn_softc *, struct mbuf *);
static void 	isdn_s_frame_input(struct isdn_softc *, struct mbuf *);
static void 	isdn_u_frame_input(struct isdn_softc *, struct mbuf *);

static void 	isdn_input(struct mbuf *);

/*
 * XXX ...
 */

extern struct protosw isdnsw[];

/*
 * Default mtx(9) on global scope.
 */
struct mtx isdn_mtx;

MTX_SYSINIT(isdn_mtx, &isdn_mtx, "isdn_lock");

/*
 * Set containing ISDN channel. 
 */
struct isdn_head isdn_ifaddrhead;
struct rwlock isdn_ifaddr_lock;

RW_SYSINIT(isdn_ifadddr_lock, &isdn_ifaddr_lock, "isdn_ifaddr_lock");

/*
 * ISDN input queue is managed by netisr(9).
 */
static struct netisr_handler isdn_nh = {
	.nh_name 		= "isdn layer",
	.nh_handler 		= isdn_input,
	.nh_proto 		= NETISR_ISDN,
	.nh_policy 		= NETISR_POLICY_FLOW,
};

/*
 * ISDN initialisation.
 */
void
isdn_init(void)
{
	TAILQ_INIT(&isdn_ifaddrhead);
	netisr_register(&isdn_nh);
}

/*
 * Input processing. 
 */
 
static void
isdn_input(struct mbuf *m)
{	
	struct ifnet *ifp;
	struct isdn_sc *sc;
	struct isdn_rd *rd;
	
	uint8_t *ptr;
	
	uint8_t chan;
	uint8_t proto;

	M_ASSERTPKTHDR(m);
		
	if (m->m_pkthdr.len < ISDN_HDRLEN) 
		goto bad;

	if (m->m_len < ISDN_HDRLEN) {
		if ((m = m_pullup(m, ISDN_HDRLEN)) == NULL)
			goto out;
	}
	
	if ((ifp = m->m_pkthdr.rcvif) == NULL)
		goto bad;

	IF_AFDATA_RLOCK(ifp);
	sc = ISDN_SOFTC(ifp);
	IF_AFDATA_RUNLOCK(ifp);	

	rd = mtod(m, struct isdn_rd *);	
	
	chan = rd->rd_chan;
	proto = rd->rd_proto;
	
	m_adj(m, ISDN_HDRLEN);
	
	ptr = mtod(m, uint8_t *);
	
#ifdef ISDN_DEBUG
	(void)printf("%s: on=%s \n", __func__, ifp->if_xname);
#endif /* ISDN_DEBUG */

	switch (chan) {
	case ISDN_D_CHAN:
/*
 * LAPD Frame received. 
 */	
		if ((*(ptr + OFF_CNTL) & 0x01) == 0) {
/* 
 * 6 oct - 2 chksum oct 
 */
			if (m->m_len < 4) {
				sc->sc_l2.l2_stat.err_rx_len++;
				NDBGL2(L2_ERROR, "ERROR, I-frame < 6 octetts!");
				goto bad;
			}
			isdn_i_frame_input(sc, m);
		} else if ((*(ptr + OFF_CNTL) & 0x03) == 0x01 ) {
/* 
 * 6 oct - 2 chksum oct 
 */
			if (m->m_len < 4) {
				sc->sc_l2.l2_stat.err_rx_len++;
				NDBGL2(L2_ERROR, "ERROR, S-frame < 6 octetts!");
				goto bad;
			}
			isdn_s_frame_input(sc, m);
		} else if ((*(ptr + OFF_CNTL) & 0x03) == 0x03 ) {
/* 
 * 5 oct - 2 chksum oct 
 */
			if (m->m_len < 3) {
				sc->sc_l2.l2_stat.err_rx_len++;
				NDBGL2(L2_ERROR, "ERROR, U-frame < 5 octetts!");
				goto bad;
			}
			isdn_u_frame_input(sc, m);
		} else {
			sc->sc_l2.l2_stat.err_rx_badf++;
			NDBGL2(L2_ERROR, "ERROR, bad frame rx'd - ");
			isdn_l2_print_frame(m->m_len, m->m_data);
			goto bad;
		}
		break;
	case ISDN_B1_CHAN: 
	case ISDN_B2_CHAN:
			
			/* FALLTHROUGH */
/*
 * XXX: Well, any ethernet port aggregates two B-chans.
 */	
		break;
	default:
		NDBGL2(L2_ERROR, "ERROR, unknown frame rx'd - ");
		isdn_l2_print_frame(m->m_len, m->m_data);
		goto bad;
	}	
out:	
	return;	
bad:
	m_freem(m);	
}

/*
 *	Process I frame, "I COMMAND" Q.921 03/93 pp 68 and pp 77
 */
static void
isdn_i_frame_input(struct isdn_softc *sc, struct mbuf *m)
{
	u_char ptr = mtod(m, uint8_t *);
	
	int nr;
	int ns;
	int p;

	if (!((sc->sc_l2.l2_tei_valid == TEI_VALID) &&
	     (sc->sc_l2.l2_tei == GETTEI(*(ptr+OFF_TEI))))) {
		m_freem(m);
		return;
	}

	if ((sc->sc_l2.l2_Q921_state != ST_MULTIFR) && 
		(sc->sc_l2.l2_Q921_state != ST_TIMREC)) {
		NDBGL2(L2_I_ERR, "%s: state != (MF || TR)!", __func__);
		m_freem(m);
		return;
	}
	SC_WLOCK(sc);
/* 
 * update frame count 
 */
	sc->sc_l2.l2_stat.rx_i++;
	
	nr = GETINR(*(ptr + OFF_INR));
	ns = GETINS(*(ptr + OFF_INS));
	p = GETIP(*(ptr + OFF_INR));
/* 
 * last packet ack 
 */
	isdn_rxd_ack(sc, nr);		
/* 
 * own receiver busy ? 
 */
	if (sc->sc_l2.l2_own_busy)	{
		m_freem(m);	/* yes, discard information */
/* 
 * P bit == 1 ? 
 */
		if (p == 1)	{
/*
 * Tx RNR response.
 */				
			(void)isdn_l2_tx_s_frame(sc, CR_RSP_TO_NT, p, RNR);		

			sc->sc_l2.l2_ack_pend = 0;	/* clear ACK pending */
		}
	} else {
/* 
 * own receiver ready, if expected sequence number ?  
 */	
		if (ns == sc->sc_l2.l2_vr)	{
			M128INC(sc->sc_l2.l2_vr);	/* yes, update */

			sc->sc_l2.l2_rej_excpt = 0;	/* clr reject exception */

			m_adj(m, I_HDR_LEN);	/* strip i frame header */

			sc->sc_l2.l2_iframe_sent = 0;	/* reset i ack'd already */

			isdn_l2_data_ind(sc, m);	/* pass data up */

			if (!sc->sc_l2.l2_iframe_sent) {
/*
 * Tx RR response.
 */				
				(void)isdn_l2_tx_s_frame(sc, CR_RSP_TO_NT, p, RR);
				
				sc->sc_l2.l2_ack_pend = 0;	/* clr ACK pending */
			}
		} else {
/* 
 * ERROR, sequence number NOT expected 
 */		
			m_freem(m);	/* discard information */
/* 
 * already exception ? 
 */
			if (sc->sc_l2.l2_rej_excpt == 1) {
/* 
 * immediate resp ? 
 */
				if (p == 1)	{
/*
 * Tx RR response.
 */				
					(void)isdn_l2_tx_s_frame(sc, CR_RSP_TO_NT, p, RR);
					
					sc->sc_l2.l2_ack_pend = 0; /* clr ack pend */
				}
			} else {
/* 
 * not in exception cond 
 */			
				sc->sc_l2.l2_rej_excpt = 1;	/* set exception */
				
/*
 * Tx REJ response.
 */				
				(void)isdn_l2_tx_s_frame(sc, CR_RSP_TO_NT, p, REJ);			
				
				sc->sc_l2.l2_ack_pend = 0;	/* clr ack pending */
			}
		}
	}
/* 
 * sequence number ranges as expected ? 
 */
	if (isdn_l2_nr_ok(nr, sc->sc_l2.l2_va, sc->sc_l2.l2_vs)) {
		if (sc->sc_l2.l2_Q921_state == ST_TIMREC) {
			sc->sc_l2.l2_va = nr;
		} else {
/* 
 * yes, other side busy ? 
 */
			if (sc->sc_l2.l2_peer_busy) {
				sc->sc_l2.l2_va = nr;	/* yes, update ack count */
			} else {
/* 
 * other side ready 
 */		
				if (nr == sc->sc_l2.l2_vs) {
/* 
 * count expected ? 
 */								
					sc->sc_l2.l2_va = nr;	/* update ack */
					isdn_l2_T200_stop(sc);
					isdn_l2_T203_restart(sc);
				} else {
					if (nr != sc->sc_l2.l2_va) {
						sc->sc_l2.l2_va = nr;
						isdn_l2_T200_restart(sc);
					}
				}
			}
		}
	} else {
/* 
 * sequence error 
 */		
		isdn_nr_error_recovery(sc);	
		
		sc->sc_l2.l2_Q921_state = ST_AW_EST;
	}
	SC_WUNLOCK(sc);
}

/*
 * Process S frame.
 */
static void
isdn_s_frame_input(struct isdn_softc *sc, struct mbuf *m)
{
	u_char *ptr = mtod(m, uint8_t *);

	if (!((sc->sc_l2.l2_tei_valid == TEI_VALID) &&
	     (sc->sc_l2.l2_tei == GETTEI(*(ptr+OFF_TEI))))) {
		goto out;
	}

	sc->sc_l2.l2_rxd_CR = GETCR(*(ptr + OFF_SAPI));
	sc->sc_l2.l2_rxd_PF = GETSPF(*(ptr + OFF_SNR));
	sc->sc_l2.l2_rxd_NR = GETSNR(*(ptr + OFF_SNR));

	isdn_rxd_ack(sc, sc->sc_l2.l2_rxd_NR);

	switch(*(ptr + OFF_SRCR)) {
	case RR:
		sc->sc_l2.l2_stat.rx_rr++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd RR, N(R) = %d", sc->sc_l2.l2_rxd_NR);
		isdn_l2_next_state(sc, EV_RXRR);
		break;
	case RNR:
		sc->sc_l2.l2_stat.rx_rnr++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd RNR, N(R) = %d", sc->sc_l2.l2_rxd_NR);
		isdn_l2_next_state(sc, EV_RXRNR);
		break;
	case REJ:
		sc->sc_l2.l2_stat.rx_rej++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd REJ, N(R) = %d", sc->sc_l2.l2_rxd_NR);
		isdn_l2_next_state(sc, EV_RXREJ);
		break;
	default:
		sc->sc_l2.l2_stat.err_rx_bads++; /* update statistics */
		NDBGL2(L2_S_ERR, "ERROR, unknown code, frame = ");
		isdn_l2_print_frame(m->m_len, m->m_data);
		break;
	}
out:	
	m_freem(m);
}

/*
 * Process a received U frame.
 */
static void
isdn_u_frame_input(struct isdn_softc *sc, struct mbuf *m)
{
	u_char *ptr = mtod(m, uint8_t *);

	int sapi = GETSAPI(*(ptr + OFF_SAPI));
	int tei = GETTEI(*(ptr + OFF_TEI));
	int pfbit = GETUPF(*(ptr + OFF_CNTL));

	switch (*(ptr + OFF_CNTL) & ~UPFBIT) {
/* 
 * commands 
 */
	case SABME:
		if ((sc->sc_l2.l2_tei_valid == TEI_VALID) && 
			(sc->sc_l2.l2_tei == GETTEI(*(ptr+OFF_TEI)))) {
			sc->sc_l2.l2_stat.rx_sabme++;
			NDBGL2(L2_U_MSG, 
				"SABME, sapi = %d, tei = %d", sapi, tei);
			sc->sc_l2.l2_rxd_PF = pfbit;
			isdn_l2_next_state(sc, EV_RXSABME);
		}
		m_freem(m);
		break;
	case UI:
		if ((sapi == SAPI_L2M) && 
			(tei == GROUP_TEI) &&
			   (*(ptr + OFF_MEI) == MEI)) {
/* 
 * layer 2 management (SAPI = 63) 
 */
			sc->sc_l2.l2_stat.rx_tei++;
			isdn_l2_rxd_tei(sc, m);
		} else if ((sapi == SAPI_CCP) && (tei == GROUP_TEI)) {
/* 
 * call control (SAPI = 0) 
 */
			sc->sc_l2.l2_stat.rx_ui++;
/* 
 * strip ui header 
 */
			m_adj(m, UI_HDR_LEN);
/* 
 * to upper layer 
 */
			isdn_dl_unit_data_ind(sc->sc_l2.l2_l3, m);
		} else {
			sc->sc_l2.l2_stat.err_rx_badui++;
			NDBGL2(L2_U_ERR, "unknown UI frame!");
			m_freem(m);
		}
		break;
	case DISC:
		if ((sc->sc_l2.l2_tei_valid == TEI_VALID) && 
			(sc->sc_l2.l2_tei == GETTEI(*(ptr+OFF_TEI)))) {
			sc->sc_l2.l2_stat.rx_disc++;
			NDBGL2(L2_U_MSG, 
				"DISC, sapi = %d, tei = %d", sapi, tei);
			sc->sc_l2.l2_rxd_PF = pfbit;
			isdn_l2_next_state(sc, EV_RXDISC);
		}
		m_freem(m);
		break;
	case XID:
		
		if ((sc->sc_l2.l2_tei_valid == TEI_VALID) && 
			(sc->sc_l2.l2_tei == GETTEI(*(ptr+OFF_TEI)))) {
			sc->sc_l2.l2_stat.rx_xid++;
			NDBGL2(L2_U_MSG, 
				"XID, sapi = %d, tei = %d", sapi, tei);
		}
		m_freem(m);
		break;
/* 
 * responses 
 */
	case DM:
		if ((sc->sc_l2.l2_tei_valid == TEI_VALID) && 
			(sc->sc_l2.l2_tei == GETTEI(*(ptr+OFF_TEI)))) {
				sc->sc_l2.l2_stat.rx_dm++;
			NDBGL2(L2_U_MSG, 
				"DM, sapi = %d, tei = %d", sapi, tei);
			isdn_l2_print_frame(m->m_len, m->m_data);
			sc->sc_l2.l2_rxd_PF = pfbit;
			isdn_l2_next_state(sc, EV_RXDM);
		}
		m_freem(m);
		break;
	case UA:
		if ((sc->sc_l2.l2_tei_valid == TEI_VALID) && 
			(sc->sc_l2.l2_tei == GETTEI(*(ptr+OFF_TEI)))) {
			sc->sc_l2.l2_stat.rx_ua++;
			NDBGL2(L2_U_MSG, "UA, sapi = %d, tei = %d", sapi, tei);
			sc->sc_l2.l2_rxd_PF = pfbit;
			isdn_l2_next_state(sc, EV_RXUA);
		}
		m_freem(m);
		break;
	case FRMR:
		if ((sc->sc_l2.l2_tei_valid == TEI_VALID) && 
			(sc->sc_l2.l2_tei == GETTEI(*(ptr+OFF_TEI)))) {
			sc->sc_l2.l2_stat.rx_frmr++;
			NDBGL2(L2_U_MSG, "FRMR, sapi = %d, tei = %d", sapi, tei);
			sc->sc_l2.l2_rxd_PF = pfbit;
			isdn_l2_next_state(sc, EV_RXFRMR);
		}
		m_freem(m);
		break;
	default:
		if ((sc->sc_l2.l2_tei_valid == TEI_VALID) && 
			(sc->sc_l2.l2_tei == GETTEI(*(ptr+OFF_TEI)))) {
			NDBGL2(L2_U_ERR, "UNKNOWN TYPE ERROR, sapi = %d, "
				"tei = %d, frame = ", sapi, tei);
			isdn_l2_print_frame(m->m_len, m->m_data);
		} else {
			NDBGL2(L2_U_ERR, "not mine -  UNKNOWN TYPE ERROR, "
				"sapi = %d, tei = %d, frame = ", sapi, tei);
			isdn_l2_print_frame(m->m_len, m->m_data);
		}
		sc->sc_l2.l2_stat.err_rx_badui++;
		m_freem(m);
		break;
	}
}

/*
 * Got s or i frame, check if valid ack for last sent frame.
 */
static void
isdn_rxd_ack(struct isdn_softc *sc, int nr)
{

	NDBGL2(L2_ERROR, "N(R)=%d, UA=%d, V(R)=%d, V(S)=%d, V(A)=%d",
		nr,
		sc->sc_l2.l2_ua_num,
		sc->sc_l2.l2_vr,
		sc->sc_l2.l2_vs,
		sc->sc_l2.l2_va);

	if (sc->sc_l2.l2_ua_num != UA_EMPTY) {

		M128DEC(nr);

		if (sc->sc_l2.l2_ua_num != nr) {
			NDBGL2(L2_ERROR, 
				"((N(R)-1)=%d) != (UA=%d) !!!", 
				nr, sc->sc_l2.l2_ua_num);
		}
		m_freem(sc->sc_l2.l2_ua_frame);
		sc->sc_l2.l2_ua_num = UA_EMPTY;
	}
}
