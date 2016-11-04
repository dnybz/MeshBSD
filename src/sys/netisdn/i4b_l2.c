/* $NetBSD: i4b_l2.c,v 1.24 2008/07/20 01:05:27 martin Exp $ */

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
 *	$Id: i4b_l2.c,v 1.24 2008/07/20 01:05:27 martin Exp $
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

#include "opt_i4bq921.h"

#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/i4b.h>

#include <netisdn/i4b_debug.h>
#include <netisdn/i4b_ioctl.h>

#include <netisdn/i4b_l3l4.h>
#include <netisdn/i4b_l2.h>
#include <netisdn/i4b_l1l2.h>
#include <netisdn/i4b_isdnq931.h>
#include <netisdn/i4b_mbuf.h>
#include <netisdn/i4b_global.h>

#include <netisdn/i4b_l2fsm.h>

/* this layers debug level */

unsigned int i4b_l2_debug = L2_DEBUG_DEFAULT;

/*---------------------------------------------------------------------------*
 *	DL_ESTABLISH_REQ from layer 3
 *---------------------------------------------------------------------------*/
int 
i4b_dl_establish_req(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_PRIM, "isdnif %d", l2->l3->isdnif);
	i4b_l1_activate(l2);
	i4b_next_l2state(l2, l3, EV_DLESTRQ);
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL_RELEASE_REQ from layer 3
 *---------------------------------------------------------------------------*/
int 
i4b_dl_release_req(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_PRIM, "isdnif %d", l2->l3->isdnif);
	i4b_next_l2state(l2, l3, EV_DLRELRQ);
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL UNIT DATA REQUEST from Layer 3
 *---------------------------------------------------------------------------*/
int 
i4b_dl_unit_data_req(struct isdn_l2 *l2, struct isdn_l3 *l3, 
	struct mbuf *m)
{
#ifdef NOTDEF
	NDBGL2(L2_PRIM, "isdnif %d", l2->isdnif);
#endif
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL DATA REQUEST from Layer 3
 *---------------------------------------------------------------------------*/
int 
i4b_dl_data_req(struct isdn_l2 *l2, struct isdn_l3 *l3, struct mbuf *m)
{
	int error;
	
	switch(l2->Q921_state) {
	case ST_AW_EST:
	case ST_MULTIFR:
	case ST_TIMREC:

		IFQ_ENQUEUE(&l2->i_queue, m, error);
		
		if (error) 
			NDBGL2(L2_ERROR, "i_queue full!!");
		else
			i4b_i_frame_queued_up(l2);
		
		break;
	default:
		NDBGL2(L2_ERROR, "isdnif %d ERROR in state [%s], "
			"freeing mbuf", l2->l3->isdnif, i4b_print_l2state(l2));
		m_freem(m);
		error = EINVAL;
		break;
	}
	return (error);
}

/*---------------------------------------------------------------------------*
 *	isdn_layer2_activate_ind - link activation/deactivation indication from  
 *  layer 1
 *---------------------------------------------------------------------------*/
int
isdn_layer2_activate_ind(struct isdn_l2 *l2, 
	struct isdn_l3 *l3, int event_activate)
{
	if (event_activate) 
		l2->ph_active = PH_ACTIVE;
	else 
		l2->ph_active = PH_INACTIVE;

