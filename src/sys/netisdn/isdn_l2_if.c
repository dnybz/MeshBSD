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
 *
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

static uint8_t 	isdn_l2_mk_q931_cause(cause_t cause);

/*---------------------------------------------------------------------------*
 * this converts our internal state (number) to the number specified
 * in Q.931 and is used for reporting our state in STATUS messages.
 *---------------------------------------------------------------------------*/
int isdn_l2_status_tab[] = {
	0,	/*	ST_U0,	*/
	1,	/*	ST_U1,	*/
	3,	/*	ST_U3,	*/
	4,	/*	ST_U4,	*/
	6,	/*	ST_U6,	*/
	7,	/*	ST_U7,	*/
	8,	/*	ST_U8,	*/
	9,	/*	ST_U9,	*/
	10,	/*	ST_U10,	*/
	11,	/*	ST_U11,	*/
	12,	/*	ST_U12,	*/
	19,	/*	ST_U19,	*/
	6,	/*	ST_IWA,	*/
	6,	/*	ST_IWR,	*/
	1,	/*	ST_OW,	*/
	6,	/*	ST_IWL,	*/
};

/*---------------------------------------------------------------------------*
 *	return a valid q.931/q.850 cause from any of the internal causes
 *---------------------------------------------------------------------------*/
static uint8_t
isdn_l2_mk_q931_cause(cause_t cause)
{
	register uint8_t ret;

	switch (GET_CAUSE_TYPE(cause)) {
	case CAUSET_Q850:
		ret = GET_CAUSE_VAL(cause);
		break;
	case CAUSET_I4B:
		ret = cause_tab_q931[GET_CAUSE_VAL(cause)];
		break;
	default:
		panic("isdn_l2_mk_q931_cause: unknown cause type!");
		break;
	}
	ret |= EXT_LAST;
	return (ret);
}

/*---------------------------------------------------------------------------*
 *	return status of data link
 *---------------------------------------------------------------------------*/
int
i4b_get_dl_stat(struct isdn_b_chan *bc)
{
	const struct isdn_l3 * l3 = cd->l3l3;
	return (sc->sc_l3.l3_dl_est);
}

/*---------------------------------------------------------------------------*
 *	DL ESTABLISH INDICATION from Layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_establish_ind(struct isdn_l3 * l3)
{
	int i, found;
	NDBGL2(L2_PRIM, "DL-ESTABLISH-IND isdnif %d", sc->sc_l3.l3_isdnif);
/* 
 * first set DL up in controller descriptor 
 */
	NDBGL3(L3_MSG, "isdnif %d DL established!", sc->sc_l3.l3_isdnif);
	sc->sc_l3.l3_dl_est = DL_UP;

	found = 0;
