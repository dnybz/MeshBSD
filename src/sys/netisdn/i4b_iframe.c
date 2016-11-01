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
 *	$Id: i4b_iframe.c,v 1.8 2005/12/11 12:25:06 christos Exp $
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

#include "i4bq921.h"

#if NI4BQ921

#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/i4b_debug.h>
#include <netisdn/i4b_ioctl.h>
#include <netisdn/i4b_trace.h>

#include <netisdn/i4b_global.h>
#include <netisdn/i4b_l2.h>
#include <netisdn/i4b_l1l2.h>
#include <netisdn/i4b_isdnq931.h>
#include <netisdn/i4b_mbuf.h>
#include <netisdn/i4b_l2fsm.h>

/*---------------------------------------------------------------------------*
 *	process i frame
 *	implements the routine "I COMMAND" Q.921 03/93 pp 68 and pp 77
 *---------------------------------------------------------------------------*/
void
i4b_rxd_i_frame(struct isdn_l2 *l2, struct isdn_l3 *l3, struct mbuf *m)
{
	u_char *ptr = m->m_data;
	int nr;
	int ns;
	int p;

	if (!((l2->tei_valid == TEI_VALID) &&
	     (l2->tei == GETTEI(*(ptr+OFF_TEI))))) {
		i4b_Dfreembuf(m);
		return;
	}

	if ((l2->Q921_state != ST_MULTIFR) && 
		(l2->Q921_state != ST_TIMREC)) {
		i4b_Dfreembuf(m);
		NDBGL2(L2_I_ERR, "ERROR, state != (MF || TR)!");
		return;
	}

	mtx_lock(&i4b_mtx);

	l2->stat.rx_i++;		/* update frame count */

	nr = GETINR(*(ptr + OFF_INR));
	ns = GETINS(*(ptr + OFF_INS));
	p = GETIP(*(ptr + OFF_INR));

	i4b_rxd_ack(l2, l3, nr);		/* last packet ack */
/* 
 * own receiver busy ? 
 */
	if (l2->own_busy)	{
		i4b_Dfreembuf(m);	/* yes, discard information */
/* 
 * P bit == 1 ? 
 */
		if (p == 1)	{
			i4b_tx_rnr_response(l2, p); /* yes, tx RNR */
			l2->ack_pend = 0;	/* clear ACK pending */
		}
	} else {
/* 
 * own receiver ready, where if /* expected sequence number ?  
 */	
		if (ns == l2->vr)	{
			M128INC(l2->vr);	/* yes, update */

			l2->rej_excpt = 0;	/* clr reject exception */

			m_adj(m, I_HDR_LEN);	/* strip i frame header */

			l2->iframe_sent = 0;	/* reset i acked already */

			i4b_dl_data_ind(l3, m);	/* pass data up */

			if (!l2->iframe_sent) {
				i4b_tx_rr_response(l2, p); /* yes, tx RR */
				l2->ack_pend = 0;	/* clr ACK pending */
			}
		} else {
/* 
 * ERROR, sequence number NOT expected 
 */		
			i4b_Dfreembuf(m);	/* discard information */
/* 
 * already exception ? 
 */
			if (l2->rej_excpt == 1) {
/* 
 * immediate response ? 
 */
				if (p == 1)	{
					i4b_tx_rr_response(l2, p); /* yes, tx RR */
					l2->ack_pend = 0; /* clr ack pend */
				}
			} else {
/* 
 * not in exception cond 
 */			
				l2->rej_excpt = 1;	/* set exception */
				i4b_tx_rej_response(l2, p);	/* tx REJ */
				l2->ack_pend = 0;	/* clr ack pending */
			}
		}
	}
/* 
 * sequence number ranges as expected ? 
 */
	if (i4b_l2_nr_ok(nr, l2->va, l2->vs)) {
		if (l2->Q921_state == ST_TIMREC) {
			l2->va = nr;

			mtx_unlock(&i4b_mtx);

			return;
		}
/* 
 * yes, other side busy ? 
 */
		if (l2->peer_busy) {
			l2->va = nr;	/* yes, update ack count */
		} else {
/* 
 * other side ready 
 */		
			if (nr == l2->vs) {
/* 
 * count expected ? 
 */			
						
				l2->va = nr;	/* update ack */
				i4b_T200_stop(l2);
				i4b_T203_restart(l2);
			} else {
				if (nr != l2->va) {
					l2->va = nr;
					i4b_T200_restart(l2);
				}
			}
		}
	} else {
		i4b_nr_error_recovery(l2);	/* sequence error */
		l2->Q921_state = ST_AW_EST;
	}

	mtx_unlock(&i4b_mtx);
}