	return (0);
}

/*---------------------------------------------------------------------------*
 *	i4b_l2_unit_init - place layer 2 unit into known state
 *---------------------------------------------------------------------------*/
static void
i4b_l2_unit_init(struct isdn_l2 *l2)
{
	mtx_lock(&i4b_mtx);
	l2->Q921_state = ST_TEI_UNAS;
	l2->tei_valid = TEI_INVALID;
	l2->vr = 0;
	l2->vs = 0;
	l2->va = 0;
	l2->ack_pend = 0;
	l2->rej_excpt = 0;
	l2->peer_busy = 0;
	l2->own_busy = 0;
	l2->l3initiated = 0;

	l2->rxd_CR = 0;
	l2->rxd_PF = 0;
	l2->rxd_NR = 0;
	l2->RC = 0;
	l2->iframe_sent = 0;

	l2->postfsmfunc = NULL;

	if (l2->ua_num != UA_EMPTY) {
		m_freem(l2->ua_frame);
		l2->ua_num = UA_EMPTY;
		l2->ua_frame = NULL;
	}
	
	i4b_T200_stop(l2);
	i4b_T202_stop(l2);
	i4b_T203_stop(l2);

	mtx_unlock(&i4b_mtx);
}

/*---------------------------------------------------------------------------*
 *	isdn_layer2_status_ind - status indication upward
 *---------------------------------------------------------------------------*/
int
isdn_layer2_status_ind(struct isdn_l2 *l2, struct isdn_l3 *l3, 
	int status, int parm)
{
	int sendup = 1;

	mtx_lock(&i4b_mtx)

	NDBGL2(L2_PRIM, "isdnif %d, status=%d, parm=%d", 
		l2->l3->isdnif, status, parm);

	switch(status) {
	case STI_ATTACH:
		if (parm == 0) {
/* 
 * detach 
 */
			callout_stop(&l2->T200_callout);
			callout_stop(&l2->T202_callout);
			callout_stop(&l2->T203_callout);
			callout_stop(&l2->IFQU_callout);
			break;
		}
		l2->i_queue.ifq_maxlen = IQUEUE_MAXLEN;
		l2->ua_frame = NULL;

		(void)memset(&l2->stat, 0, sizeof(lapdstat_t));
/* 
 * initialize the callout handles for timeout routines 
 */
		callout_init(&l2->T200_callout, 0);
		callout_init(&l2->T202_callout, 0);
		callout_init(&l2->T203_callout, 0);
		callout_init(&l2->IFQU_callout, 0);

		i4b_l2_unit_init(l2);
		break;
	case STI_L1STAT:	/* state of layer 1 */
		break;
	case STI_PDEACT:	/* Timer 4 expired */
/*
 * XXX
 */			
 		if ((l2->Q921_state >= ST_AW_EST) &&
			   (l2->Q921_state <= ST_TIMREC)) {
			NDBGL2(L2_ERROR, "isdnif %d, persistent deactivation!", 
				l2->l3->isdnif);
			i4b_l2_unit_init(l2);
			parm = -1;	/* this is passed as the new
						 * TEI to upper layers */
		} else 
			sendup = 0;
		break;
	case STI_NOL1ACC:
		i4b_l2_unit_init(l2);
		NDBGL2(L2_ERROR, "isdnif %d, cannot access S0 bus!", 
			l2->l3->isdnif);
		break;
	default:
		NDBGL2(L2_ERROR, "ERROR, isdnif %d, unknown status message!", 
			l2->l3->isdnif);
		break;
	}

	if (sendup)
		i4b_mdl_status_ind(l2->l3, status, parm);  /* send up to layer 3 */

	mtx_unlock(&i4b_mtx);

	return (0);
}

/*---------------------------------------------------------------------------*
 *	MDL_COMMAND_REQ from layer 3
 *---------------------------------------------------------------------------*/
int 
i4b_mdl_command_req(struct isdn_l3 *l3, int command, void *parm)
{
	struct isdn_l2 *sc = (struct isdn_l2*)l3->l1_token;

	NDBGL2(L2_PRIM, "isdnif %d, command=%d, parm=%p",
		 l3->isdnif, command, parm);

	switch(command) {
	case CMR_DOPEN:
		i4b_l2_unit_init(sc);
			/* XXX - enable interrupts */
		break;
	case CMR_DCLOSE:
			/* XXX - disable interrupts */
		break;
	}

	/* pass down to layer 1 driver */
	if (sc->driver)
		sc->driver->mph_command_req(sc->l1_token, command, parm);

	return (0);
}

int 
i4b_l2_channel_get_state(struct isdn_l3 *l3, int b_chanid)
{
	struct isdn_l2 *sc = l3->l1_token;
	return (sc->bchan_state[b_chanid]);
}

void i4b_l2_channel_set_state(struct isdn_l3 *l3, 
	int b_chanid, int state)
{
	struct isdn_l2 *sc = l3->l1_token;
	sc->bchan_state[b_chanid] = state;
}

/*---------------------------------------------------------------------------*
 *	telephony silence detection
 *---------------------------------------------------------------------------*/

#define TEL_IDLE_MIN (BCH_MAX_DATALEN/2)

int
isdn_bchan_silence(unsigned char *data, int len)
{
	register int i = 0;
	register int j = 0;
	int error = 1;
/* 
 * count idle bytes 
 */

	for (;i < len; i++) {
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