/* 
 * second, inform all (!) active call of the event 
 */
	for (i = 0; i < N_BCH; i++) {
		if ( (call_desc[i].cdid != 0)
		    && call_desc[i].isdnif == sc->sc_l3.l3_isdnif) {
			next_l3state(&call_desc[i], EV_DLESTIN);
			found++;
		}
	}

	if (found == 0) {
		NDBGL3(L3_ERR, "ERROR, no cdid for isdnif %d found!",
		    sc->sc_l3.l3_isdnif);
		return (-1);
	} 

	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL ESTABLISH CONFIRM from Layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_establish_cnf(struct isdn_l3 * l3)
{
	int i;
	int found = 0;

	NDBGL2(L2_PRIM, "DL-ESTABLISH-CONF isdnif %d", sc->sc_l3.l3_isdnif);

	for (i = 0; i < N_BCH; i++) {
		if (call_desc[i].cdid != 0
		    && call_desc[i].isdnif == sc->sc_l3.l3_isdnif) {
			sc->sc_l3.l3_dl_est = DL_UP;
			next_l3state(&call_desc[i], EV_DLESTCF);
			found++;
		}
	}

	if (found == 0) {
		NDBGL3(L3_ERR, "ERROR, no cdid for isdnif %d found!",
		    sc->sc_l3.l3_isdnif);
		return (-1);
	}
	
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL RELEASE INDICATION from Layer 2
 *---------------------------------------------------------------------------*/
int
isdn_l2_release_ind(struct isdn_softc *sc)
{
	int i, found;

	NDBGL2(L2_PRIM, "DL-RELEASE-IND isdnif %d", sc->sc_l3.l3_isdnif);
/* 
 * first set controller to down 
 */
	sc->sc_l3.l3_dl_est = DL_DOWN;

	found = 0;
/* 
 * second, inform all (!) active calls of the event
 *
	for (i = 0; i < N_BCH; i++) {
		if (call_desc[i].cdid != 0
		    && call_desc[i].isdnif == sc->sc_l3.l3_isdnif) {
			next_l3state(&call_desc[i], EV_DLRELIN);
			found++;
		}
	}
 */
	if (found == 0) {
/* 
 * this is not an error since it might be a normal call end 
 */
		NDBGL3(L3_MSG, "no cdid for isdnif %d found", sc->sc_l3.l3_isdnif);
	}
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL RELEASE CONFIRM from Layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_release_cnf(struct isdn_l3 * l3)
{
	NDBGL2(L2_PRIM, "DL-RELEASE-CONF isdnif %d", sc->sc_l3.l3_isdnif);

	sc->sc_l3.l3_dl_est = DL_DOWN;
	return (0);
}

/*---------------------------------------------------------------------------*
 *	i4b_dl_data_ind - process a rx'd I-frame got from layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_data_ind(struct isdn_l3 *l3, struct mbuf *m)
{
	i4b_decode_q931(sc->sc_l3.l3_isdnif, m->m_len, m->m_data);
	m_freem(m);
	return (0);
}

/*---------------------------------------------------------------------------*
 *	dl_unit_data_ind - process a rx'd U-frame got from layer 2
 *---------------------------------------------------------------------------*/
int
i4b_dl_unit_data_ind(struct isdn_l3 *l3, struct mbuf *m)
{
	i4b_decode_q931(sc->sc_l3.l3_isdnif, m->m_len, m->m_data);
	m_freem(m);
	return (0);
}

/*---------------------------------------------------------------------------*
 *	send CONNECT message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_connect(struct isdn_b_chan *bc)
{
	struct isdn_l2 *l2sc = (isdn_l2_t*)cd->l3sc->sc_l3.l3_l1_token;
	struct mbuf *m;
	u_char *ptr;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if ((m = isdn_getmbuf(I_FRAME_HDRLEN + MSG_CONNECT_LEN,
		M_DONTWAIT, MT_I4B_D)) == NULL) {
		panic("%s: can't allocate mbuf", __func__);
	}
	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = CONNECT;		/* message type = connect */

	i4b_dl_data_req(l2sc, m);
}

/*---------------------------------------------------------------------------*
 *	send RELEASE COMPLETE message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_release_complete(struct isdn_b_chan *bc, int send_cause_flag)
{
	struct isdn_l2 *l2sc = (isdn_l2_t*)cd->l3sc->sc_l3.l3_l1_token;
	struct mbuf *m;
	u_char *ptr;
	int len = I_FRAME_HDRLEN + MSG_RELEASE_COMPLETE_LEN;

	if (send_cause_flag == 0) {
		len -= 4;
		NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x",
			cd->isdnif, cd->cr);
	} else {
		NDBGL3(L3_PRIM, "isdnif=%d, cr=0x%02x, cause=0x%x",
			cd->isdnif, cd->cr, cd->cause_out);
	}

	if ((m = isdn_getmbuf(len, M_DONTWAIT, MT_I4B_D)) == NULL)
		panic("%s: can't allocate mbuf", __func__);

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = RELEASE_COMPLETE;	/* message type = release complete */

	if (send_cause_flag) {
		*ptr++ = IEI_CAUSE;		/* cause ie */
		*ptr++ = CAUSE_LEN;
		*ptr++ = CAUSE_STD_LOC_OUT;
		*ptr++ = isdn_l2_mk_q931_cause(cd->cause_out);
	}

	i4b_dl_data_req(sc, m);
}

