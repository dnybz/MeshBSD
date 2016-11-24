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
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/isdn.h>
#include <netisdn/isdn_var.h>
#include <netisdn/isdn_dl.h>

/*
 * XXX: ...
 */

static void 	isdn_dl_init(struct isdn_softc *);

static void 	isdn_bc_callout_init(struct isdn_bc *);
static void 	isdn_bc_callout_stop(struct isdn_bc *);

/*
 * XXX: ... 
 */

struct isdn_bcq 	isdn_dl_bcq;

/*
 * XXX: I'll transform this in to MIB.
 *
int isdn_dl_debug = L2_DEBUG_DEFAULT;
 */

/*---------------------------------------------------------------------------*
 *	isdn_print_frame - just print the hex contents of a frame
 *---------------------------------------------------------------------------*/

void
isdn_dl_print_frame(int len, uint8_t *buf)
{
#ifdef ISDN_DEBUG
	int i;
/* 
 * XXX: ...
 */
	if (isdn_dl_debug & L2_ERROR) {
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
isdn_dl_nr_ok(int nr, int va, int vs)
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
 *	routine ESTABLISH DATA LINK (Q.921 03/93 page 83)
 *---------------------------------------------------------------------------*/
void
isdn_dl_establish(struct isdn_ifaddr *ii)
{
	isdn_dl_clear_exception_cond(ii);

	ii->ii_RC = 0;

	(void)isdn_dl_tx_u_frame(ii, CR_CMD_TO_NT, P1, SABME);
	
	isdn_T200_restart(ii);

	isdn_T203_stop(ii);
}

/*---------------------------------------------------------------------------*
 *	routine CLEAR EXCEPTION CONDITIONS (Q.921 03/93 page 83)
 *---------------------------------------------------------------------------*/
void
isdn_dl_clear_exception_cond(struct isdn_ifaddr *ii) 
{

/*XXX -------------------------------------------------------------- */
/*XXX is this really appropriate here or should it moved elsewhere ? */

	IF_DRAIN(&ii->ii_i_queue);

	if (ii->ii_ua_num != UA_EMPTY) {
		m_freem(ii->ii_ua_frame);
		ii->ii_ua_num = UA_EMPTY;
	}
	ii->ii_peer_busy = 0;
	ii->ii_rej_excpt = 0;
	ii->ii_own_busy = 0;
	ii->ii_ack_pend = 0;
}

/*---------------------------------------------------------------------------*
 *	routine TRANSMIT ENQUIRE (Q.921 03/93 page 83)
 *---------------------------------------------------------------------------*/
void
isdn_dl_tx_enquire(struct isdn_ifaddr *ii)
{
	if (ii->ii_own_busy)
		(void)isdn_dl_tx_s_frame(ii, CR_RSP_TO_NT, P1, RNR);	
	else
		(void)isdn_dl_tx_s_frame(ii, CR_RSP_TO_NT, P1, RR);	

	ii->ii_ack_pend = 0;

	isdn_T200_start(ii);
}

/*---------------------------------------------------------------------------*
 *	routine NR ERROR RECOVERY (Q.921 03/93 page 83)
 *---------------------------------------------------------------------------*/
void
isdn_dl_nr_error_recovery(struct isdn_ifaddr *ii)
{

	isdn_lme_error_ind(ii, "isdn_dl_nr_error_recovery", MDL_ERR_J);

	isdn_dl_establish(ii);

	ii->ii_l3_init = 0;
}

/*---------------------------------------------------------------------------*
 *	routine ENQUIRY RESPONSE (Q.921 03/93 page 84)
 *---------------------------------------------------------------------------*/
void
isdn_dl_enquiry_resp(struct isdn_ifaddr *ii)
{
	if (ii->ii_own_busy)
		(void)isdn_dl_tx_s_frame(ii, CR_RSP_TO_NT, F1, RNR);	
	else
		(void)isdn_dl_tx_s_frame(ii, CR_RSP_TO_NT, F1, RR);

	ii->ii_ack_pend = 0;
}

/*---------------------------------------------------------------------------*
 *	routine INVOKE RETRANSMISSION (Q.921 03/93 page 84)
 *---------------------------------------------------------------------------*/
void
isdn_dl_invoke_rtx(struct isdn_ifaddr *ii, int nr)
{
	NDBGL2(L2_ERROR, "nr = %d", nr);

	while (ii->ii_vs != nr) {
		NDBGL2(L2_ERROR, "nr(%d) != vs(%d)", nr, ii->ii_vs);

		M128DEC(ii->ii_vs);

/* XXXXXXXXXXXXXXXXX */

		if ((ii->ii_ua_num != UA_EMPTY) && 
			(ii->ii_vs == ii->ii_ua_num)) {
			
			if (_IF_QFULL(&ii->ii_i_queue)) 
				NDBGL2(L2_ERROR, "ERROR, I-queue full!");
			else {
				IF_ENQUEUE(&ii->ii_i_queue, 
					ii->ii_ua_frame);
				ii->ii_ua_num = UA_EMPTY;
			}
		} else {
			NDBGL2(L2_ERROR, "ERROR, l2->vs = %d, "
				"l2->ua_num = %d ", 
				ii->ii_vs, 
				ii->ii_ua_num);
		}

/* XXXXXXXXXXXXXXXXX */

		isdn_queue_i_frame(ii);
	}
}

/*---------------------------------------------------------------------------*
 *	DL_ESTABLISH_REQ from layer 3
 *---------------------------------------------------------------------------*/
int 
isdn_dl_establish_req(struct isdn_ifaddr *ii)
{
	
	NDBGL2(L2_PRIM, "isdnif %d", ii->ii_ifp->if_index);
	
	isdn_dl_next_state(ii, EV_DLESTRQ);
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL_RELEASE_REQ from layer 3
 *---------------------------------------------------------------------------*/
int 
isdn_dl_release_req(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_PRIM, "isdnif %d", ii->ii_ifp->if_index);
	isdn_dl_next_state(ii, EV_DLRELRQ);
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL UNIT DATA REQUEST from Layer 3
 *---------------------------------------------------------------------------*/
int 
isdn_dl_unit_data_req(struct isdn_ifaddr *ii, struct mbuf *m)
{
#ifdef NOTDEF
	NDBGL2(L2_PRIM, "isdnif %d", ii->ii_ifp->if_index);
#endif
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL DATA REQUEST from Layer 3
 *---------------------------------------------------------------------------*/
int 
isdn_dl_data_req(struct isdn_ifaddr *ii, struct mbuf *m)
{
	int error = 0;

	switch(ii->ii_Q921_state) {
	case ST_AW_EST:
	case ST_MULTIFR:
	case ST_TIMREC:
/*
 * Allocate I Frame header.
 */
		M_PREPEND(m, I_FRAME_HDRLEN, (M_ZERO|M_NOWAIT));
		if (m == NULL) {
			error = ENOBUFS;
			break;
		}		
/*
 * Enqueue, if not possible, mbuf will be discarded.
 */
		IFQ_ENQUEUE(&ii->ii_i_queue, m, error);
		
		if (error != 0) 
			NDBGL2(L2_ERROR, "i_queue full!!");
		else
			isdn_queue_i_frame(ii);
		
		break;
	default:
		NDBGL2(L2_ERROR, "isdnif %d ERROR in state [%s], "
			"freeing mbuf", ii->ii_ifp->if_index, 
			isdn_dl_print_state(ii));
		m_freem(m);
		error = EINVAL;
		break;
	}
	return (error);
}

/*---------------------------------------------------------------------------*
 *	isdn_dl_init - place layer 2 unit into known state
 *---------------------------------------------------------------------------*/
static void
isdn_dl_init(struct isdn_ifaddr *ii)
{
	ii->ii_Q921_state = ST_TEI_UNAS;
	ii->ii_tei_valid = TEI_INVALID;
	ii->ii_vr = 0;
	ii->ii_vs = 0;
	ii->ii_va = 0;
	ii->ii_ack_pend = 0;
	ii->ii_rej_excpt = 0;
	ii->ii_peer_busy = 0;
	ii->ii_own_busy = 0;
	ii->ii_l2_l3_init = 0;

	ii->ii_rxd_CR = 0;
	ii->ii_rxd_PF = 0;
	ii->ii_rxd_NR = 0;
	ii->ii_RC = 0;
	ii->ii_iframe_sent = 0;

	ii->ii_post_fsm_fn = NULL;

	if (ii->ii_ua_num != UA_EMPTY) {
		m_freem(ii->ii_ua_frame);
		ii->ii_ua_num = UA_EMPTY;
		ii->ii_ua_frame = NULL;
	}
	
	isdn_T200_stop(ii);
	isdn_T202_stop(ii);
	isdn_T203_stop(ii);
}

/*---------------------------------------------------------------------------*
 *	isdn_dl_status_ind - status indication upward
 *---------------------------------------------------------------------------*/
int
isdn_dl_status_ind(struct isdn_ifaddr *ii, int status, int parm)
{
	int send_up, init_l2;

	NDBGL2(L2_PRIM, "isdnif %d, status=%d, parm=%d", 
		ii->ii_ifp->if_index, status, parm);

	send_up = 1;
	init_l2 = 0;

	switch (status) {
	case STI_ATTACH:
		if (parm == 0) {
/* 
 * detach 
 */
			callout_stop(&ii->ii_T200_callout);
			callout_stop(&ii->ii_T202_callout);
			callout_stop(&ii->ii_T203_callout);
			callout_stop(&ii->ii_IFQU_callout);
			break;
		}
		ii->ii_i_queue.ifq_maxlen = IQUEUE_MAXLEN;
		ii->ii_ua_frame = NULL;

		(void)memset(&ii->ii_stat, 0, sizeof(lapdstat_t));
/* 
 * initialize the callout handles for timeout routines 
 */
		callout_init(&ii->ii_T200_callout, 0);
		callout_init(&ii->ii_T202_callout, 0);
		callout_init(&ii->ii_T203_callout, 0);
		callout_init(&ii->ii_IFQU_callout, 0);

		init_l2 = 1;
		break;
	case STI_L1STAT:	/* state of layer 1 */
		break;
	case STI_PDEACT:	/* Timer 4 expired */
/*
 * XXX
 */			
 		if ((ii->ii_Q921_state >= ST_AW_EST) &&
			   (ii->ii_Q921_state <= ST_TIMREC)) {
			NDBGL2(L2_ERROR, "isdnif %d, persistent deactivation!", 
				ii->ii_ifp->if_index);
			init_l2 = 1;
			parm = -1;	/* this is passed as the new
						 * TEI to upper layers */
		} else 
			send_up = 0;
		break;
	case STI_NOL1ACC:
		init_l2 = 1;
		NDBGL2(L2_ERROR, "isdnif %d, cannot access S0 bus!", 
			ii->ii_ifp->if_index);
		break;
	default:
		NDBGL2(L2_ERROR, "ERROR, isdnif %d, unknown status message!", 
			ii->ii_ifp->if_index);
		break;
	}

	if (init_l2) 
		isdn_dl_init(ii);

	if (send_up)
		isdn_mdl_status_ind(ii, status, parm);  /* send up to layer 3 */

	return (0);
}

/*---------------------------------------------------------------------------*
 *	MDL_COMMAND_REQ from layer 3
 *---------------------------------------------------------------------------*/
int 
isdn_dl_cmd_req(struct isdn_ifaddr *ii, int cmd, void *arg)
{
	NDBGL2(L2_PRIM, "isdnif %d, cmd=%d, arg=%p",
		 ii->ii_ifp->if_index, cmd, arg);

	switch(cmd) {
	case CMR_DOPEN:
		isdn_dl_init(ii);

		break;
	case CMR_DCLOSE:
		break;
	default;
		break;
	}
/* 
 * XXX pass down Trace cmd to isdn_input 
 *
	if (ii->driver)
		sc->driver->mph_cmd_req(ii->l1_token, cmd, parm);
 */
	return (0);
}

/*---------------------------------------------------------------------------*
 *	telephony silence detection
 *---------------------------------------------------------------------------*/

#define TEL_IDLE_MIN (BCH_MAX_DATALEN/2)

int
isdn_bc_silence(uint8_t *data, int len)
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

#ifdef ISDN_DEBUG
	(void)printf("%s: got %d silence bytes in frame\n", __func__, j);
#endif /* ISDN_DEBUG */

	if (j < (TEL_IDLE_MIN))
		error = 0;
	
	return (error);
}


/*
 * XXX: ... 
 */

struct isdn_bc * 	
isdn_bc_alloc(struct isdn_ifaddr *ii)
{
	struct isdn_bc *bc;

	if ((bc = malloc(sizeof(*bc), M_IFADDR, M_NOWAIT|M_ZERO)) != NULL) { 
		
		TAILQ_INSERT_HEAD(&ii->ii_bcq, bc, bc_link);
		TAILQ_INSERT_HEAD(&isdn_dl_bcq, bc, bc_chain);
		isdn_bc_callout_init(bc);
	}
	return (bc);
}

void 	
isdn_bc_free(struct isdn_bc *bc) 
{
	struct isdn_ifaddr *ii = bc->bc_sc;
	
	isdn_bc_callout_stop(bc);
	TAILQ_REMOVE(&ii->ii_bcq, bc, bc_link);
	TAILQ_REMOVE(&isdn_dl_bcq, bc, bc_chain);
	free(bc, M_IFADDR);
}


/*---------------------------------------------------------------------------*
 *      search calldescriptor
 *      ---------------------
 *      This routine searches for the calldescriptor for a passive controller
 *      given by unit number, callreference and callreference flag.
 *	It returns a pointer to the calldescriptor if found, else a NULL.
 *---------------------------------------------------------------------------*/
struct isdn_bc * 	
isdn_bc_by_cr(struct isdn_ifaddr *ii, int cr, int crf)
{
	struct isdn_bc *bc;

	TAILQ_FOREACH(bc, &ii->ii_bcq, bc_link) {
		
		if ((bc->bc_cr == cr) && 
			(bc->bc_cr_flag == crf)) {
			break;
		}
	}

	if (bc != NULL) {
		NDBGL4(L4_MSG, "found b-cahnnel @ isdnif=%d id=%u cr=%d",
			ii->ii_ifp->if_index, bc->bc_id, bc->bc_cr);
		isdn_bc_callout_init(bc);
	}
	return (bc);
}

/*
 *	generate 7 bit "random" number used for outgoing Call Reference
 *
 * XXX: This fn is completely wrong, because Q.931 dictates cr >= 2byte.
 */
uint8_t
isdn_bc_get_cr(void)
{
	register int i;
	uint8_t val, cr;

	val = 42;

	for (i = 0; i < 50; i++) {
	
		arc4rand(&val, sizeof(val), 0);

		cr = val & 0x7f;

		if (cr == 0 || cr == 0x7f)
			continue;

		TAILQ_FOREACH(bc, &isdn_dl_bcq, bc_chain) {
		
			if (bc->bc_cr == cr) {
				cr = 0;
				break;
			}
		}

		if (cr)
			break;
	}
	
	return (cr);
}

/*
 * XXX: ... 
 */

static void
isdn_bc_callout_stop(struct isdn_bc *bc)
{
	if (bc->bc_callouts_inited) {
		callout_stop(&bc->bc_idle_timeout_handle);
		callout_stop(&bc->bc_T303_callout);
		callout_stop(&bc->bc_T305_callout);
		callout_stop(&bc->bc_T308_callout);
		callout_stop(&bc->bc_T309_callout);
		callout_stop(&bc->bc_T310_callout);
		callout_stop(&bc->bc_T313_callout);
		callout_stop(&bc->bc_T400_callout);
	}
}


void
isdn_bc_callout_init(struct isdn_bc *bc)
{
	if (bc->bc_callouts_inited == 0) {
		callout_init(&bc->bc_idle_timeout_handle, 0);
		callout_init(&bc->bc_T303_callout, 0);
		callout_init(&bc->bc_T305_callout, 0);
		callout_init(&bc->bc_T308_callout, 0);
		callout_init(&bc->bc_T309_callout, 0);
		callout_init(&bc->bc_T310_callout, 0);
		callout_init(&bc->bc_T313_callout, 0);
		callout_init(&bc->bc_T400_callout, 0);
		bc->bc_callouts_inited = 1;
	}
}



