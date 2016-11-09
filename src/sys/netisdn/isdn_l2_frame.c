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
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/isdn.h>
#include <netisdn/isdn_var.h>
#include <netisdn/isdn_l2.h>

/*
 * XXX: ...
 */

int isdn_l2_debug = L2_DEBUG_DEFAULT;

/*---------------------------------------------------------------------------*
 *	isdn_print_frame - just print the hex contents of a frame
 *---------------------------------------------------------------------------*/

void
isdn_l2_print_frame(int len, u_char *buf)
{
#ifdef ISDN_DEBUG
	int i;
/* 
 * XXX: ...
 */
	if (isdn_l2_debug & L2_ERROR) {
		for (i = 0; i < len; i++)
			(void)printf(" 0x%x", buf[i]);
	
		(void)printf("\n");
	}
#endif
}


/*---------------------------------------------------------------------------*
 *	got s or i frame, check if valid ack for last sent frame
 *---------------------------------------------------------------------------*/
void
isdn_l2_rxd_ack(struct isdn_softc *sc, int nr)
{

#ifdef NOTDEF
	NDBGL2(L2_ERROR, "N(R)=%d, UA=%d, V(R)=%d, V(S)=%d, V(A)=%d",
		nr,
		sc->sc_l2.l2_ua_num,
		sc->sc_l2.l2_vr,
		sc->sc_l2.l2_vs,
		sc->sc_l2.l2_va);
#endif

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

/*---------------------------------------------------------------------------*
 *	check for v(a) <= n(r) <= v(s)
 *	nr = receive sequence frame counter, va = acknowledge sequence frame
 *	counter and vs = transmit sequence frame counter
 *---------------------------------------------------------------------------*/
int
isdn_l2_nr_ok(int nr, int va, int vs)
{
	int error = 1;
	
	if ((va > nr) && ((nr != 0) || (va != 127))) {
		NDBGL2(L2_ERROR, 
			"ERROR, va = %d, nr = %d, vs = %d [1]", va, nr, vs);
/* 
 * fail 
 */	
		error = 0;	
		goto out;
	}

	if ((nr > vs) && ((vs != 0) || (nr != 127))) {
		NDBGL2(L2_ERROR, 
			"ERROR, va = %d, nr = %d, vs = %d [2]", va, nr, vs);
/* 
 * fail 
 */		
		error = 0;
	}
out:	
	return (error);		/* good */
}


/*-
 *	process i frame
 *	implements the routine "I COMMAND" Q.921 03/93 pp 68 and pp 77
 *
 */
void
isdn_l2_rxd_i_frame(struct isdn_softc *sc, struct mbuf *m)
{
	u_char ptr = m->m_data;
	
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
		NDBGL2(L2_I_ERR, "ERROR, state != (MF || TR)!");
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

	isdn_l2_rxd_ack(sc, nr);		/* last packet ack */
/* 
 * own receiver busy ? 
 */
	if (sc->sc_l2.l2_own_busy)	{
		m_freem(m);	/* yes, discard information */
/* 
 * P bit == 1 ? 
 */
		if (p == 1)	{
			isdn_l2_tx_rnr_resp(l2, p); /* yes, tx RNR */
			sc->sc_l2.l2_ack_pend = 0;	/* clear ACK pending */
		}
	} else {
/* 
 * own receiver ready, where if /* expected sequence number ?  
 */	
		if (ns == sc->sc_l2.l2_vr)	{
			M128INC(sc->sc_l2.l2_vr);	/* yes, update */

			sc->sc_l2.l2_rej_excpt = 0;	/* clr reject exception */

			m_adj(m, I_HDR_LEN);	/* strip i frame header */

			sc->sc_l2.l2_iframe_sent = 0;	/* reset i ack'd already */

			isdn_dl_data_ind(l3, m);	/* pass data up */

			if (!sc->sc_l2.l2_iframe_sent) {
				isdn_l2_tx_rr_resp(l2, p); /* yes, tx RR */
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
					isdn_l2_tx_rr_resp(l2, p); /* yes, tx RR */
					sc->sc_l2.l2_ack_pend = 0; /* clr ack pend */
				}
			} else {
/* 
 * not in exception cond 
 */			
				sc->sc_l2.l2_rej_excpt = 1;	/* set exception */
				isdn_l2_tx_rej_resp(l2, p);	/* tx REJ */
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
					isdn_T200_stop(l2);
					isdn_T203_restart(l2);
				} else {
					if (nr != sc->sc_l2.l2_va) {
						sc->sc_l2.l2_va = nr;
						isdn_T200_restart(l2);
					}
				}
			}
		}
	} else {
/* 
 * sequence error 
 */		
		isdn_nr_error_recovery(l2);	
		sc->sc_l2.l2_Q921_state = ST_AW_EST;
	}
	SC_WUNLOCK(sc);
}