/*---------------------------------------------------------------------------*
 *	send DISCONNECT message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_disconnect(struct isdn_b_chan *bc)
{
	struct isdn_l2 *l2sc = (isdn_l2_t*)cd->l3sc->sc_l3.l3_l1_token;
	struct mbuf *m;
	u_char *ptr;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if ((m = isdn_getmbuf(I_FRAME_HDRLEN + MSG_DISCONNECT_LEN, 
		M_DONTWAIT, MT_I4B_D)) == NULL) {
		panic("%s: can't allocate mbuf", __func__);
	}
	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call ref length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = DISCONNECT;		/* message type = disconnect */

	*ptr++ = IEI_CAUSE;		/* cause ie */
	*ptr++ = CAUSE_LEN;
	*ptr++ = CAUSE_STD_LOC_OUT;
	*ptr++ = isdn_l2_mk_q931_cause(cd->cause_out);

	i4b_dl_data_req(l2sc, m);
}

/*---------------------------------------------------------------------------*
 *	send SETUP message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_setup(struct isdn_b_chan *bc)
{
	struct isdn_l2 *l2sc = (isdn_l2_t*)cd->l3sc->sc_l3.l3_l1_token;
	struct mbuf *m;
	u_char *ptr;
	int slen = strlen(cd->src_telno);
	int dlen = strlen(cd->dst_telno);
	int msglen = I_FRAME_HDRLEN + MSG_SETUP_LEN + slen + dlen +
			    (cd->bprot == BPROT_NONE ? 1 : 0);

	if (slen == 0)
		msglen -= IEI_CALLINGPN_LEN+2;	/* whole IE not send */

	/*
	 * there is one additional octet if cd->bprot == BPROT_NONE
	 * NOTE: the selection of a bearer capability by a B L1
	 *       protocol is highly questionable and a better
	 *       mechanism should be used in future. (-hm)
	 */

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if ((m = isdn_getmbuf(msglen, M_DONTWAIT, MT_I4B_D)) == NULL) 
		panic("%s: can't allocate mbuf", __func__);

	cd->crflag = CRF_ORIG;		/* we are the originating side */

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call ref length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = SETUP;			/* message type = setup */

	*ptr++ = IEI_SENDCOMPL;		/* sending complete */

	*ptr++ = IEI_BEARERCAP;		/* bearer capability */

	/* XXX
	 * currently i have no idea if this should be switched by
	 * the choosen B channel protocol or if there should be a
	 * separate configuration item for the bearer capability.
	 * For now, it is switched by the choosen b protocol (-hm)
	 */

	switch (cd->bprot) {
	case BPROT_NONE:	
/* 
 * telephony 
 */
		*ptr++ = IEI_BEARERCAP_LEN+1;
		*ptr++ = IT_CAP_SPEECH;
		*ptr++ = IT_RATE_64K;
		*ptr++ = IT_UL1_G711A;
		break;
	case BPROT_RHDLC:	
/* 
 * raw HDLC 
 */
		*ptr++ = IEI_BEARERCAP_LEN;
		*ptr++ = IT_CAP_UNR_DIG_INFO;
		*ptr++ = IT_RATE_64K;
		break;
	default:
		*ptr++ = IEI_BEARERCAP_LEN;
		*ptr++ = IT_CAP_UNR_DIG_INFO;
		*ptr++ = IT_RATE_64K;
		break;
	}

	*ptr++ = IEI_CHANNELID;		/* channel id */
	*ptr++ = IEI_CHANNELID_LEN;	/* channel id length */

	switch (cd->channelid) {
	case CHAN_B1:
		*ptr++ = CHANNELID_B1;
		break;
	case CHAN_B2:
		*ptr++ = CHANNELID_B2;
		break;
	default:
		*ptr++ = CHANNELID_ANY;
		break;
	}

	if (slen) {
		*ptr++ = IEI_CALLINGPN;		/* calling party no */
		*ptr++ = IEI_CALLINGPN_LEN+slen;/* calling party no length */
		*ptr++ = NUMBER_TYPEPLAN;	/* type of number, number plan id */
		strncpy(ptr, cd->src_telno, slen);
		ptr += slen;
	}

	*ptr++ = IEI_CALLEDPN;		/* called party no */
	*ptr++ = IEI_CALLEDPN_LEN+dlen;	/* called party no length */
	*ptr++ = NUMBER_TYPEPLAN;	/* type of number, number plan id */
	strncpy(ptr, cd->dst_telno, dlen);
	ptr += dlen;

	i4b_dl_data_req(l2sc, m);
}

