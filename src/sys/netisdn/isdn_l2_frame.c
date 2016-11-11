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

static void 	isdn_l2_ack_pending(struct isdn_softc *);
static void 	isdn_l2_print_var(struct isdn_softc *);
static void 	isdn_l2_rxd_ack(struct isdn_softc *, int);

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

/*---------------------------------------------------------------------------*
 *	internal I FRAME QUEUED UP routine (Q.921 03/93 p 61)
 *---------------------------------------------------------------------------*/
void
isdn_l2_queue_i_frame(struct isdn_softc *sc)
{
	struct sockaddr_isdn sisdn;
	struct mbuf *m, *n;
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
	bzero(&sisdn, sizeof(sisdn));
	
	sisdn.sisdn_type = AF_ISDN;
	sisdn.sisdn_len = sizeof(sisdn);
	sisdn.sisdn_rd.rd_chan = ISDN_D_CHAN;
	sisdn.sisdn_rd.rd_sapi = SAPI_CCP;
	sisdn.sisdn_rd.rd_tei = sc->sc_l2.l2_tei;
	
	ptr = m->m_data;

	PUTSAPI(sisdn.sisdn_rd.rd_sapi, CR_CMD_TO_NT, *(ptr + OFF_SAPI));
	PUTTEI(sisdn.sisdn_rd.rd_tei, *(ptr + OFF_TEI));

	*(ptr + OFF_INS) = (sc->sc_l2.l2_vs << 1) & 0xfe; /* bit 0 = 0 */
	*(ptr + OFF_INR) = (sc->sc_l2.l2_vr << 1) & 0xfe; /* P bit = 0 */

	sc->sc_l2.l2_stat.tx_i++;	/* update frame counter */
/*
 * Transmit writable copy of I frame.
 */
 	if ((n = m_dup(m, M_NOWAIT)) != NULL) 
 		(void)isdn_output(sc->sc_ifp, n, &sisdn);
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
isdn_l2_tx_rej_resp(struct isdn_softc *sc, fbit_t fbit)
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

/*---------------------------------------------------------------------------*
 *	Tx U-frame
 *---------------------------------------------------------------------------*/
int  
isdn_l2_tx_u_frame(struct isdn_softc *sc, crbit_to_nt_t crbit, 
	pbit_t pbit, uint8_t type)
{
	struct sockaddr_isdn sisdn;
	struct mbuf *m;
	int error;
	
	bzero(&sisdn, sizeof(sisdn));
	
	sisdn.sisdn_type = AF_ISDN;
	sisdn.sisdn_len = sizeof(sisdn);
	sisdn.sisdn_rd.rd_chan = ISDN_D_CHAN;
	sisdn.sisdn_rd.rd_sapi = SAPI_CCP;
	sisdn.sisdn_rd.rd_tei = sc->sc_l2.l2_tei;
	
	if ((m = isdn_getmbuf(U_FRAME_LEN, M_DONTWAIT, MT_I4B_D)) == NULL) {
		error = ENOBUFS;
		goto out;
	}
	
	switch (type) {
/*
 * tx SABME command
 */	
		sc->sc_l2.l2_stat.tx_sabme++;
		NDBGL2(L2_U_MSG, "tx SABME, tei = %d", sc->sc_l2.l2_tei);
		break;
	case DM:
/*
 * tx DM response
 */	
		sc->sc_l2.l2_stat.tx_dm++;
		NDBGL2(L2_U_MSG, "tx DM, tei = %d", sc->sc_l2.l2_tei);
		break;
	case DISC:
/*
 * tx DISC cmd
 */		
		sc->sc_l2.l2_stat.tx_disc++;
		NDBGL2(L2_U_MSG, "tx DISC, tei = %d", sc->sc_l2.l2_tei);
		break;
	case UA:
/*
 * tx UA response
 */			
		sc->sc_l2.l2_stat.tx_ua++;
		NDBGL2(L2_U_MSG, "tx UA, tei = %d", sc->sc_l2.l2_tei);
		break;
	case FRMR:	
/*
 * tx FRMR response
 */
		sc->sc_l2.l2_stat.tx_frmr++;
		NDBGL2(L2_U_MSG, "tx FRMR, tei = %d", sc->sc_l2.l2_tei);
		break;
	default:
		error = EINVAL;
		m_freem(m);
		goto out;
	}
	PUTSAPI(sisdn.sisdn_rd.rd_sapi, crbit, m->m_data[OFF_SAPI]);

	PUTTEI(sisdn.sisdn_rd.rd_tei, m->m_data[OFF_TEI]);

	if (pbit)
		m->m_data[OFF_CNTL] = type | UPBITSET;
	else
		m->m_data[OFF_CNTL] = type & ~UPBITSET;
	
	error = isdn_output(sc->sc_ifp, m, &sisdn);	
out:
	return (error);
}

/*
 * Subr.
 */

/*---------------------------------------------------------------------------*
 *	isdn_l2_print_var - print some l2softc vars
 *---------------------------------------------------------------------------*/
static void
isdn_l2_print_var(struct isdn_softc *sc)
{	
	NDBGL2(L2_ERROR, "isdnif %d V(R)=%d, V(S)=%d, "
		"V(A)=%d,ACKP=%d,PBSY=%d,OBSY=%d",
		sc->sc_ifp->if_index,
		sc->sc_l2.l2_vr,
		sc->sc_l2.l2_vs,
		sc->sc_l2.l2_va,
		sc->sc_l2.l2_ack_pend,
		sc->sc_l2.l2_peer_busy,
		sc->sc_l2.l2_own_busy);
}

/*---------------------------------------------------------------------------*
 *	routine ACKNOWLEDGE PENDING (Q.921 03/93 p 70)
 *---------------------------------------------------------------------------*/
static void
isdn_l2_ack_pending(struct isdn_softc *sc)
{
	if (sc->sc_l2.l2_ack_pend) {
		sc->sc_l2.l2_ack_pend = 0;
		isdn_l2_tx_rr_resp(sc, F0);
	}
}

/*---------------------------------------------------------------------------*
 *	got s or i frame, check if valid ack for last sent frame
 *---------------------------------------------------------------------------*/
static void
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

