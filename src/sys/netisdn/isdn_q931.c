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

static int 	isdn_q931_decode_cs0_ie(struct isdn_bc *, int, uint8_t *);
static void 	isdn_q931_decode_msg(struct isdn_bc *, uint8_t);

/*
 * XXX: well, I'll transform it into an OID.
 *
int isdn_l3_debug = L3_DEBUG_DEFAULT;
 *
 */
 
/* 
 * protocol independent causes -> Q.931 causes 
 */

uint8_t isdn_q931_cause_tab[ISDN_CAUSE_MAX] = {
/* 
 * CAUSE_ISDN_NORMAL -> normal call clearing 
 */	
	CAUSE_Q850_NCCLR, 	
/* 
 * CAUSE_ISDN_BUSY -> user busy 
 */	
	CAUSE_Q850_USRBSY,	
/* 
 * CAUSE_ISDN_NOCHAN -> no circuit/channel available
 */	
	CAUSE_Q850_NOCAVAIL,
/* 
 * CAUSE_ISDN_INCOMP -> incompatible destination 
 */	
	CAUSE_Q850_INCDEST,	
/* 
 * CAUSE_ISDN_REJECT -> call rejected 
 */	
	CAUSE_Q850_CALLREJ,	
/* 
 * CAUSE_ISDN_OOO -> destination out of order 
 */	
	CAUSE_Q850_DSTOOORDR,
/* 
 * CAUSE_ISDN_TMPFAIL -> temporary failure 
 */		
	CAUSE_Q850_TMPFAIL,	
/* 
 * CAUSE_ISDN_L1ERROR -> L1 error / persistent deact XXX 
 */	
	CAUSE_Q850_USRBSY,
/* 
 * CAUSE_ISDN_LLDIAL -> no dialout on leased line XXX 
 */	
	CAUSE_Q850_USRBSY,	
};

/*---------------------------------------------------------------------------*
 *	setup cr ref flag according to direction
 *---------------------------------------------------------------------------*/
uint8_t
isdn_q931_setup_cr(struct isdn_bc *bc, uint8_t cr)
{
	if (bc->bc_cr_flag == CRF_ORIG)
		return (cr & 0x7f);	/* clear cr ref flag */
	else if (bc->bc_cr_flag == CRF_DEST)
		return (cr | 0x80);	/* set cr ref flag */
	else
		panic("isdn_q931_setup_cr: invalid cr_flag!");
}

/*---------------------------------------------------------------------------*
 *	decode and process a Q.931 message
 *---------------------------------------------------------------------------*/
