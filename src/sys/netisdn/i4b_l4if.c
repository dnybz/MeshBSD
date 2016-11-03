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
 *	i4b_l4if.c - Layer 3 interface to Layer 4
 *	-------------------------------------------
 *
 *	$Id: i4b_l4if.c,v 1.19 2005/12/11 12:25:06 christos Exp $
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
 
#include "opt_i4bq931.h"

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/i4b_debug.h>
#include <netisdn/i4b_ioctl.h>
#include <netisdn/i4b_cause.h>

#include <netisdn/i4b_isdnq931.h>
#include <netisdn/i4b_l2.h>
#include <netisdn/i4b_l1l2.h>
#include <netisdn/i4b_l3l4.h>
#include <netisdn/i4b_mbuf.h>
#include <netisdn/i4b_global.h>

#include <netisdn/i4b_l3.h>
#include <netisdn/i4b_l3fsm.h>
#include <netisdn/i4b_q931.h>

#include <netisdn/i4b_l4.h>

void 	n_connect_request(struct isdn_call_desc *);
void 	n_connect_response(struct isdn_call_desc *, int, int);
void 	n_disconnect_request(struct isdn_call_desc *, int);
void 	n_alert_request(struct isdn_call_desc *);
void 	n_mgmt_command(struct isdn_l3 *, int, void *);

/*---------------------------------------------------------------------------*
 *	i4b_mdl_status_ind - status indication from lower layers
 *---------------------------------------------------------------------------*/
int
i4b_mdl_status_ind(struct isdn_l3 *l3, int status, int parm)
{
	int sendup, i;

	NDBGL3(L3_MSG, "isdnif = %d, status = %d, parm = %d",
	    l3->l3_id, status, parm);

	switch (status) {
	case STI_ATTACH:
		
		if (parm) {
			NDBGL3(L3_MSG, "STI_ATTACH: attaching "
				"isdnif %d", l3->l3_id);
		} else {
			NDBGL3(L3_MSG, "STI_ATTACH: dettaching "
				"isdnif %d", l3->l3_id);
		}
		break;
	case STI_L1STAT:
		
		i4b_l4_l12stat(d, 1, parm);

		NDBGL3(L3_MSG, "STI_L1STAT: isdnif %d "
			"layer 1 = %s", l3->l3_id, status ? "up" : "down");
		break;
	case STI_L2STAT:
		
		i4b_l4_l12stat(d, 2, parm);
		
		NDBGL3(L3_MSG, "STI_L2STAT: isdnif %d "
			"layer 2 = %s", l3->l3_id, status ? "up" : "down");
		break;
	case STI_TEIASG:
		d->tei = parm;
		i4b_l4_teiasg(d, parm);

		NDBGL3(L3_MSG, "STI_TEIASG: isdnif %d "
			"TEI = %d = 0x%02x", l3->l3_id, parm, parm);
		break;
	case STI_PDEACT:	
/* 
 * L1 T4 timeout 
 */
		NDBGL3(L3_ERR, "STI_PDEACT: isdnif %d "
			"TEI = %d = 0x%02x", l3->l3_id, parm, parm);

		sendup = 0;

		for (i = 0; i < num_call_desc; i++) {
			if (call_desc[i].cd_l3_id == l3->l3_id) {
				
				i4b_l3_stop_all_timers(&(call_desc[i]));
				
				if (call_desc[i].cd_id != CDID_UNUSED) {
					sendup++;
					call_desc[i].cd_id = CDID_UNUSED;
				}
			}
		}

		l3->l3_dl_est = DL_DOWN;

		for (i = 0; i < l3->l3_nbch; i++)
			l3->l3_bch_state[i] = BCH_ST_FREE;
		d->tei = -1;

		if (sendup) 
			i4b_l4_pdeact(d, sendup);
			
		break;
	case STI_NOL1ACC:	
/* 
 * no outgoing access to S0 
 */
		NDBGL3(L3_ERR, "STI_NOL1ACC: isdnif %d "
			"no outgoing access to S0", l3->l3_id);

		for (i = 0; i < num_call_desc; i++) {
			if (call_desc[i].cd_l3_id == l3->l3_id) {
				if (call_desc[i].cd_id != CDID_UNUSED) {
					SET_CAUSE_TYPE(
						call_desc[i].cd_cause_in, 
						CAUSET_I4B
					);
					SET_CAUSE_VAL(
						call_desc[i].cd_cause_in, 
						CAUSE_I4B_L1ERROR
					);
					i4b_l4_disconnect_ind(&(call_desc[i]));
				}
			}
		}
		l3->l3_dl_est = DL_DOWN;

		for (i = 0; i < l3->l3_nbch; i++)
			l3->l3_bch_state[i] = BCH_ST_FREE;
		d->tei = -1;

		break;
	default:
		NDBGL3(L3_ERR, "ERROR, isdnif %d, unknown "
			"status value %d!", l3->l3_id, status);
		break;
	}

	return(0);
}

