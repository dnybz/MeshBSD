/* $NetBSD: isdn_l2.c,v 1.24 2008/07/20 01:05:27 martin Exp $ */

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
 *
 *---------------------------------------------------------------------------
 *
 *      i4b_l2.c - ISDN layer 2 (Q.921)
 *	-------------------------------
 *
 *	$Id: isdn_l2.c,v 1.24 2008/07/20 01:05:27 martin Exp $
 *
 * $FreeBSD$
 *
 *      last edit-date: [Fri Jan  5 11:33:47 2001]
 *
 *---------------------------------------------------------------------------*/
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
#include "opt_isdn_debug.h"

#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>
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




/*---------------------------------------------------------------------------*
 *	routine ESTABLISH DATA LINK (Q.921 03/93 page 83)
 *---------------------------------------------------------------------------*/
void
isdn_l2_establish(struct isdn_softc *sc)
{
	isdn_clear_exception_cond(sc);

	sc->sc_l2.l2_RC = 0;

	isdn_l2_tx_sabme(sc, P1);

	isdn_T200_restart(&sc->sc_l2);

	isdn_T203_stop(&sc->sc_l2);
}

/*---------------------------------------------------------------------------*
 *	routine CLEAR EXCEPTION CONDITIONS (Q.921 03/93 page 83)
 *---------------------------------------------------------------------------*/
void
isdn_l2_clear_exception_cond(struct isdn_softc *sc) 
{
	SC_WLOCK(sc);

/*XXX -------------------------------------------------------------- */
/*XXX is this really appropriate here or should it moved elsewhere ? */

	IF_DRAIN(&sc->sc_l2.l2_i_queue);

	if (sc->sc_l2.l2_ua_num != UA_EMPTY) {
		m_freem(sc->sc_l2.l2_ua_frame);
		sc->sc_l2.l2_ua_num = UA_EMPTY;
	}
	sc->sc_l2.l2_peer_busy = 0;
	sc->sc_l2.l2_rej_excpt = 0;
	sc->sc_l2.l2_own_busy = 0;
	sc->sc_l2.l2_ack_pend = 0;

	SC_WUNLOCK(sc);
}

/*---------------------------------------------------------------------------*
 *	routine TRANSMIT ENQUIRE (Q.921 03/93 page 83)
 *---------------------------------------------------------------------------*/
void
isdn_l2_tx_enquire(struct isdn_softc *sc)
{
	if (sc->sc_l2.l2_own_busy)
		isdn_l2_tx_rnr_cmd(sc, P1);
	else
		isdn_l2_tx_rr_cmd(sc, P1);

	sc->sc_l2.l2_ack_pend = 0;

	isdn_T200_start(&sc->sc_l2);
}

/*---------------------------------------------------------------------------*
 *	routine NR ERROR RECOVERY (Q.921 03/93 page 83)
 *---------------------------------------------------------------------------*/
void
isdn_l2_nr_error_recovery(struct isdn_softc *sc)
{
/*
 * XXX ... LME
 
	isdn_mdl_error_ind(l2, "isdn_l2_nr_error_recovery", MDL_ERR_J);
 */
	isdn_l2_establish(sc);

	sc->sc_l2.l2_l3_init = 0;
}

/*---------------------------------------------------------------------------*
 *	routine ENQUIRY RESPONSE (Q.921 03/93 page 84)
 *---------------------------------------------------------------------------*/
void
isdn_l2_enquiry_resp(struct isdn_softc *sc)
{
	if (sc->sc_l2.l2_own_busy)
		isdn_l2_tx_rnr_resp(sc, F1);
	else
		isdn_l2_tx_rr_resp(sc, F1);

	sc->sc_l2.l2_ack_pend = 0;
}

/*---------------------------------------------------------------------------*
 *	routine INVOKE RETRANSMISSION (Q.921 03/93 page 84)
 *---------------------------------------------------------------------------*/
void
isdn_l2_invoke_rtx(struct isdn_softc *sc, int nr)
{
	NDBGL2(L2_ERROR, "nr = %d", nr);

	while (sc->sc_l2.l2_vs != nr) {
		NDBGL2(L2_ERROR, "nr(%d) != vs(%d)", nr, sc->sc_l2.l2_vs);

		M128DEC(sc->sc_l2.l2_vs);

/* XXXXXXXXXXXXXXXXX */

		if ((sc->sc_l2.l2_ua_num != UA_EMPTY) && 
			(sc->sc_l2.l2_vs == sc->sc_l2.l2_ua_num)) {
			
			if (_IF_QFULL(&sc->sc_l2.l2_i_queue)) 
				NDBGL2(L2_ERROR, "ERROR, I-queue full!");
			else {
				IF_ENQUEUE(&sc->sc_l2.l2_i_queue, 
					sc->sc_l2.l2_ua_frame);
				sc->sc_l2.l2_ua_num = UA_EMPTY;
			}
		} else {
			NDBGL2(L2_ERROR, "ERROR, l2->vs = %d, "
				"l2->ua_num = %d ", 
				sc->sc_l2.l2_vs, 
				sc->sc_l2.l2_ua_num);
		}

/* XXXXXXXXXXXXXXXXX */

		isdn_l2_queue_i_frame(sc);
	}
}

