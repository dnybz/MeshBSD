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

#include <netisdn/isdn_debug.h>
#include <netisdn/isdn_ioctl.h>

#include <netisdn/isdn_global.h>
#include <netisdn/isdn_l2.h>
#include <netisdn/isdn_l1l2.h>
#include <netisdn/isdn_isdnq931.h>
#include <netisdn/isdn_mbuf.h>
#include <netisdn/isdn_l2fsm.h>
#include <netisdn/isdn_l3l4.h>

/*---------------------------------------------------------------------------*
 *	isdn_print_frame - just print the hex contents of a frame
 *---------------------------------------------------------------------------*/
void
isdn_l2_print_frame(int len, u_char *buf)
{
#ifdef ISDN_DEBUG
	int i;

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
isdn_l2_rxd_ack(struct isdn_l2 *l2, int nr)
{

#ifdef NOTDEF
	NDBGL2(L2_ERROR, "N(R)=%d, UA=%d, V(R)=%d, V(S)=%d, V(A)=%d",
		nr,
		l2->ua_num,
		l2->vr,
		l2->vs,
		l2->va);
#endif

	if (l2->ua_num != UA_EMPTY) {

		M128DEC(nr);

		if (l2->ua_num != nr) {
			NDBGL2(L2_ERROR, 
				"((N(R)-1)=%d) != (UA=%d) !!!", 
				nr, l2->ua_num);
		}
		m_freem(l2->ua_frame);
		l2->ua_num = UA_EMPTY;
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
		error = 0;	/* fail */
		goto out;
	}

	if ((nr > vs) && ((vs != 0) || (nr != 127))) {
		NDBGL2(L2_ERROR, 
			"ERROR, va = %d, nr = %d, vs = %d [2]", va, nr, vs);
		error = 0;	/* fail */
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
	u_char *ptr;
	struct isdn_l2 *l2;
	
	int nr;
	int ns;
	int p;

	ptr = m->m_data;
	l2 = &sc->sc_l2;

	if (!((l2->l2_tei_valid == TEI_VALID) &&
	     (l2->l2_tei == GETTEI(*(ptr+OFF_TEI))))) {
		m_freem(m);
		return;
	}

	if ((l2->l2_Q921_state != ST_MULTIFR) && 
		(l2->l2_Q921_state != ST_TIMREC)) {
		NDBGL2(L2_I_ERR, "ERROR, state != (MF || TR)!");
		m_freem(m);
		return;
	}
	mtx_lock(&l2->l2_mtx);
/* 
 * update frame count 
 */
	l2->l2_stat.rx_i++;
	
	nr = GETINR(*(ptr + OFF_INR));
	ns = GETINS(*(ptr + OFF_INS));
	p = GETIP(*(ptr + OFF_INR));

	isdn_rxd_ack(l2, nr);		/* last packet ack */
/* 
 * own receiver busy ? 
 */
	if (l2->l2_own_busy)	{
		m_freem(m);	/* yes, discard information */
/* 
 * P bit == 1 ? 
 */
		if (p == 1)	{
			isdn_tx_rnr_resp(l2, p); /* yes, tx RNR */
			l2->l2_ack_pend = 0;	/* clear ACK pending */
		}
	} else {
/* 
 * own receiver ready, where if /* expected sequence number ?  
 */	
		if (ns == l2->l2_vr)	{
			M128INC(l2->l2_vr);	/* yes, update */

			l2->l2_rej_excpt = 0;	/* clr reject exception */

			m_adj(m, I_HDR_LEN);	/* strip i frame header */

			l2->l2_iframe_sent = 0;	/* reset i acked already */

			isdn_dl_data_ind(l3, m);	/* pass data up */

			if (!l2->l2_iframe_sent) {
				isdn_tx_rr_resp(l2, p); /* yes, tx RR */
				l2->l2_ack_pend = 0;	/* clr ACK pending */
			}
		} else {
/* 
 * ERROR, sequence number NOT expected 
 */		
			m_freem(m);	/* discard information */
/* 
 * already exception ? 
 */
			if (l2->l2_rej_excpt == 1) {
/* 
 * immediate resp ? 
 */
				if (p == 1)	{
					isdn_tx_rr_resp(l2, p); /* yes, tx RR */
					l2->l2_ack_pend = 0; /* clr ack pend */
				}
			} else {
/* 
 * not in exception cond 
 */			
				l2->l2_rej_excpt = 1;	/* set exception */
				isdn_tx_rej_resp(l2, p);	/* tx REJ */
				l2->l2_ack_pend = 0;	/* clr ack pending */
			}
		}
	}
/* 
 * sequence number ranges as expected ? 
 */
	if (isdn_l2_nr_ok(nr, l2->l2_va, l2->l2_vs)) {
		if (l2->l2_Q921_state == ST_TIMREC) {
			l2->l2_va = nr;
		} else {
/* 
 * yes, other side busy ? 
 */
			if (l2->l2_peer_busy) {
				l2->l2_va = nr;	/* yes, update ack count */
			} else {
/* 
 * other side ready 
 */		
				if (nr == l2->l2_vs) {
/* 
 * count expected ? 
 */								
					l2->l2_va = nr;	/* update ack */
					isdn_T200_stop(l2);
					isdn_T203_restart(l2);
				} else {
					if (nr != l2->l2_va) {
						l2->l2_va = nr;
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
		l2->l2_Q921_state = ST_AW_EST;
	}
	mtx_unlock(&l2->l2_mtx);
}

/*---------------------------------------------------------------------------*
 *	internal I FRAME QUEUED UP routine (Q.921 03/93 p 61)
 *---------------------------------------------------------------------------*/
void
isdn_l2_queue_i_frame(struct isdn_l2 *l2)
{
	struct mbuf *m;
	u_char *ptr;

	if ((l2->l2_peer_busy) || 
		(l2->l2_vs == ((l2->l2_va + MAX_K_VALUE) & 127))) {
		if (l2->l2_peer_busy) {
			NDBGL2(L2_I_MSG, "regen IFQUP, cause: peer busy!");
		}

		if (l2->l2_vs == ((l2->l2_va + MAX_K_VALUE) & 127)) {
			NDBGL2(L2_I_MSG, "regen IFQUP, cause: vs=va+k!");
		}

		/*
		 * XXX see: Q.921, page 36, 5.6.1 ".. may retransmit an I
		 * frame ...", shall we retransmit the last i frame ?
		 */

		if (!(_IF_QEMPTY(&l2->l2_i_queue))) {
			NDBGL2(L2_I_MSG, "re-scheduling IFQU call!");
			START_TIMER(l2->l2_IFQU_callout, 
			isdn_l2_queue_i_frame, l2, IFQU_DLY);
		}
		return;
	}
 /* 
  * fetch next frame to tx 
  */
	IF_DEQUEUE(&l2->l2_i_queue, m);   

	if (!m) {
		NDBGL2(L2_I_ERR, "ERROR, mbuf NULL after IF_DEQUEUE");
		return;
	}

	ptr = m->m_data;

	PUTSAPI(SAPI_CCP, CR_CMD_TO_NT, *(ptr + OFF_SAPI));
	PUTTEI(l2->l2_tei, *(ptr + OFF_TEI));

	*(ptr + OFF_INS) = (l2->l2_vs << 1) & 0xfe; /* bit 0 = 0 */
	*(ptr + OFF_INR) = (l2->l2_vr << 1) & 0xfe; /* P bit = 0 */

	l2->l2_stat.tx_i++;	/* update frame counter */
/* 
 * XXX: wrong ... free'd when ack'd ! 
 */
	isdn_output(l2, m, MBUF_DONTFREE); 
/*
 * in case we ack an I frame with another I frame 
 */		
	l2->l2_iframe_sent = 1;		

	if (l2->l2_ua_num != UA_EMPTY) {
/* 
 * failsafe 
 */	
		NDBGL2(L2_I_ERR, "ERROR, l2->l2_ua_num: %d != UA_EMPTY", l2->l2_ua_num);
		
		isdn_print_l2var(sc);
		
		m_freem(l2->l2_ua_frame);
	}

	l2->l2_ua_frame = m;		/* save unacked frame */
	l2->l2_ua_num = l2->l2_vs;	/* save frame number */

	M128INC(l2->l2_vs);

	l2->l2_ack_pend = 0;

	if (l2->l2_T200 == TIMER_IDLE) {
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
	u_char *ptr;
	struct isdn_l2 *l2;
	
	ptr = m->m_data;
	l2 = &sc->sc_l2;

	if (!((l2->l2_tei_valid == TEI_VALID) &&
	     (l2->l2_tei == GETTEI(*(ptr+OFF_TEI))))) {
		goto out;
	}

	l2->l2_rxd_CR = GETCR(*(ptr + OFF_SAPI));
	l2->l2_rxd_PF = GETSPF(*(ptr + OFF_SNR));
	l2->l2_rxd_NR = GETSNR(*(ptr + OFF_SNR));

	isdn_rxd_ack(l2, l2->l2_rxd_NR);

	switch(*(ptr + OFF_SRCR)) {
	case RR:
		l2->l2_stat.rx_rr++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd RR, N(R) = %d", l2->l2_rxd_NR);
		isdn_next_l2state(l2, l3, EV_RXRR);
		break;
	case RNR:
		l2->l2_stat.rx_rnr++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd RNR, N(R) = %d", l2->l2_rxd_NR);
		isdn_next_l2state(l2, l3, EV_RXRNR);
		break;
	case REJ:
		l2->l2_stat.rx_rej++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd REJ, N(R) = %d", l2->l2_rxd_NR);
		isdn_next_l2state(l2, l3, EV_RXREJ);
		break;
	default:
		l2->l2_stat.err_rx_bads++; /* update statistics */
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
isdn_l2_tx_rr_cmd(struct isdn_l2 *l2, pbit_t pbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RR, isdnif = %d", l2->l2_l3->isdnif);

	m = isdn_l2_build_s_frame(l2, CR_CMD_TO_NT, pbit, RR);

	isdn_output(l2, m, MBUF_FREE);

	l2->l2_stat.tx_rr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RR resp
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_rr_resp(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RR, isdnif = %d", l2->l2_l3->isdnif);

	m = isdn_l2_build_s_frame(l2, CR_RSP_TO_NT, fbit, RR);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);

	l2->l2_stat.tx_rr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RNR cmd
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_rnr_cmd(struct isdn_l2 *l2, pbit_t pbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RNR, isdnif = %d", l2->l2_l3->isdnif);

	m = isdn_l2_build_s_frame(l2, CR_CMD_TO_NT, pbit, RNR);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);

	l2->l2_stat.tx_rnr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RNR resp
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_rnr_resp(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RNR, isdnif = %d", l2->l2_l3->isdnif);

	m = isdn_l2_build_s_frame(l2, CR_RSP_TO_NT, fbit, RNR);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);

	l2->l2_stat.tx_rnr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit REJ resp
 *---------------------------------------------------------------------------*/
void
isdn__tx_rej_resp(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx REJ, isdnif = %d", l2->l2_l3->isdnif);

	m = isdn_l2_build_s_frame(l2, CR_RSP_TO_NT, fbit, REJ);
/*
 * XXX: wrong ...
 */
	isdn_output(l2, m, MBUF_FREE);

	l2->l2_stat.tx_rej++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	build S-frame for sending
 *---------------------------------------------------------------------------*/
struct mbuf *
isdn_l2_build_s_frame(struct isdn_l2 *l2, crbit_to_nt_t crbit, 
	pbit_t pbit, u_char type)
{
	struct mbuf *m;

	if ((m = isdn_getmbuf(S_FRAME_LEN, M_DONTWAIT, MT_I4B_D)) == NULL)
		goto out;

	PUTSAPI(SAPI_CCP, crbit, m->m_data[OFF_SAPI]);

	PUTTEI(l2->l2_tei, m->m_data[OFF_TEI]);

	m->m_data[OFF_SRCR] = type;

	m->m_data[OFF_SNR] = (l2->l2_vr << 1) | (pbit & 0x01);
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
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			l2->stat.rx_sabme++;
			NDBGL2(L2_U_MSG, "SABME, sapi = %d, tei = %d", sapi, tei);
			l2->rxd_PF = pfbit;
			i4b_next_l2state(l2, l3, EV_RXSABME);
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
			l2->stat.rx_tei++;
			i4b_tei_rxframe(l2, l3, m);
		} else if ((sapi == SAPI_CCP) && (tei == GROUP_TEI)) {
/* 
 * call control (SAPI = 0) 
 */
			l2->stat.rx_ui++;
/* 
 * strip ui header 
 */
			m_adj(m, UI_HDR_LEN);
/* 
 * to upper layer 
 */
			i4b_dl_unit_data_ind(l2->l3, m);
		} else {
			l2->stat.err_rx_badui++;
			NDBGL2(L2_U_ERR, "unknown UI frame!");
			m_freem(m);
		}
		break;
	case DISC:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			l2->stat.rx_disc++;
			NDBGL2(L2_U_MSG, "DISC, sapi = %d, tei = %d", sapi, tei);
			l2->rxd_PF = pfbit;
			i4b_next_l2state(l2, l3, EV_RXDISC);
		}
		m_freem(m);
		break;
	case XID:
		
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			l2->stat.rx_xid++;
			NDBGL2(L2_U_MSG, "XID, sapi = %d, tei = %d", sapi, tei);
		}
		m_freem(m);
		break;
/* 
 * responses 
 */
	case DM:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
				l2->stat.rx_dm++;
			NDBGL2(L2_U_MSG, "DM, sapi = %d, tei = %d", sapi, tei);
			i4b_print_frame(m->m_len, m->m_data);
			l2->rxd_PF = pfbit;
			i4b_next_l2state(l2, l3, EV_RXDM);
		}
		m_freem(m);
		break;
	case UA:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			l2->stat.rx_ua++;
			NDBGL2(L2_U_MSG, "UA, sapi = %d, tei = %d", sapi, tei);
			l2->rxd_PF = pfbit;
			i4b_next_l2state(l2, l3, EV_RXUA);
		}
		m_freem(m);
		break;
	case FRMR:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			l2->stat.rx_frmr++;
			NDBGL2(L2_U_MSG, "FRMR, sapi = %d, tei = %d", sapi, tei);
			l2->rxd_PF = pfbit;
			i4b_next_l2state(l2, l3, EV_RXFRMR);
		}
		m_freem(m);
		break;
	default:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			NDBGL2(L2_U_ERR, "UNKNOWN TYPE ERROR, sapi = %d, "
				"tei = %d, frame = ", sapi, tei);
			i4b_print_frame(m->m_len, m->m_data);
		} else {
			NDBGL2(L2_U_ERR, "not mine -  UNKNOWN TYPE ERROR, "
				"sapi = %d, tei = %d, frame = ", sapi, tei);
			i4b_print_frame(m->m_len, m->m_data);
		}
		l2->stat.err_rx_badui++;
		m_freem(m);
		break;
	}
}

/*---------------------------------------------------------------------------*
 *	build U-frame for sending
 *---------------------------------------------------------------------------*/
struct mbuf *
isdn_l2_build_u_frame(struct isdn_l2 *l2, crbit_to_nt_t crbit, 
	pbit_t pbit, u_char type)
{
	struct mbuf *m;

	if ((m = i4b_getmbuf(U_FRAME_LEN, M_DONTWAIT, MT_I4B_D)) == NULL)
		goto out;

	PUTSAPI(SAPI_CCP, crbit, m->m_data[OFF_SAPI]);

	PUTTEI(l2->tei, m->m_data[OFF_TEI]);

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
i4b_tx_sabme(struct isdn_l2 *l2, pbit_t pbit)
{
	struct mbuf *m;

	l2->stat.tx_sabme++;
	NDBGL2(L2_U_MSG, "tx SABME, tei = %d", l2->tei);
	m = isdn_l2_build_u_frame(l2, CR_CMD_TO_NT, pbit, SABME);
/*
 * XXX: wrong ...
 */
	i4b_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit DM response
 *---------------------------------------------------------------------------*/
void
i4b_tx_dm(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	l2->stat.tx_dm++;
	NDBGL2(L2_U_MSG, "tx DM, tei = %d", l2->tei);
	m = isdn_l2_build_u_frame(l2, CR_RSP_TO_NT, fbit, DM);
/*
 * XXX: wrong ...
 */
	i4b_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit DISC command
 *---------------------------------------------------------------------------*/
void
i4b_tx_disc(struct isdn_l2 *l2, pbit_t pbit)
{
	struct mbuf *m;

	l2->stat.tx_disc++;
	NDBGL2(L2_U_MSG, "tx DISC, tei = %d", l2->tei);
	m = isdn_l2_build_u_frame(l2, CR_CMD_TO_NT, pbit, DISC);
/*
 * XXX: wrong ...
 */
	i4b_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit UA response
 *---------------------------------------------------------------------------*/
void
i4b_tx_ua(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	l2->stat.tx_ua++;
	NDBGL2(L2_U_MSG, "tx UA, tei = %d", l2->tei);
	m = isdn_l2_build_u_frame(l2, CR_RSP_TO_NT, fbit, UA);
/*
 * XXX: wrong ...
 */
	i4b_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit FRMR response
 *---------------------------------------------------------------------------*/
void
i4b_tx_frmr(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	l2->stat.tx_frmr++;
	NDBGL2(L2_U_MSG, "tx FRMR, tei = %d", l2->tei);
	m = isdn_l2_build_u_frame(l2, CR_RSP_TO_NT, fbit, FRMR);
/*
 * XXX: wrong ...
 */
	i4b_output(l2, m, MBUF_FREE);
}