/*---------------------------------------------------------------------------*
 *	internal I FRAME QUEUED UP routine (Q.921 03/93 p 61)
 *---------------------------------------------------------------------------*/
void
isdn_l2_queue_i_frame(struct isdn_softc *sc)
{
	struct mbuf *m;
	u_char *ptr;

	SC_WLOCK(sc);

	if ((sc->sc_l2.l2_peer_busy) || 
		(sc->sc_l2.l2_vs == ((sc->sc_l2.l2_va + MAX_K_VALUE) & 127))) {
		
		if (sc->sc_l2.l2_peer_busy) {
			NDBGL2(L2_I_MSG, "regen IFQUP, cause: peer busy!");
		}

		if (sc->sc_l2.l2_vs == ((sc->sc_l2.l2_va + MAX_K_VALUE) & 127)) {
			NDBGL2(L2_I_MSG, "regen IFQUP, cause: vs=va+k!");
		}
/*
 * XXX: see: Q.921, page 36, 5.6.1 ".. may retransmit an I
 * XXX: frame ...", shall we retransmit the last i frame ?
 */
		if (!(_IF_QEMPTY(&sc->sc_l2.l2_i_queue))) {
			NDBGL2(L2_I_MSG, "re-scheduling IFQU call!");
			START_TIMER(sc->sc_l2.l2_IFQU_callout, 

			isdn_l2_queue_i_frame, l2, IFQU_DLY);
		}
		SC_WUNLOCK(sc);
		return;
	}
/* 
 * Fetch next frame for tx. 
 */
	IF_DEQUEUE(&sc->sc_l2.l2_i_queue, m);   

	if (m == NULL) {
		NDBGL2(L2_I_ERR, "ERROR, mbuf NULL after IF_DEQUEUE");
		SC_WUNLOCK(sc);
		return;
	}

	ptr = m->m_data;

	PUTSAPI(SAPI_CCP, CR_CMD_TO_NT, *(ptr + OFF_SAPI));
	PUTTEI(sc->sc_l2.l2_tei, *(ptr + OFF_TEI));

	*(ptr + OFF_INS) = (sc->sc_l2.l2_vs << 1) & 0xfe; /* bit 0 = 0 */
	*(ptr + OFF_INR) = (sc->sc_l2.l2_vr << 1) & 0xfe; /* P bit = 0 */

	sc->sc_l2.l2_stat.tx_i++;	/* update frame counter */
/* 
 * XXX: wrong ... free'd when ack'd ! 
 */
	isdn_output(sc->sc_ifp, l2, m, MBUF_DONTFREE); 
/*
 * in case we ack an I frame with another I frame 
 */	
	sc->sc_l2.l2_iframe_sent = 1;

	if (sc->sc_l2.l2_ua_num != UA_EMPTY) {
/* 
 * failsafe 
 */	
		NDBGL2(L2_I_ERR, "sc->sc_l2.l2_ua_num: %d != UA_EMPTY", 
			sc->sc_l2.l2_ua_num);
/*
 * XXX
 */		
		isdn_l2_print_var(sc);
		
		m_freem(sc->sc_l2.l2_ua_frame);
	}
/* 
 * save unack'd frame 
 */
	sc->sc_l2.l2_ua_frame = m;		
/* 
 * save frame number 
 */
	sc->sc_l2.l2_ua_num = sc->sc_l2.l2_vs;	

	M128INC(sc->sc_l2.l2_vs);

	sc->sc_l2.l2_ack_pend = 0;

	SC_WUNLOCK(sc);

	if (sc->sc_l2.l2_T200 == TIMER_IDLE) {
		isdn_T203_stop(l2);
		isdn_T200_start(l2);
	}
}