/*---------------------------------------------------------------------------*
 *	internal I FRAME QUEUED UP routine (Q.921 03/93 p 61)
 *---------------------------------------------------------------------------*/
void
i4b_i_frame_queued_up(struct isdn_l2 *l2)
{
	struct mbuf *m;
	u_char *ptr;

	mtx_lock(&i4b_mtx);

	if ((l2->peer_busy) || 
		(l2->vs == ((l2->va + MAX_K_VALUE) & 127))) {
		if (l2->peer_busy) {
			NDBGL2(L2_I_MSG, "regen IFQUP, cause: peer busy!");
		}

		if (l2->vs == ((l2->va + MAX_K_VALUE) & 127)) {
			NDBGL2(L2_I_MSG, "regen IFQUP, cause: vs=va+k!");
		}

		/*
		 * XXX see: Q.921, page 36, 5.6.1 ".. may retransmit an I
		 * frame ...", shall we retransmit the last i frame ?
		 */

		if (!(IF_QEMPTY(&l2->i_queue))) {
			NDBGL2(L2_I_MSG, "re-scheduling IFQU call!");
			START_TIMER(l2->IFQU_callout, 
			i4b_i_frame_queued_up, l2, IFQU_DLY);
		}
		mtx_unlock(&i4b_mtx);
		return;
	}

	IF_DEQUEUE(&l2->i_queue, m);    /* fetch next frame to tx */

	if (!m) {
		NDBGL2(L2_I_ERR, "ERROR, mbuf NULL after IF_DEQUEUE");
		mtx_unlock(&i4b_mtx);
		return;
	}

	ptr = m->m_data;

	PUTSAPI(SAPI_CCP, CR_CMD_TO_NT, *(ptr + OFF_SAPI));
	PUTTEI(l2->tei, *(ptr + OFF_TEI));

	*(ptr + OFF_INS) = (l2->vs << 1) & 0xfe; /* bit 0 = 0 */
	*(ptr + OFF_INR) = (l2->vr << 1) & 0xfe; /* P bit = 0 */

	l2->stat.tx_i++;	/* update frame counter */
/* 
 * free'd when ack'd ! 
 */
	l2->driver->ph_data_req(l2->l1_token, m, MBUF_DONTFREE); 
/*
 * in case we ack an I frame with another I frame 
 */		
	l2->iframe_sent = 1;		

	if (l2->ua_num != UA_EMPTY) {
/* 
 * failsafe 
 */	
		NDBGL2(L2_I_ERR, "ERROR, l2->ua_num: %d != UA_EMPTY", l2->ua_num);
		i4b_print_l2var(l2);
		i4b_Dfreembuf(l2->ua_frame);
	}

	l2->ua_frame = m;		/* save unacked frame */
	l2->ua_num = l2->vs;	/* save frame number */

	M128INC(l2->vs);

	l2->ack_pend = 0;

	mtx_unlock(&i4b_mtx);

	if (l2->T200 == TIMER_IDLE) {
		i4b_T203_stop(l2);
		i4b_T200_start(l2);
	}
}

#endif /* NI4BQ921 */
