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

#include "opt_inet.h" 
#include "opt_isdn.h" 
 
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
#include <net/if_var.h>

#include <netisdn/i4b.h>
#include <netisdn/i4b_var.h>

static void 	isdn_ack_pending(struct isdn_softc *);
static void 	isdn_print_var(struct isdn_softc *);
static void 	isdn_tx_i_frame(struct isdn_softc *, struct mbuf *);

/*---------------------------------------------------------------------------*
 *	isdn_print_var - print some l2softc vars
 *---------------------------------------------------------------------------*/
static void
isdn_print_var(struct isdn_softc *sc)
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
isdn_ack_pending(struct isdn_softc *sc)
{
	if (sc->sc_l2.l2_ack_pend) {
		sc->sc_l2.l2_ack_pend = 0;
		isdn_l2_tx_rr_resp(sc, F0);
	}
}

/*---------------------------------------------------------------------------*
 *	internal I FRAME QUEUED UP routine (Q.921 03/93 p 61)
 *---------------------------------------------------------------------------*/
void
isdn_queue_i_frame(struct isdn_softc *sc)
{
	if ((sc->sc_l2.l2_peer_busy) || 
		(sc->sc_l2.l2_vs == ((sc->sc_l2.l2_va + MAX_K_VALUE) & 127))) {
		
		if (sc->sc_l2.l2_peer_busy) {
			NDBGL2(L2_I_MSG, 
				"%s: IFQUP, cause: peer busy!", __func__);
		}

		if (sc->sc_l2.l2_vs == ((sc->sc_l2.l2_va + MAX_K_VALUE) & 127)) {
			NDBGL2(L2_I_MSG, 
				"%s: IFQUP, cause: vs=va+k!", __func__);
		}
/*
 * XXX: see: Q.921, page 36, 5.6.1 ".. may retransmit an I
 * XXX: frame ...", shall we retransmit the last i frame ?
 */
		if (!(_IF_QEMPTY(&sc->sc_l2.l2_i_queue))) {
			NDBGL2(L2_I_MSG, 
				"%s: re-scheduling IFQU call!", __func__);
			START_TIMER(sc->sc_l2.l2_IFQU_callout, 
				isdn_queue_i_frame, sc, IFQU_DLY);
		}	
	} else {
/* 
 * Fetch next frame for transmission. 
 */
		IF_DEQUEUE(&sc->sc_l2.l2_i_queue, m);   

		if (m == NULL) {
			NDBGL2(L2_I_ERR, "%s: mbuf NULL after "
				"IF_DEQUEUE", __func__);
		} else
			isdn_tx_i_frame(sc, m);
	}
}

/*
 * Transmit I Frame
 */
