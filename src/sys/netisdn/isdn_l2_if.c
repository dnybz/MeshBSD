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
	uint8_t ret;

	switch (GET_CAUSE_TYPE(cause)) {
	case CAUSET_Q850:
		ret = GET_CAUSE_VAL(cause);
		break;
	case CAUSET_I4B:
		ret = isdn_q931_cause_tab[GET_CAUSE_VAL(cause)];
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
isdn_l2_get_stat(struct isdn_bc *bc)
{
	
	return (bc->bc_sc->sc_l3.l3_dl_est);
}

/*---------------------------------------------------------------------------*
 *	DL ESTABLISH INDICATION from Layer 2
 *---------------------------------------------------------------------------*/
int
isdn_l2_establish_ind(struct isdn_softc *sc)
{
	struct isdn_bc *bc;
	int found, error;
	
	NDBGL2(L2_PRIM, "DL-ESTABLISH-IND isdnif %d", sc->sc_ifp->if_index);
/* 
 * first set DL up in controller descriptor 
 */
	NDBGL3(L3_MSG, "isdnif %d DL established!", sc->sc_ifp->if_index);
	sc->sc_l3.l3_dl_est = DL_UP;
	
	found = 0;
/* 
 * second, inform all (!) active call of the event 
 */
	TAILQ_FOREACH(bc, &sc->sc_bcq, bc_link) {
		isdn_l3_next_state(bc, EV_DLESTIN);
		found++;
	}

	if (found == 0) {
		NDBGL3(L3_ERR, "%s: no call-descriptor for isdnif %d found!",
		    __func__, sc->sc_ifp->if_index);
		error = -1;
	} else 
		error = 0; 

	return (error);
}

/*---------------------------------------------------------------------------*
 *	DL ESTABLISH CONFIRM from Layer 2
 *---------------------------------------------------------------------------*/
int
isdn_l2_establish_cnf(struct isdn_softc *sc)
{
	struct isdn_bc *bc;
	int found, error;

	NDBGL2(L2_PRIM, "DL-ESTABLISH-CONF isdnif %d", sc->sc_ifp->if_index);

	found = 0;

	TAILQ_FOREACH(bc, &sc->sc_bcq, bc_link) {
		sc->sc_l3.l3_dl_est = DL_UP;
		isdn_l3_next_state(bc, EV_DLESTCF);
		found++;
	}
	
	if (found == 0) {
		NDBGL3(L3_ERR, "ERROR, no cdid for isdnif %d found!",
		    sc->sc_ifp->if_index);
		error = -1;
	} else
		error = 0;
	
	return (error);
}

/*---------------------------------------------------------------------------*
 *	DL RELEASE INDICATION from Layer 2
 *---------------------------------------------------------------------------*/
int
isdn_l2_release_ind(struct isdn_softc *sc)
{
	struct isdn_bc *bc;
	int found;
	
	NDBGL2(L2_PRIM, "DL-RELEASE-IND isdnif %d", sc->sc_ifp->if_index);
/* 
 * first set controller to down 
 */
	sc->sc_l3.l3_dl_est = DL_DOWN;

	found = 0;
/* 
 * second, inform all (!) active calls of the event
 */
	TAILQ_FOREACH(bc, &sc->sc_bcq, bc_link) {
		isdn_l3_next_state(bc, EV_DLRELIN);
		found++;
	}

	if (found == 0) {
/* 
 * this is not an error since it might be a normal call end 
 */
		NDBGL3(L3_MSG, "no cdid for isdnif %d found", sc->sc_ifp->if_index);
	}
	return (0);
}

/*---------------------------------------------------------------------------*
 *	DL RELEASE CONFIRM from Layer 2
 *---------------------------------------------------------------------------*/
int
isdn_l2_release_cnf(struct isdn_softc *sc)
{
	NDBGL2(L2_PRIM, "DL-RELEASE-CONF isdnif %d", sc->sc_ifp->if_index);

	sc->sc_l3.l3_dl_est = DL_DOWN;
	return (0);
}

/*---------------------------------------------------------------------------*
 *	isdn_l2_data_ind - process a rx'd I-frame got from layer 2
 *---------------------------------------------------------------------------*/
int
isdn_l2_data_ind(struct isdn_softc *sc, struct mbuf *m)
{
	isdn_q931_decode(sc, m->m_len, mtod(m, uint8_t *));
	m_freem(m);
	return (0);
}

/*---------------------------------------------------------------------------*
 *	dl_unit_data_ind - process a rx'd U-frame got from layer 2
 *---------------------------------------------------------------------------*/
int
isdn_l2_unit_data_ind(struct isdn_softc *sc, struct mbuf *m)
{
	isdn_q931_decode(sc, m->m_len, mtod(m, uint8_t *));
	m_freem(m);
	return (0);
}

/*---------------------------------------------------------------------------*
 *	send CONNECT message
 *---------------------------------------------------------------------------*/
void
isdn_l3_tx_connect(struct isdn_bc *bc)
{
	struct mbuf *m;
	uint8_t *ptr;
	int len = I_FRAME_HDRLEN + MSG_CONNECT_LEN;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", 
		bc->bc_sc->sc_ifp->if_index, 
		bc->bc_cr);

	if ((m = isdn_getmbuf(len, M_DONTWAIT, MT_I4B_D)) == NULL) {
		panic("%s: can't allocate mbuf", __func__);
	}
	ptr = mtod(m, uint8_t *) + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(bc, bc->bc_cr);	/* call reference value */
	*ptr++ = CONNECT;		/* message type = connect */

	isdn_l2_data_req(bc->bc_sc, m);
}