void
isdn_q931_decode(struct isdn_ifaddr *ii, struct mbuf *m)
{
	struct isdn_bc *bc = NULL;
	int codeset = CODESET_0;
	int codeset_old = CODESET_0;
	int shift_flag = UNSHIFTED;
	int cr_len = 0;
	int cr = 0;
	int cr_flag = 0;
	int i;
	int offset;
	int msg_len;  
	uint8_t *msg_ptr;  
	
	msg_len = m->m_len;  
	msg_ptr = mtod(m, uint8_t *);  
/* 
 * check protocol discriminator 
 */
	if (*msg_ptr != PD_Q931) {
/*
 * XXX ... I'll move this artefact into isdn_softc{}.
 */
		static int proto_flag = -1;	/* print only once .. */

		if (*msg_ptr != proto_flag) {
			NDBGL3(L3_P_MSG, "unknown protocol discriminator 0x%x!", *msg_ptr);
			proto_flag = *msg_ptr;
		}
		goto out;
	}
	
	msg_ptr++;
	msg_len--;		
/* 
 * extract call reference 
 */
	cr_len = *msg_ptr & CRLENGTH_MASK;
	msg_ptr++;
	msg_len--;

	if (cr_len != 0) {
		cr += *msg_ptr & 0x7f;
		cr_flag = (*msg_ptr >> 7) & 0x01;
		msg_ptr++;
		msg_len--;

		for (i = 1; i < cr_len; i++) {
			cr += *msg_ptr;
			msg_ptr++;
			msg_len--;
		}
	} else {
		cr = 0;
		cr_flag = 0;
	}

	NDBGL3(L3_P_MSG, "Call Ref, len %d, val %d, flag %d", 
		cr_len, cr, cr_flag);
/* 
 * find or allocate calldescriptor 
 */
	if ((bc = isdn_bc_by_cr(sc, cr,
			cr_flag == CRF_DEST ? CRF_ORIG : CRF_DEST)) == NULL) {
		if (*msg_ptr == SETUP) {
/*
 * get and init new calldescriptor 
 */
			if ((bc = isdn_bc_alloc(sc)) == NULL) {
				NDBGL3(L3_P_ERR, "cannot allocate calldescriptor "
					"for cr = 0x%x, cr_flag = 0x%x, "
					"msg = 0x%x, frame = ", 
					cr, cr_flag, *msg_ptr);
				
				isdn_l2_print_frame(msg_len, msg_ptr);
				goto out;
			}
			bc->bc_cr = cr;
			bc->bc_cr_flag = CRF_DEST;	/* we are the dest side */
		} else {
/*
 * XXX
 */			
			if (cr != 0) {
/* 
 * ignore global call references 
 */						
				NDBGL3(L3_P_ERR, "cannot find calldescriptor "
					"for cr = 0x%x, cr_flag = 0x%x, "
					"msg = 0x%x, frame = ", 
					cr, cr_flag, *msg_ptr);
				
				isdn_l2_print_frame(msg_len, msg_ptr);
			}
			goto out;
		}
	}
/* 
 * decode and handle message type 
 */
	isdn_q931_decode_msg(bc, *msg_ptr);
	msg_ptr++;
	msg_len--;
/* 
 * process information elements 
 */
	while (msg_len > 0) {
/* 
 * check for shift codeset IE 
 */
		if ((*msg_ptr & 0x80) && 
			((*msg_ptr & 0xf0) == SOIE_SHIFT)) {
			
			if (!(*msg_ptr & SHIFT_LOCK))
				shift_flag = SHIFTED;

			codeset_old = codeset;
			codeset = *msg_ptr & CODESET_MASK;

			if ((shift_flag != SHIFTED) &&
			   (codeset <= codeset_old)) {
				NDBGL3(L3_P_ERR, "Q.931 lockingshift "
				"proc violation, shift %d -> %d", 
				codeset_old, codeset);
				codeset = codeset_old;
			}
			msg_len--;
			msg_ptr++;
		}
/* 
 * process one IE for selected codeset 
 */
		switch (codeset) {
		case CODESET_0:
			offset = isdn_q931_decode_cs0_ie(bc, msg_len, msg_ptr);
			msg_len -= offset;
			msg_ptr += offset;
			break;
		default:
			NDBGL3(L3_P_ERR, "unknown codeset %d, ", codeset);
			isdn_l2_print_frame(msg_len, msg_ptr);
			msg_len = 0;
			break;
		}
/* 
 * check for non-locking shifts 
 */
		if (shift_flag == SHIFTED) {
			shift_flag = UNSHIFTED;
			codeset = codeset_old;
		}
	}
	isdn_l3_next_state(bc, bc->bc_event);
out:
	m_freem(m);	
}

/*---------------------------------------------------------------------------*
 *	decode and process one Q.931 codeset 0 information element
 *---------------------------------------------------------------------------*/