static void 	
isdn_tx_i_frame(struct isdn_softc *sc, struct mbuf *m)
{
	struct mbuf *n;
	uint8_t *ptr;

	SC_WLOCK(sc);
	
	ptr = mtod(m, uint8_t *);

	PUTSAPI(SAPI_CCP, CR_CMD_TO_NT, *(ptr + OFF_SAPI));
	PUTTEI(sc->sc_l2.l2_tei, *(ptr + OFF_TEI));

	*(ptr + OFF_INS) = (sc->sc_l2.l2_vs << 1) & 0xfe; /* bit 0 = 0 */
	*(ptr + OFF_INR) = (sc->sc_l2.l2_vr << 1) & 0xfe; /* P bit = 0 */

	sc->sc_l2.l2_stat.tx_i++;	/* update frame counter */
/*
 * Transmit writable copy of frame.
 */
 	if ((n = m_dup(m, M_NOWAIT)) != NULL) {
 		(void)isdn_output(sc->sc_ifp, n, 
 			ISDN_D_CHAN, 0, SAPI_CCP, sc->sc_l2.l2_tei);
 	}	
/*
 * in case we ack an I frame with another I frame 
 */	
	sc->sc_l2.l2_iframe_sent = 1;

	if (sc->sc_l2.l2_ua_num != UA_EMPTY) {
/* 
 * failsafe 
 */	
		NDBGL2(L2_I_ERR, "%s: sc->sc_l2.l2_ua_num: %d != UA_EMPTY", 
			__func__, sc->sc_l2.l2_ua_num);
/*
 * XXX
 */		
		isdn_print_var(sc);
		
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

	if (sc->sc_l2.l2_T200 == TIMER_IDLE) {
		isdn_T203_stop(l2);
		isdn_T200_start(l2);
	}
	SC_WUNLOCK(sc);
}

/*
 * Transmit S frame
 */
int  
isdn_tx_s_frame(struct isdn_softc *sc, crbit_to_nt_t crbit, 
	pbit_t pbit, uint8_t type)
{
	struct mbuf *m;
	int error;
	
	if ((m = isdn_getmbuf(S_FRAME_LEN, M_NOWAIT, MT_I4B_D)) == NULL) {
		error = ENOBUFS;
		goto out;
	}
	
	switch (type) {
	case RR:
		NDBGL2(L2_S_MSG, "tx RR, isdnif = %d", sc->sc_ifp->if_index);
		break;
	case RNR:
		NDBGL2(L2_S_MSG, "tx RNR, isdnif= %d", sc->sc_ifp->if_index);
		break;
	case REJ:
		NDBGL2(L2_S_MSG, "tx REJ, isdnif = %d", sc->sc_ifp->if_index);
		break;
	default:
		error = EINVAL;
		m_freem(m);
		goto out;
	}
	
	PUTSAPI(SAPI_CCP, crbit, m->m_data[OFF_SAPI]);

	PUTTEI(sc->sc_l2.l2_tei, m->m_data[OFF_TEI]);

	m->m_data[OFF_SRCR] = type;

	m->m_data[OFF_SNR] = (sc->sc_l2.l2_vr << 1) | (pbit & 0x01);
	
	error = isdn_output(sc->sc_ifp, m, 
		ISDN_D_CHAN, 0, SAPI_CCP, sc->sc_l2.l2_tei);	
	
	switch (type) {
	case RR:
		sc->sc_l2.l2_stat.tx_rr++; /* update statistics */
		break;
	case RNR:
		sc->sc_l2.l2_stat.tx_rnr++; /* update statistics */
		break;
	case REJ:
		sc->sc_l2.l2_stat.tx_rej++; /* update statistics */
		break;
	default:
		break;
	}
out:
	return (error);
}

/*
 * Transmit U frame
 */
int  
isdn_tx_u_frame(struct isdn_softc *sc, crbit_to_nt_t crbit, 
	pbit_t pbit, uint8_t type)
{
	struct mbuf *m;
	int error;
	
	if ((m = isdn_getmbuf(U_FRAME_LEN, M_NOWAIT, MT_I4B_D)) == NULL) {
		error = ENOBUFS;
		goto out;
	}
	
	switch (type) {
	case SABME:
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
	
	PUTSAPI(SAPI_CCP, crbit, m->m_data[OFF_SAPI]);

	PUTTEI(sc->sc_l2.l2_tei, m->m_data[OFF_TEI]);

	if (pbit)
		m->m_data[OFF_CNTL] = type | UPBITSET;
	else
		m->m_data[OFF_CNTL] = type & ~UPBITSET;
	
	error = isdn_output(sc->sc_ifp, m, 
		ISDN_D_CHAN, 0, SAPI_CCP, sc->sc_l2.l2_tei);	
out:
	return (error);
}

/*
 * Handoff to IEE802.{3,11} link-layer.
 */

int
isdn_output(struct ifnet *ifp, struct mbuf *m, uint8_t chan, 
	uint8_t proto, uint8_t sapi, uint8_t tei)
{	
	struct sockaddr_isdn sisdn;
	struct isdn_rd *rd;
	int error;

	bzero(&sisdn, sizeof(sisdn));
	
	sisdn.sisdn_type = AF_ISDN;
	sisdn.sisdn_len = sizeof(sisdn);
	sisdn.sisdn_rd.rd_chan = chan;
	sisdn.sisdn_rd.rd_proto = proto;
	sisdn.sisdn_rd.rd_sapi = sapi;
	sisdn.sisdn_rd.rd_tei = tei;

	M_PREPEND(m, sizeof(*rd), (M_ZERO|M_NOWAIT));
	if (m == NULL) {
		error = ENOBUFS;
		goto out;
	}		
	rd = mtod(m, struct isdn_rd *);
	rd->rd_chan = chan;
	rd->rd_proto = proto;
	rd->rd_sapi = sapi; 	
	rd->rd_tei = tei;	 
/*
 * XXX ...
 */
	error = (*ifp->if_output)
		(ifp, m, (const struct sockaddr *)&sisdn, NULL);
out:	
	return (error);
}