/*---------------------------------------------------------------------------*
 *	send RELEASE COMPLETE message
 *---------------------------------------------------------------------------*/
void
isdn_l3_tx_release_complete(struct isdn_bc *bc, int send_cause_flag)
{
	struct mbuf *m;
	uint8_t *ptr;
	int len = I_FRAME_HDRLEN + MSG_RELEASE_COMPLETE_LEN;

	if (send_cause_flag == 0) {
		len -= 4;
		NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x",
			bc->bc_sc->sc_ifp->if_index, bc->bc_cr);
	} else {
		NDBGL3(L3_PRIM, "isdnif=%d, cr=0x%02x, cause=0x%x",
			bc->bc_sc->sc_ifp->if_index, 
			bc->bc_cr, bc->bc_cause_out);
	}

	if ((m = isdn_getmbuf(len, M_DONTWAIT, MT_I4B_D)) == NULL)
		panic("%s: can't allocate mbuf", __func__);

	ptr = mtod(m, uint8_t *) + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, bc->bc_cr);	/* call reference value */
	*ptr++ = RELEASE_COMPLETE;	/* message type = release complete */

	if (send_cause_flag) {
		*ptr++ = IEI_CAUSE;		/* cause ie */
		*ptr++ = CAUSE_LEN;
		*ptr++ = CAUSE_STD_LOC_OUT;
		*ptr++ = isdn_l2_mk_q931_cause(bc->bc_cause_out);
	}

	isdn_l2_data_req(bc->bc_sc, m);
}

/*---------------------------------------------------------------------------*
 *	send DISCONNECT message
 *---------------------------------------------------------------------------*/
void
isdn_l3_tx_disconnect(struct isdn_bc *bc)
{
	struct mbuf *m;
	uint8_t *ptr;
	int len = I_FRAME_HDRLEN + MSG_DISCONNECT_LEN;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", 
		bc->bc_sc->sc_ifp->if_index, bc->bc_cr);

	if ((m = isdn_getmbuf(len, M_DONTWAIT, MT_I4B_D)) == NULL) {
		panic("%s: can't allocate mbuf", __func__);
	}
	ptr = mtod(m, uint8_t *) + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call ref length */
	*ptr++ = setup_cr(bc, bc->bc_cr);	/* call reference value */
	*ptr++ = DISCONNECT;		/* message type = disconnect */

	*ptr++ = IEI_CAUSE;		/* cause ie */
	*ptr++ = CAUSE_LEN;
	*ptr++ = CAUSE_STD_LOC_OUT;
	*ptr++ = isdn_l2_mk_q931_cause(bc->bc_cause_out);

	isdn_l2_data_req(bc->bc_sc, m);
}

/*---------------------------------------------------------------------------*
 *	send SETUP message
 *---------------------------------------------------------------------------*/
void
isdn_l3_tx_setup(struct isdn_bc *bc)
{
	struct mbuf *m;
	uint8_t *ptr;
	int slen = strlen(bc->bc_src.se167_telno);
	int dlen = strlen(bc->bc_dst.se167_telno);
	int len = I_FRAME_HDRLEN + MSG_SETUP_LEN + slen + dlen +
			    (bc->bc_prot == BPROT_NONE ? 1 : 0);

	if (slen == 0)
		len -= (IEI_CALLINGPN_LEN + 2);	/* whole IE not send */

	/*
	 * there is one additional octet if bc->bc_prot == BPROT_NONE
	 * NOTE: the selection of a bearer capability by a B L1
	 *       protocol is highly questionable and a better
	 *       mechanism should be used in future. (-hm)
	 */

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", 
		bc->bc_sc->sc_ifp->if_index, bc->bc_cr);

	if ((m = isdn_getmbuf(len, M_DONTWAIT, MT_I4B_D)) == NULL) 
		panic("%s: can't allocate mbuf", __func__);

	bc->bc_crflag = CRF_ORIG;		/* we are the originating side */

	ptr = mtod(m, uint8_t) + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call ref length */
	*ptr++ = setup_cr(bc, bc->bc_cr);	/* call reference value */
	*ptr++ = SETUP;			/* message type = setup */

	*ptr++ = IEI_SENDCOMPL;		/* sending complete */

	*ptr++ = IEI_BEARERCAP;		/* bearer capability */

	/* XXX
	 * currently i have no idea if this should be switched by
	 * the choosen B channel protocol or if there should be a
	 * separate configuration item for the bearer capability.
	 * For now, it is switched by the choosen b protocol (-hm)
	 */

	switch (bc->bc_prot) {
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

	switch (bc->bc_id) {
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
		*ptr++ = IEI_CALLINGPN_LEN + slen;/* calling party no length */
		*ptr++ = NUMBER_TYPEPLAN;	/* type of number, number plan id */
		
		(void)strncpy(ptr, bc->bc_src.se167_telno, slen);
		
		ptr += slen;
	}

	*ptr++ = IEI_CALLEDPN;		/* called party no */
	*ptr++ = IEI_CALLEDPN_LEN + dlen;	/* called party no length */
	*ptr++ = NUMBER_TYPEPLAN;	/* type of number, number plan id */
	
	(void)strncpy(ptr, bc->bc_dst.se167_telno, dlen);
	
	ptr += dlen;

	isdn_l2_data_req(bc->bc_sc, m);
}