/*-
 *	process s frame
 *
 */
void
isdn_l2_rxd_s_frame(struct isdn_softc *sc, struct mbuf *m)
{
	u_char *ptr = m->m_data;

	if (!((sc->sc_l2.l2_tei_valid == TEI_VALID) &&
	     (sc->sc_l2.l2_tei == GETTEI(*(ptr+OFF_TEI))))) {
		goto out;
	}

	sc->sc_l2.l2_rxd_CR = GETCR(*(ptr + OFF_SAPI));
	sc->sc_l2.l2_rxd_PF = GETSPF(*(ptr + OFF_SNR));
	sc->sc_l2.l2_rxd_NR = GETSNR(*(ptr + OFF_SNR));

	isdn_l2_rxd_ack(sc, sc->sc_l2.l2_rxd_NR);

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
		isdn_print_frame(m->m_len, m->m_data);
		break;
	}
out:	
	m_freem(m);
}

/*---------------------------------------------------------------------------*
 *	transmit RR cmd
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_rr_cmd(struct isdn_softc *sc, pbit_t pbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RR, isdnif = %d", sc->sc_l2.l2_l3->isdnif);

	m = isdn_l2_build_s_frame(sc, CR_CMD_TO_NT, pbit, RR);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);

	sc->sc_l2.l2_stat.tx_rr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RR resp
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_rr_resp(struct isdn_softc *sc, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RR, isdnif = %d", sc->sc_l2.l2_l3->isdnif);

	m = isdn_l2_build_s_frame(sc, CR_RSP_TO_NT, fbit, RR);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);

	sc->sc_l2.l2_stat.tx_rr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RNR cmd
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_rnr_cmd(struct isdn_softc *sc, pbit_t pbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RNR, isdnif = %d", sc->sc_l2.l2_l3->isdnif);

	m = isdn_l2_build_s_frame(sc, CR_CMD_TO_NT, pbit, RNR);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);

	sc->sc_l2.l2_stat.tx_rnr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RNR resp
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_rnr_resp(struct isdn_softc *sc, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RNR, isdnif = %d", sc->sc_l2.l2_l3->isdnif);

	m = isdn_l2_build_s_frame(sc, CR_RSP_TO_NT, fbit, RNR);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);

	sc->sc_l2.l2_stat.tx_rnr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit REJ resp
 *---------------------------------------------------------------------------*/
void
isdn__tx_rej_resp(struct isdn_softc *sc, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx REJ, isdnif = %d", sc->sc_l2.l2_l3->isdnif);

	m = isdn_l2_build_s_frame(sc, CR_RSP_TO_NT, fbit, REJ);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);

	sc->sc_l2.l2_stat.tx_rej++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	build S-frame for sending
 *---------------------------------------------------------------------------*/
struct mbuf *
isdn_l2_build_s_frame(struct isdn_softc *sc, crbit_to_nt_t crbit, 
	pbit_t pbit, u_char type)
{
	struct mbuf *m;

	if ((m = isdn_getmbuf(S_FRAME_LEN, M_DONTWAIT, MT_I4B_D)) == NULL)
		goto out;

	PUTSAPI(SAPI_CCP, crbit, m->m_data[OFF_SAPI]);

	PUTTEI(sc->sc_l2.l2_tei, m->m_data[OFF_TEI]);

	m->m_data[OFF_SRCR] = type;

	m->m_data[OFF_SNR] = (sc->sc_l2.l2_vr << 1) | (pbit & 0x01);
out:
	return (m);
}

