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
 *	i4b_iframe.c - i frame handling routines
 *	------------------------------------------
 *
 *	$Id: isdn_iframe.c,v 1.8 2005/12/11 12:25:06 christos Exp $
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

#include "isdnq921.h"

#if NI4BQ921

#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/isdn_debug.h>
#include <netisdn/isdn_ioctl.h>
#include <netisdn/isdn_trace.h>

#include <netisdn/isdn_global.h>
#include <netisdn/isdn_l2.h>
#include <netisdn/isdn_l1l2.h>
#include <netisdn/isdn_isdnq931.h>
#include <netisdn/isdn_mbuf.h>
#include <netisdn/isdn_l2fsm.h>

/*---------------------------------------------------------------------------*
 *	process i frame
 *	implements the routine "I COMMAND" Q.921 03/93 pp 68 and pp 77
 *---------------------------------------------------------------------------*/
void
isdn_rxd_i_frame(struct isdn_sc *sc, struct mbuf *m)
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
isdn_i_frame_queued_up(struct isdn_l2 *l2)
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

		if (!(IF_QEMPTY(&l2->l2_i_queue))) {
			NDBGL2(L2_I_MSG, "re-scheduling IFQU call!");
			START_TIMER(l2->l2_IFQU_callout, 
			isdn_i_frame_queued_up, l2, IFQU_DLY);
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
 * free'd when ack'd ! 
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
		isdn_print_l2var(l2);
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