static int
isdn_q931_decode_cs0_ie(struct isdn_bc *bc, int msg_len, uint8_t *msg_ptr)
{
	int i, j;
	size_t len;
	char *p;

	if (*msg_ptr == IEI_SENDCOMPL) {
/* 
 * single byte IE's 
 */			
		NDBGL3(L3_P_MSG, "IEI_SENDCOMPL");
		return (1);
	}
/* 
 * multi byte IE's
  */
	switch (*msg_ptr) {
	
/*********/
/* Q.931 */
/*********/	
	case IEI_SEGMMSG:	
/* 
 * segmented message 
 */			
		NDBGL3(L3_P_MSG, "IEI_SEGMENTED_MESSAGE");
		break;
	case IEI_BEARERCAP:	
/* 
 * bearer capability 
 */
		switch (msg_ptr[2]) {
		case 0x80:	/* speech */
		case 0x89:	/* restricted digital info */
		case 0x90:	/* 3.1 kHz audio */
/* 
 * XXX 
 */	
			bc->bc_proto = BPROT_NONE;
			NDBGL3(L3_P_MSG, "IEI_BEARERCAP - Telephony");
			break;
		default:

 			bc->bc_proto = BPROT_NONE;
			NDBGL3(L3_P_ERR, "IEI_BEARERCAP - Unsupported "
				"B-Protocol 0x%x", msg_ptr[2]);
			break;
		}
		break;
	case IEI_CAUSE:		
/* 
 * cause 
 */			
		if (msg_ptr[2] & 0x80) {
			bc->bc_cause_in = msg_ptr[3] & 0x7f;
			NDBGL3(L3_P_MSG, "IEI_CAUSE = %d", 
				msg_ptr[3] & 0x7f);
		} else {
			bc->bc_cause_in = msg_ptr[4] & 0x7f;
			NDBGL3(L3_P_MSG, "IEI_CAUSE = %d", 
				msg_ptr[4] & 0x7f);
		}
		break;
	case IEI_CALLID:	
/* 
 * call identity 
 */
		NDBGL3(L3_P_MSG, "IEI_CALL_IDENTITY");
		break;
	case IEI_CALLSTATE:	
/* 
 * call state
 */
		bc->bc_call_state = msg_ptr[2] & 0x3f;
		NDBGL3(L3_P_MSG, "IEI_CALLSTATE = %d", bc->bc_call_state);
		break;
	case IEI_CHANNELID:	
/* 
 * channel id 
 */
		if ((msg_ptr[2] & 0xf4) != 0x80) {
			bc->bc_id = CHAN_NO;
			NDBGL3(L3_P_ERR, "IEI_CHANNELID, "
				"unsupported value 0x%x", msg_ptr[2]);
		} else {
			int old_chanid = bc->bc_id;
			
			switch (msg_ptr[2] & 0x03) {
			case IE_CHAN_ID_NO:
				bc->bc_id = CHAN_NO;
				break;
			case IE_CHAN_ID_B1:
				bc->bc_id = CHAN_B1;
				break;
			case IE_CHAN_ID_B2:
				bc->bc_id = CHAN_B2;
				break;
			case IE_CHAN_ID_ANY:
				bc->bc_id = CHAN_ANY;
				break;
			}
			bc->bc_excl = (msg_ptr[2] & 0x08) >> 3;

			NDBGL3(L3_P_MSG, "IEI_CHANNELID - channel %d, "
			"exclusive = %d", bc->bc_id, bc->bc_excl);
/* 
 * if this is the first time 
 * we know the real channel,
 * reserve it 
 */
			if (old_chanid != bc->bc_id) {
				
				if ((bc->bc_id == CHAN_B1) || 
					(bc->bc_id == CHAN_B2)) {
					
					if (bc->bc_state == BCH_ST_FREE) 
						bc->bc_state = BCH_ST_RSVD;
					else {
						NDBGL3(L3_P_ERR, 
							"IE ChannelID, Channel NOT free!!");
				
					}
				} else if (bc->bc_id == CHAN_NO) {
					NDBGL3(L3_P_MSG, "IE ChannelID, SETUP "
						"with channel = No channel (CW)");
				} else {
/* 
 * bc->bc_id == CHAN_ANY 
 */					
					NDBGL3(L3_P_ERR, "ERROR: IE ChannelID, "
						"SETUP with channel = Any channel!");
				}
			}
		}
		break;
	case IEI_PROGRESSI:	
/* 
 * progress indicator 
 */
		NDBGL3(L3_P_MSG, "IEI_PROGRESSINDICATOR");
		break;
	case IEI_NETSPCFAC:	
/* 
 * network specific fac 
 */
		NDBGL3(L3_P_MSG, "IEI_NETSPCFAC");
		break;
	case IEI_NOTIFIND:	
/* 
 * notification indicator 
 */
		NDBGL3(L3_P_MSG, "IEI_NOTIFICATION_INDICATOR");
		break;
	case IEI_DISPLAY:	
/* 
 * display	
 */
		(void)memcpy(bc->bc_display, &msg_ptr[2], 
			min(DISPLAY_MAX, msg_ptr[1]));
		
		bc->bc_display[min(DISPLAY_MAX, msg_ptr[1])] = '\0';
		
		NDBGL3(L3_P_MSG, "IEI_DISPLAY = %s", bc->bc_display);
  		break;
	case IEI_DATETIME:	
/* 
 * date / time
 */
		i = 2;
		j = msg_ptr[1];
		p = bc->bc_datetime;
		*p = '\0';

		len = 0;
		
		for (j = msg_ptr[1]; j > 0; j--, i++) {
			(void)snprintf(p + len, 
				sizeof(bc->bc_datetime) - len,
				    "%02d", msg_ptr[i]);
			len = strlen(p);
		}
		NDBGL3(L3_P_MSG, "IEI_DATETIME = %s", bc->bc_datetime);
		break;
	case IEI_KEYPAD:	
/* 
 * keypad facility 
 */
		NDBGL3(L3_P_MSG, "IEI_KEYPAD_FACILITY");
		break;
	case IEI_SIGNAL:
/* 
 * signal type 
 */
		NDBGL3(L3_P_MSG, "IEI_SIGNAL = %d", msg_ptr[2]);
		break;
	case IEI_INFRATE:
/* 
 * information rate 
 */
		NDBGL3(L3_P_MSG, "IEI_INFORMATION_RATE");
		break;
	case IEI_ETETDEL:
/* 
 * end to end transit delay 
 */
		NDBGL3(L3_P_MSG, "IEI_END_TO_END_TRANSIT_DELAY");
		break;
	case IEI_CUG:
/* 
 * closed user group 
 */
		NDBGL3(L3_P_MSG, "IEI_CLOSED_USER_GROUP");
		break;
	case IEI_CALLINGPN:	
/* 
 * calling party no 
 */
		bc->bc_type_plan = msg_ptr[2] & 0x7f;
		
		if (msg_ptr[2] & 0x80) {
/* 
 * no presentation/screening indicator ? 
 */		
			(void)memcpy(bc->bc_src.se167_telno, &msg_ptr[3], 
				min(TELNO_MAX, msg_ptr[1]-1));
				
			bc->bc_src.se167_telno[min(TELNO_MAX, msg_ptr[1] - 1)] = '\0';
			bc->bc_scr_ind = SCR_NONE;
			bc->bc_prs_ind = PRS_NONE;
		} else {
			(void)memcpy(bc->bc_src.se167_telno, &msg_ptr[4], 
				min(TELNO_MAX, msg_ptr[1]-2));
			bc->bc_src.se167_telno[min(TELNO_MAX, msg_ptr[1] - 2)] = '\0';
			bc->bc_scr_ind = (msg_ptr[3] & 0x03) + SCR_USR_NOSC;
			bc->bc_prs_ind = ((msg_ptr[3] >> 5) & 0x03) + PRS_ALLOWED;
		}
		NDBGL3(L3_P_MSG, "IEI_CALLINGPN = %s", bc->bc_src.se167_telno);
		break;
	case IEI_CALLINGPS:	
/* 
 * calling party subaddress 
 */
		NDBGL3(L3_P_MSG, "IEI_CALLINGPS");
		
		(void)memcpy(bc->bc_src.se167_subaddr, &msg_ptr[1], 
			min(SUBADDR_MAX, msg_ptr[1]-1));
		break;
	case IEI_CALLEDPN:
/* 
 * called party number 
 */
		(void)memcpy(bc->bc_dst.se167_telno, &msg_ptr[3], 
			min(TELNO_MAX, msg_ptr[1]-1));
		
		bc->bc_dst.se167_telno[min(TELNO_MAX, msg_ptr [1] - 1)] = '\0';
		
		NDBGL3(L3_P_MSG, "IEI_CALLED = %s", bc->bc_dst.se167_telno);
		break;
	case IEI_CALLEDPS:	
/* 
 * called party subaddress 
 */
		NDBGL3(L3_P_MSG, "IEI_CALLEDPS");
		
		(void)memcpy(bc->bc_dest_subaddr, &msg_ptr[1], 
			min(SUBADDR_MAX, msg_ptr[1]-1));
		break;
	case IEI_REDIRNO:	
/* 
 * redirecting number 
 */
		NDBGL3(L3_P_MSG, "IEI_REDIRECTING_NUMBER");
		break;
	case IEI_TRNSEL:
/* 
 * transit network selection 
 */
		NDBGL3(L3_P_MSG, "IEI_TRANSIT_NETWORK_SELECTION");
		break;
	case IEI_RESTARTI:	
/* 
 * restart indicator 
 */
		NDBGL3(L3_P_MSG, "IEI_RESTART_INDICATOR");
		break;
	case IEI_LLCOMPAT:	
/* 
 * low layer compat 
 */
		NDBGL3(L3_P_MSG, "IEI_LLCOMPAT");
		break;
	case IEI_HLCOMPAT:	
/* 
 * high layer compat
 */
		NDBGL3(L3_P_MSG, "IEI_HLCOMPAT");
		break;
	case IEI_USERUSER:
/* 
 * user-user
 */
		NDBGL3(L3_P_MSG, "IEI_USER_USER");
		break;
	case IEI_ESCAPE:
/* 
 * escape for extension 
 */
		NDBGL3(L3_P_MSG, "IEI_ESCAPE");
		break;

/*********/
/* Q.932 */
/*********/
	case IEI_FACILITY:	
/* 
 * facility
 */
		NDBGL3(L3_P_MSG, "IEI_FACILITY");
		
		if (isdn_q932fac_aoc(msg_ptr, cd) > -1)
			i4b_l4_charging_ind(bc);
		
		break;

/*********/
/* Q.95x */
/*********/
	case IEI_CONCTDNO:
/* 
 * connected number	
 */
		NDBGL3(L3_P_MSG, "IEI_CONCTDNO");
		break;
	default:
		NDBGL3(L3_P_ERR, "Unknown IE %d - ", *msg_ptr);
		
		isdn_l2_print_frame(msg_ptr[1]+2, msg_ptr);
		break;
	}
	return (msg_ptr[1] + 2);
}