/*---------------------------------------------------------------------------*
 *	send CONNECT ACKNOWLEDGE message
 *---------------------------------------------------------------------------*/
void
isdn_l3_tx_connect_ack(struct isdn_bc *bc)
{
	struct mbuf *m;
	uint8_t *ptr;
	int len = I_FRAME_HDRLEN + MSG_CONNECT_ACK_LEN;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", 
		bc->bc_sc->sc_ifp->if_index, bc->bc_cr);

	if ((m = isdn_getmbuf(len, M_DONTWAIT, MT_I4B_D)) == NULL) {
		panic("%s: can't allocate mbuf", __func__);
	}
	ptr = mtod(m, uint8_t *) + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(bc, bc->bc_cr);	/* call reference value */
	*ptr++ = CONNECT_ACKNOWLEDGE;	/* message type = connect ack */

	isdn_l2_data_req(bc->bc_sc, m);
}

/*---------------------------------------------------------------------------*
 *	send STATUS message
 *---------------------------------------------------------------------------*/
void
isdn_l3_tx_status(struct isdn_bc *bc, uint8_t q850cause)
{
	struct mbuf *m;
	uint8_t *ptr;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", 
		bc->bc_sc->sc_ifp->if_index, bc->bc_cr);

	if ((m = isdn_getmbuf(I_FRAME_HDRLEN + MSG_STATUS_LEN,
		M_DONTWAIT, MT_I4B_D)) == NULL) {
		panic("%s: can't allocate mbuf", __func__);
	}
	ptr = mtod(m, uint8_t *) + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(bc, bc->bc_cr);	/* call reference value */
	*ptr++ = STATUS;	/* message type = connect ack */

	*ptr++ = IEI_CAUSE;		/* cause ie */
	*ptr++ = CAUSE_LEN;
	*ptr++ = CAUSE_STD_LOC_OUT;
	*ptr++ = q850cause | EXT_LAST;

	*ptr++ = IEI_CALLSTATE;		/* call state ie */
	*ptr++ = CALLSTATE_LEN;
	*ptr++ = isdn_l2_status_tab[bc->bc_Q931state];

	isdn_l2_data_req(bc->bc_sc, m);
}

/*---------------------------------------------------------------------------*
 *	send RELEASE message
 *---------------------------------------------------------------------------*/
void
isdn_l3_tx_release(struct isdn_bc *bc, int send_cause_flag)
{
	struct mbuf *m;
	uint8_t *ptr;
	int len = I_FRAME_HDRLEN + MSG_RELEASE_LEN;

	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", 
		bc->bc_sc->sc_ifp->if_index, bc->bc_cr);

	if (send_cause_flag == 0)
		len -= 4;

	if ((m = isdn_getmbuf(len, M_DONTWAIT, MT_I4B_D)) == NULL)
		panic("%s: can't allocate mbuf", __func__);

	ptr = mtod(m, uint8_t *) + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(bc, bc->bc_cr);	/* call reference value */
	*ptr++ = RELEASE;		/* message type = release complete */

	if (send_cause_flag) {
		*ptr++ = IEI_CAUSE;		/* cause ie */
		*ptr++ = CAUSE_LEN;
		*ptr++ = CAUSE_STD_LOC_OUT;
		*ptr++ = isdn_l2_mk_q931_cause(bc->bc_cause_out);
	}

	isdn_l2_data_req(bc->bc_sc, m);
}

/*---------------------------------------------------------------------------*
 *	send ALERTING message
 *---------------------------------------------------------------------------*/
void
isdn_l3_tx_alert(struct isdn_bc *bc)
{
	struct mbuf *m;
	uint8_t *ptr;
	int len = I_FRAME_HDRLEN + MSG_ALERT_LEN;

	if ((m = isdn_getmbuf(len, M_DONTWAIT, MT_I4B_D)) == NULL) {
		panic("%s: can't allocate mbuf", __func__);
	}
	NDBGL3(L3_PRIM, "isdnif %d, cr = 0x%02x", 
		bc->bc_sc->sc_ifp->if_index, bc->bc_cr);

	ptr = mtod(m, uint8_t *) + I_FRAME_HDRLEN;

	*ptr++ = PD_Q931;		/* protocol discriminator */
	*ptr++ = 0x01;			/* call reference length */
	*ptr++ = setup_cr(cd, bc->bc_cr);	/* call reference value */
	*ptr++ = ALERT;			/* message type = alert */

	isdn_l2_data_req(bc->bc_sc, m);
}