/*-
 *	process a received U-frame
 *
 */
void
isdn_l2_rxd_u_frame(struct isdn_softc *sc, struct mbuf *m)
{
	u_char *ptr = m->m_data;

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
			isdn_tei_rxframe(l2, l3, m);
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
			isdn_print_frame(m->m_len, m->m_data);
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
			isdn_print_frame(m->m_len, m->m_data);
		} else {
			NDBGL2(L2_U_ERR, "not mine -  UNKNOWN TYPE ERROR, "
				"sapi = %d, tei = %d, frame = ", sapi, tei);
			isdn_print_frame(m->m_len, m->m_data);
		}
		sc->sc_l2.l2_stat.err_rx_badui++;
		m_freem(m);
		break;
	}
}

/*---------------------------------------------------------------------------*
 *	build U-frame for sending
 *---------------------------------------------------------------------------*/
struct mbuf *
isdn_l2_build_u_frame(struct isdn_softc *sc, crbit_to_nt_t crbit, 
	pbit_t pbit, u_char type)
{
	struct mbuf *m;

	if ((m = isdn_getmbuf(U_FRAME_LEN, M_DONTWAIT, MT_I4B_D)) == NULL)
		goto out;

	PUTSAPI(SAPI_CCP, crbit, m->m_data[OFF_SAPI]);

	PUTTEI(sc->sc_l2.l2_tei, m->m_data[OFF_TEI]);

	if (pbit)
		m->m_data[OFF_CNTL] = type | UPBITSET;
	else
		m->m_data[OFF_CNTL] = type & ~UPBITSET;
out:
	return (m);
}

/*---------------------------------------------------------------------------*
 *	transmit SABME command
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_sabme(struct isdn_softc *sc, pbit_t pbit)
{
	struct mbuf *m;

	sc->sc_l2.l2_stat.tx_sabme++;
	NDBGL2(L2_U_MSG, "tx SABME, tei = %d", sc->sc_l2.l2_tei);
	m = isdn_l2_build_u_frame(sc, CR_CMD_TO_NT, pbit, SABME);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit DM response
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_dm(struct isdn_softc *sc, fbit_t fbit)
{
	struct mbuf *m;

	sc->sc_l2.l2_stat.tx_dm++;
	NDBGL2(L2_U_MSG, "tx DM, tei = %d", sc->sc_l2.l2_tei);
	m = isdn_l2_build_u_frame(sc, CR_RSP_TO_NT, fbit, DM);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit DISC command
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_disc(struct isdn_softc *sc, pbit_t pbit)
{
	struct mbuf *m;

	sc->sc_l2.l2_stat.tx_disc++;
	NDBGL2(L2_U_MSG, "tx DISC, tei = %d", sc->sc_l2.l2_tei);
	m = isdn_l2_build_u_frame(sc, CR_CMD_TO_NT, pbit, DISC);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit UA response
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_ua(struct isdn_softc *sc, fbit_t fbit)
{
	struct mbuf *m;

	sc->sc_l2.l2_stat.tx_ua++;
	NDBGL2(L2_U_MSG, "tx UA, tei = %d", sc->sc_l2.l2_tei);
	m = isdn_l2_build_u_frame(sc, CR_RSP_TO_NT, fbit, UA);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit FRMR response
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_frmr(struct isdn_softc *sc, fbit_t fbit)
{
	struct mbuf *m;

	sc->sc_l2.l2_stat.tx_frmr++;
	NDBGL2(L2_U_MSG, "tx FRMR, tei = %d", sc->sc_l2.l2_tei);
	m = isdn_l2_build_u_frame(sc, CR_RSP_TO_NT, fbit, FRMR);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);
}