/*---------------------------------------------------------------------------*
 *	DL_ESTABLISH_REQ from layer 3
 *---------------------------------------------------------------------------*/
int 
isdn_l2_establish_req(struct isdn_softc *sc)
{
	
	NDBGL2(L2_PRIM, "isdnif %d", sc->sc_ifp->if_index);
	
	isdn_next_l2state(sc, EV_DLESTRQ);
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL_RELEASE_REQ from layer 3
 *---------------------------------------------------------------------------*/
int 
isdn_l2_release_req(struct isdn_softc *sc)
{
	NDBGL2(L2_PRIM, "isdnif %d", sc->sc_ifp->if_index);
	isdn_next_l2state(sc, EV_DLRELRQ);
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL UNIT DATA REQUEST from Layer 3
 *---------------------------------------------------------------------------*/
int 
isdn_l2_unit_data_req(struct isdn_softc *sc, struct mbuf *m)
{
#ifdef NOTDEF
	NDBGL2(L2_PRIM, "isdnif %d", sc->sc_ifp->if_index);
#endif
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL DATA REQUEST from Layer 3
 *---------------------------------------------------------------------------*/
int 
isdn_l2_data_req(struct isdn_softc *sc, struct mbuf *m)
{
	struct isdn_l2 *l2 = &sc->sc_l2;
	int error = 0;

	switch(sc->sc_l2.l2_Q921_state) {
	case ST_AW_EST:
	case ST_MULTIFR:
	case ST_TIMREC:

		IFQ_ENQUEUE(&sc->sc_l2.l2_i_queue, m, error);
		
		if (error) 
			NDBGL2(L2_ERROR, "i_queue full!!");
		else
			isdn_l2_queue_i_frame(sc);
		
		break;
	default:
		NDBGL2(L2_ERROR, "isdnif %d ERROR in state [%s], "
			"freeing mbuf", sc->sc_ifp->if_index, 
			isdn_l2_print_state(l2));
		m_freem(m);
		error = EINVAL;
		break;
	}
	return (error);
}

/*---------------------------------------------------------------------------*
 *	isdn_l2_init - place layer 2 unit into known state
 *---------------------------------------------------------------------------*/
static void
isdn_l2_init(struct isdn_l2 *l2)
{
	sc->sc_l2.l2_Q921_state = ST_TEI_UNAS;
	sc->sc_l2.l2_tei_valid = TEI_INVALID;
	sc->sc_l2.l2_vr = 0;
	sc->sc_l2.l2_vs = 0;
	sc->sc_l2.l2_va = 0;
	sc->sc_l2.l2_ack_pend = 0;
	sc->sc_l2.l2_rej_excpt = 0;
	sc->sc_l2.l2_peer_busy = 0;
	sc->sc_l2.l2_own_busy = 0;
	sc->sc_l2.l2_l2_l3_init = 0;

	sc->sc_l2.l2_rxd_CR = 0;
	sc->sc_l2.l2_rxd_PF = 0;
	sc->sc_l2.l2_rxd_NR = 0;
	sc->sc_l2.l2_RC = 0;
	sc->sc_l2.l2_iframe_sent = 0;

	sc->sc_l2.l2_postfsmfunc = NULL;

	if (sc->sc_l2.l2_ua_num != UA_EMPTY) {
		m_freem(sc->sc_l2.l2_ua_frame);
		sc->sc_l2.l2_ua_num = UA_EMPTY;
		sc->sc_l2.l2_ua_frame = NULL;
	}
	
	isdn_T200_stop(l2);
	isdn_T202_stop(l2);
	isdn_T203_stop(l2);
}

/*---------------------------------------------------------------------------*
 *	isdn_l2_status_ind - status indication upward
 *---------------------------------------------------------------------------*/
int
isdn_l2_status_ind(struct isdn_softc *sc, 
	int status, int parm)
{
	struct isdn_l2 *l2;
	int sendup;

	NDBGL2(L2_PRIM, "isdnif %d, status=%d, parm=%d", 
		sc->sc_ifp->if_index, status, parm);

	l2 = &sc->sc_l2;
	
	mtx_lock(&sc->sc_l2.l2_mtx);

	sendup = 1;

	switch(status) {
	case STI_ATTACH:
		if (parm == 0) {
/* 
 * detach 
 */
			callout_stop(&sc->sc_l2.l2_T200_callout);
			callout_stop(&sc->sc_l2.l2_T202_callout);
			callout_stop(&sc->sc_l2.l2_T203_callout);
			callout_stop(&sc->sc_l2.l2_IFQU_callout);
			break;
		}
		sc->sc_l2.l2_i_queue.ifq_maxlen = IQUEUE_MAXLEN;
		sc->sc_l2.l2_ua_frame = NULL;

		(void)memset(&sc->sc_l2.l2_stat, 0, sizeof(lapdstat_t));
/* 
 * initialize the callout handles for timeout routines 
 */
		callout_init(&sc->sc_l2.l2_T200_callout, 0);
		callout_init(&sc->sc_l2.l2_T202_callout, 0);
		callout_init(&sc->sc_l2.l2_T203_callout, 0);
		callout_init(&sc->sc_l2.l2_IFQU_callout, 0);

		isdn_l2_init(&l2->l2);
		break;
	case STI_L1STAT:	/* state of layer 1 */
		break;
	case STI_PDEACT:	/* Timer 4 expired */
/*
 * XXX
 */			
 		if ((sc->sc_l2.l2_Q921_state >= ST_AW_EST) &&
			   (sc->sc_l2.l2_Q921_state <= ST_TIMREC)) {
			NDBGL2(L2_ERROR, "isdnif %d, persistent deactivation!", 
				sc->sc_ifp->if_index);
			isdn_l2_init(&l2->l2);
			parm = -1;	/* this is passed as the new
						 * TEI to upper layers */
		} else 
			sendup = 0;
		break;
	case STI_NOL1ACC:
		isdn_l2_init(&l2->l2);
		NDBGL2(L2_ERROR, "isdnif %d, cannot access S0 bus!", 
			sc->sc_ifp->if_index);
		break;
	default:
		NDBGL2(L2_ERROR, "ERROR, isdnif %d, unknown status message!", 
			sc->sc_ifp->if_index);
		break;
	}

	if (sendup)
		isdn_mdl_status_ind(sc, status, parm);  /* send up to layer 3 */

	mtx_unlock(&sc->sc_l2.l2_mtx);

	return (0);
}

/*---------------------------------------------------------------------------*
 *	MDL_COMMAND_REQ from layer 3
 *---------------------------------------------------------------------------*/
int 
isdn_l2_cmd_req(struct isdn_softc *sc, int cmd, void *arg)
{
	struct isdn_l2 *l2;

	NDBGL2(L2_PRIM, "isdnif %d, cmd=%d, arg=%p",
		 sc->sc_ifp->if_index, cmd, arg);

	l2 = &sc->sc_l2;

	switch(cmd) {
	case CMR_DOPEN:
		isdn_l2_init(sc);

		break;
	case CMR_DCLOSE:
		break;
	default;
		break;
	}
/* 
 * XXX pass down Trace cmd to isdn_input 
 *
	if (sc->driver)
		sc->driver->mph_cmd_req(sc->l1_token, cmd, parm);
 */
	return (0);
}

int 
isdn_l2_b_chan_get_state(struct isdn_l3 *l3, int b_chan_id)
{
/*
 * XXX ...
 *	
	struct isdn_l2 *sc = l3->l1_token;
	return (sc->bchan_state[b_chan_id]);	
 *
 *
 */
}

void 
isdn_l2_b_chan_set_state(struct isdn_l3 *l3, 
	int b_chan_id, int state)
{
/*
 * XXX ...
 *	
	struct isdn_l2 *sc = l3->l1_token;
	sc->bchan_state[b_chan_id] = state;

 *
 */
}

/*---------------------------------------------------------------------------*
 *	telephony silence detection
 *---------------------------------------------------------------------------*/

#define TEL_IDLE_MIN (BCH_MAX_DATALEN/2)

int
isdn_l2_b_chan_silence(uint8_t *data, int len)
{
	register int i = 0;
	register int j = 0;
	int error = 1;
/* 
 * count idle bytes 
 */
	for (; i < len; i++) {
		if ((*data >= 0xaa) && (*data <= 0xac))
			j++;
		data++;
	}

#ifdef NOTDEF
	(void)printf("%s: got %d silence bytes in frame\n", __func__, j);
#endif

	if (j < (TEL_IDLE_MIN))
		error = 0;
	
	return (error);
}