/*---------------------------------------------------------------------------*
 *	send CONNECT ACKNOWLEDGE message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_connect_ack(struct isdn_b_chan *bc)
{
	struct isdn_l2 *l2sc = (isdn_l2_t*)cd->l3sc->sc_l3.l3_l1_token;
	struct mbuf *m;
	u_char *ptr;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if ((m = isdn_getmbuf(I_FRAME_HDRLEN + MSG_CONNECT_ACK_LEN, 
		M_DONTWAIT, MT_I4B_D)) == NULL) {
		panic("%s: can't allocate mbuf", __func__);
	}
	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = CONNECT_ACKNOWLEDGE;	/* message type = connect ack */

	i4b_dl_data_req(l2sc, m);
}

/*---------------------------------------------------------------------------*
 *	send STATUS message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_status(struct isdn_b_chan *bc, u_char q850cause)
{
	struct isdn_l2 *l2sc = (isdn_l2_t*)cd->l3sc->sc_l3.l3_l1_token;
	struct mbuf *m;
	u_char *ptr;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if ((m = isdn_getmbuf(I_FRAME_HDRLEN + MSG_STATUS_LEN,
		M_DONTWAIT, MT_I4B_D)) == NULL) {
		panic("%s: can't allocate mbuf", __func__);
	}
	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = STATUS;	/* message type = connect ack */

	*ptr++ = IEI_CAUSE;		/* cause ie */
	*ptr++ = CAUSE_LEN;
	*ptr++ = CAUSE_STD_LOC_OUT;
	*ptr++ = q850cause | EXT_LAST;

	*ptr++ = IEI_CALLSTATE;		/* call state ie */
	*ptr++ = CALLSTATE_LEN;
	*ptr++ = isdn_l2_status_tab[cd->Q931state];

	i4b_dl_data_req(l2sc, m);
}

/*---------------------------------------------------------------------------*
 *	send RELEASE message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_release(struct isdn_b_chan *bc, int send_cause_flag)
{
	struct isdn_l2 *l2sc = (isdn_l2_t*)cd->l3sc->sc_l3.l3_l1_token;
	struct mbuf *m;
	u_char *ptr;
	int len = I_FRAME_HDRLEN + MSG_RELEASE_LEN;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	if (send_cause_flag == 0)
		len -= 4;

	if ((m = isdn_getmbuf(len, M_DONTWAIT, MT_I4B_D)) == NULL)
		panic("%s: can't allocate mbuf", __func__);

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = RELEASE;		/* message type = release complete */

	if (send_cause_flag) {
		*ptr++ = IEI_CAUSE;		/* cause ie */
		*ptr++ = CAUSE_LEN;
		*ptr++ = CAUSE_STD_LOC_OUT;
		*ptr++ = isdn_l2_mk_q931_cause(cd->cause_out);
	}

	i4b_dl_data_req(l2sc, m);
}

/*---------------------------------------------------------------------------*
 *	send ALERTING message
 *---------------------------------------------------------------------------*/
void
i4b_l3_tx_alert(struct isdn_b_chan *bc)
{
	struct isdn_l2 *l2sc = (isdn_l2_t*)cd->l3sc->sc_l3.l3_l1_token;
	struct mbuf *m;
	u_char *ptr;

	if ((m = isdn_getmbuf(I_FRAME_HDRLEN + MSG_ALERT_LEN,
		M_DONTWAIT, MT_I4B_D)) == NULL) {
		panic("%s: can't allocate mbuf", __func__);
	}
	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", cd->isdnif, cd->cr);

	ptr = m->m_data + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, cd->cr);	/* call reference value */
	*ptr++ = ALERT;			/* message type = alert */

	i4b_dl_data_req(l2sc, m);
}