/*---------------------------------------------------------------------------*
 *	decode and process one Q.931 codeset 0 information element
 *---------------------------------------------------------------------------*/
static void 	
isdn_q931_decode_msg(struct isdn_bc *bc, uint8_t msg_type)
{
	const char *m = NULL;

	bc->bc_event = EV_ILL;

	switch (msg_type) {
/* 
 * call establishment 
 */
	case ALERT:
		m = "ALERT";
		
		bc->bc_event = EV_ALERT;
		break;
	case CALL_PROCEEDING:
		m = "CALL_PROCEEDING";
		
		bc->bc_event = EV_CALLPRC;
		break;
	case PROGRESS:
		m = "PROGRESS";
		
		bc->bc_event = EV_PROGIND;
		break;
	case SETUP:
		m = "SETUP";

		bc->bc_proto = BPROT_NONE;
		bc->bc_cause_in = 0;
		bc->bc_cause_out = 0;
		bc->bc_dst.se167_telno[0] = '\0';
		bc->bc_src.se167_telno[0] = '\0';
		bc->bc_id = CHAN_NO;
		bc->bc_excl = 0;
		bc->bc_display[0] = '\0';
		bc->bc_datetime[0] = '\0';
		bc->bc_event = EV_SETUP;
		break;
	case CONNECT:
		m = "CONNECT";
		
		bc->bc_datetime[0] = '\0';
		bc->bc_event = EV_CONNECT;
		break;
	case SETUP_ACKNOWLEDGE:
		m = "SETUP_ACKNOWLEDGE";	

		bc->bc_event = EV_SETUPAK;
		break;
	case CONNECT_ACKNOWLEDGE:
		m = "CONNECT_ACKNOWLEDGE";	

		bc->bc_event = EV_CONACK;
		break;
/* 
 * call information 
 */
	case USER_INFORMATION:
		m = "USER_INFORMATION";
		break;
	case SUSPEND_REJECT:
		m = "SUSPEND_REJECT";
		break;
	case RESUME_REJECT:
		m = "RESUME_REJECT";
		break;
	case HOLD:
		m = "HOLD";
		break;
	case SUSPEND:
		m = "SUSPEND";
		break;
	case RESUME:
		m = "RESUME";
		break;
	case HOLD_ACKNOWLEDGE:
		m = "HOLD_ACKNOWLEDGE";
		break;
	case SUSPEND_ACKNOWLEDGE:
		m = "SUSPEND_ACKNOWLEDGE";
		break;
	case RESUME_ACKNOWLEDGE:
		m = "RESUME_ACKNOWLEDGE";
		break;
	case HOLD_REJECT:
		m = "HOLD_REJECT";
		break;
	case RETRIEVE:
		m = "RETRIEVE";
		break;
	case RETRIEVE_ACKNOWLEDGE:
		m = "RETRIEVE_ACKNOWLEDGE";
		break;
	case RETRIEVE_REJECT:
		m = "RETRIEVE_REJECT";
		break;
/* 
 * call clearing 
 */
	case DISCONNECT:
		m = "DISCONNECT";

		bc->bc_event = EV_DISCONN;
	break;
	case RESTART:
		m = "RESTART";
		break;
	case RELEASE:
		m = "RELEASE";
		
		bc->bc_event = EV_RELEASE;
		break;
	case RESTART_ACKNOWLEDGE:
		m = "RESTART_ACKNOWLEDGE";
		break;
	case RELEASE_COMPLETE:
		m = "RELEASE_COMPLETE";
		
		bc->bc_event = EV_RELCOMP;
		break;
/* 
 * misc messages 
 */
	case SEGMENT:
		m = "SEGMENT";
		break;
	case FACILITY:
		m = "FACILITY";
		
		bc->bc_event = EV_FACILITY;
		break;
	case REGISTER:
		m = "REGISTER";
		break;
	case NOTIFY:
		m = "NOTIFY";
		break;
	case STATUS_ENQUIRY:
		m = "STATUS_ENQUIRY";
		
		bc->bc_event = EV_STATENQ;
		break;
	case CONGESTION_CONTROL:
		m = "CONGESTION_CONTROL";
		break;
	case INFORMATION:
		m = "INFORMATION";
		
		bc->bc_event = EV_INFO;
		break;
	case STATUS:
		m = "STATUS";
		
		bc->bc_event = EV_STATUS;
		break;
	default:
		NDBGL3(L3_P_ERR, "isdnif %d, "
			"cr = 0x%02x, msg = 0x%02x", 
			bc->bc_sc->sc_ifp->if_index, bc->bc_cr, msg_type);
		break;
	}
	
	if (m) {
		NDBGL3(L3_PRIM, "%s: isdnif %d, "
			"cr = 0x%02x\n", 
			m, bc->bc_sc->sc_ifp->if_index, bc->bc_cr);
	}
}