/*---------------------------------------------------------------------------*
 *	send command to the lower layers
 *---------------------------------------------------------------------------*/
void
n_mgmt_command(struct isdn_l3 *d, int cmd, void *parm)
{
	int i;

	switch (cmd) {
	case CMR_DOPEN:
		
		NDBGL3(L3_MSG, "CMR_DOPEN for isdnif %d", l3->l3_id);

		for (i = 0; i < num_call_desc; i++) {
			if (call_desc[i].cd_l3_id == l3->l3_id) 
				call_desc[i].cd_id = CDID_UNUSED;		
		}
		l3->l3_dl_est = DL_DOWN;
		
		for (i = 0; i < l3->l3_nbch; i++)
			l3->l3_bch_state[i] = BCH_ST_FREE;
		d->tei = -1;

		break;
	case CMR_DCLOSE:
		NDBGL3(L3_MSG, "CMR_DCLOSE for isdnif %d", l3->l3_id);
		break;
	default:
		NDBGL3(L3_MSG, "unknown cmd %d for isdnif %d",
			cmd, l3->l3_id);
		break;
	}
	i4b_mdl_command_req(l3, cmd, parm);
}

/*---------------------------------------------------------------------------*
 *	handle connect request message from userland
 *---------------------------------------------------------------------------*/
void
n_connect_request(struct isdn_call_desc *cd)
{
	next_l3state(cd, EV_SETUPRQ);
}

/*---------------------------------------------------------------------------*
 *	handle setup response message from userland
 *---------------------------------------------------------------------------*/
void
n_connect_response(struct isdn_call_desc *cd, int response, int cause)
{
	struct isdn_l3 *l3 = cd->cd_l3;
	int chstate;

	T400_stop(cd);

	cd->cd_response = response;
	cd->cd_cause_out = cause;

	switch (response) {
	case SETUP_RESP_ACCEPT:
		next_l3state(cd, EV_SETACRS);
		chstate = BCH_ST_USED;
		break;
	case SETUP_RESP_REJECT:
		next_l3state(cd, EV_SETRJRS);
		chstate = BCH_ST_FREE;
		break;
	case SETUP_RESP_DNTCRE:
		next_l3state(cd, EV_SETDCRS);
		chstate = BCH_ST_FREE;
		break;
	default:	
/* 
 * failsafe 
 */
		next_l3state(cd, EV_SETDCRS);
		chstate = BCH_ST_FREE;
		NDBGL3(L3_ERR, "unknown response, doing SETUP_RESP_DNTCRE");
		break;
	}

	if ((cd->cd_ch_id >= 0) && 
		(cd->cd_ch_id < l3->l3_nbch)) {
		
		l3->l3_bch_state[cd->cd_ch_id] = chstate;

		i4b_l2_channel_set_state(cd->cd_l3, cd->cd_ch_id, chstate);
	} else {
		NDBGL3(L3_MSG, "Warning, invalid channelid %d, "
			"response = %d\n", cd->cd_ch_id, response);
	}
}

/*---------------------------------------------------------------------------*
 *	handle disconnect request message from userland
 *---------------------------------------------------------------------------*/
void
n_disconnect_request(struct isdn_call_desc *cd, int cause)
{
	cd->cause_out = cause;

	next_l3state(cd, EV_DISCRQ);
}

/*---------------------------------------------------------------------------*
 *	handle alert request message from userland
 *---------------------------------------------------------------------------*/
void
n_alert_request(struct isdn_call_desc *cd)
{
	next_l3state(cd, EV_ALERTRQ);
}
