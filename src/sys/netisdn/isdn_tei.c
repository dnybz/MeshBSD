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
#include <sys/syslog.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <sys/random.h>

#include <netisdn/isdn.h>
#include <netisdn/isdn_var.h>

#include <netisdn/isdn_l2.h>
#include <netisdn/isdn_mbuf.h>
#include <netisdn/isdn_l2_fsm.h>

/* 
 * Structure of a TEI management frame.
 */

#define TEI_MGMT_FRM_LEN   8		/* frame length */
#define TEIM_SAPIO	0x00		/* SAPI, CR, EA */
#define TEIM_TEIO	0x01		/* TEI, EA */
#define TEIM_UIO	0x02		/* frame type = UI = 0x03 */
#define TEIM_MEIO	0x03		/* management entity id = 0x0f */
#define 	MEI	0x0f
#define TEIM_RILO	0x04		/* reference number, low  */
#define TEIM_RIHO	0x05		/* reference number, high */
#define TEIM_MTO	0x06		/* message type */
#define 	MT_ID_REQEST	0x01
#define 	MT_ID_ASSIGN	0x02
#define 	MT_ID_DENY	0x03
#define 	MT_ID_CHK_REQ	0x04
#define 	MT_ID_CHK_RSP	0x05
#define 	MT_ID_REMOVE	0x06
#define 	MT_ID_VERIFY	0x07
#define TEIM_AIO	0x07		/* action indicator */


static void 	isdn_tei_mk_rand_ri(struct isdn_softc *);
static int 	isdn_tei_tx(struct isdn_softc *, uint8_t);

/*---------------------------------------------------------------------------*
 *	handle a received TEI management frame
 *---------------------------------------------------------------------------*/
void
isdn_tei_rxd(struct isdn_softc *sc, struct mbuf *m)
{
	u_char *ptr = mtod(m, uint8_t);

	switch (*(ptr + OFF_MT)) {
	case MT_ID_ASSIGN:
		
		if (*(ptr + OFF_RIL) == sc->sc_l2.l2_last_ril) &&
			(*(ptr + OFF_RIH) == sc->sc_l2.l2_last_rih)) {
			
			sc->sc_l2.l2_tei = GET_TEIFROMAI(*(ptr+OFF_AI));
			sc->sc_l2.l2_tei_valid = TEI_VALID;

			if (sc->sc_l2.l2_T202 == TIMER_ACTIVE)
				isdn_T202_stop(sc);
/*
 * XXX ...
 *
			i4b_mdl_status_ind(drv, STI_TEIASG, sc->sc_l2.l2_tei);
 */
			log(LOG_INFO, "isdn: isdnif %d, assigned "
				"TEI = %d = 0x%02x\n", 
				sc->sc_ifp->if_index, 
				sc->sc_l2.l2_tei, sc->sc_l2.l2_tei);

			NDBGL2(L2_TEI_MSG, "TEI ID Assign - TEI = %d", 
				sc->sc_l2.l2_tei);

			isdn_l2_next_state(sc, EV_MDASGRQ);
		}
		break;
	case MT_ID_DENY:
		if (*(ptr + OFF_RIL) == sc->sc_l2.l2_last_ril) &&
			(*(ptr + OFF_RIH) == sc->sc_l2.l2_last_rih)) {
			
			sc->sc_l2.l2_tei_valid = TEI_INVALID;
			sc->sc_l2.l2_tei = GET_TEIFROMAI(*(ptr+OFF_AI));

			if (sc->sc_l2.l2_tei == GROUP_TEI) {
				log(LOG_WARNING, "isdn: isdnif %d, denied TEI, "
					"no TEI values available from exchange!\n", 
						sc->sc_ifp->if_index);
				
				NDBGL2(L2_TEI_ERR, "TEI ID Denied, No TEI "
					"values available from exchange!");
			} else {
				log(LOG_WARNING, "isdn: isdnif %d, "
					"denied TEI = %d = 0x%02x\n", 
					sc->sc_ifp->if_index, sc->sc_l2.l2_tei, 
					sc->sc_l2.l2_tei);
				NDBGL2(L2_TEI_ERR, 
					"TEI ID Denied - TEI = %d", 
					sc->sc_l2.l2_tei);
			}
/*
 * XXX ...
 *	
			i4b_mdl_status_ind(drv, STI_TEIASG, -1);
 */
			isdn_l2_next_state(sc, EV_MDERRRS);
		}
		break;
	case MT_ID_CHK_REQ:
		
		if (sc->sc_l2.l2_tei_valid == TEI_VALID) && 
			((sc->sc_l2.l2_tei == GET_TEIFROMAI(*(ptr+OFF_AI))) || 
			(GROUP_TEI == GET_TEIFROMAI(*(ptr+OFF_AI))))) {
			
			sc->sc_l2.l2_tei_last = -1;

			if (sc->sc_l2.l2_tei != sc->sc_l2.l2_tei_last) {
				NDBGL2(L2_TEI_MSG, 
					"TEI ID Check Req - TEI = %d", 
					sc->sc_l2.l2_tei);
				sc->sc_l2.l2_tei_last = sc->sc_l2.l2_tei;
			}

			if (sc->sc_l2.l2_T202 == TIMER_ACTIVE)
				isdn_T202_stop(sc);
			
			isdn_tei_chk_resp(sc);
		}
		break;
	case MT_ID_REMOVE:
		if (sc->sc_l2.l2_tei_valid == TEI_VALID) && 
			((sc->sc_l2.l2_tei == GET_TEIFROMAI(*(ptr+OFF_AI))) || 
			(sc->sc_l2.l2_tei == GET_TEIFROMAI(*(ptr+OFF_AI))))) {
			
			sc->sc_l2.l2_tei_valid = TEI_INVALID;
			sc->sc_l2.l2_tei = GET_TEIFROMAI(*(ptr+OFF_AI));

			log(LOG_INFO, "isdn: isdnif %d, "
				"removed TEI = %d = 0x%02x\n", 
				drv->isdnif, sc->sc_l2.l2_tei, sc->sc_l2.l2_tei);
				
			NDBGL2(L2_TEI_MSG, "TEI ID Remove - TEI = %d", sc->sc_l2.l2_tei);
/*
 * XXX
 *
			i4b_mdl_status_ind(drv, STI_TEIASG, -1);
 */
			isdn_l2_next_state(sc, EV_MDREMRQ);
		}
		break;
	default:
		NDBGL2(L2_TEI_ERR, "UNKNOWN TEI MGMT Frame, "
			"type = 0x%x", *(ptr + OFF_MT));
		isdn_l2_print_frame(m->m_len, m->m_data);
		break;
	}
	m_freem(m);
}

/*---------------------------------------------------------------------------*
 *	isdn_tei_assign - TEI assignment procedure (Q.921, 5.3.2, pp 24)
 *	T202func and N202 _MUST_ be set prior to calling this function !
 *---------------------------------------------------------------------------*/
void
isdn_tei_assign(struct isdn_softc *sc)
{
	int error;

	NDBGL2(L2_TEI_MSG, "tx TEI ID_Request");

	error = isdn_tei_tx(sc, MT_ID_REQEST);

	if (error == ENOBUFS)
		panic("%s: no mbuf", __func__);
}

/*---------------------------------------------------------------------------*
 *	isdn_tei_assign - TEI verify procedure (Q.921, 5.3.5, pp 29)
 *	T202func and N202 _MUST_ be set prior to calling this function !
 *---------------------------------------------------------------------------*/
void
isdn_tei_verify(struct isdn_softc *sc)
{
	int error;

	NDBGL2(L2_TEI_MSG, "tx TEI ID_Verify");

	error = isdn_tei_tx(sc, MT_ID_VERIFY);

	if (error == ENOBUFS)
		panic("%s: no mbuf", __func__);
}

/*---------------------------------------------------------------------------*
 *	isdn_tei_chk_resp - TEI check response procedure (Q.921, 5.3.5, pp 29)
 *---------------------------------------------------------------------------*/
void
isdn_tei_chk_resp(struct isdn_softc *sc)
{
	int lasttei = 0, error;

	if (sc->sc_l2.l2_tei != lasttei) {
		lasttei = sc->sc_l2.l2_tei;
		NDBGL2(L2_TEI_MSG, "tx TEI ID_Check_Response");
	}
	
	error = isdn_tei_tx(sc, MT_ID_CHK_RSP);

	if (error == ENOBUFS)
		panic("%s: no mbuf", __func__);
}

/*---------------------------------------------------------------------------*
 *	allocate and fill up a TEI management frame for sending
 *---------------------------------------------------------------------------*/
static int
isdn_tei_tx(struct isdn_softc *sc, uint8_t type)
{
	struct mbuf *m;
	int error;

	if ((m = isdn_getmbuf(TEI_MGMT_FRM_LEN, M_NOWAIT, MT_I4B_D)) == NULL) {
		error = ENOBUFS;
		goto out;
	}
	m->m_flags |= M_BCAST;
		
	m->m_data[TEIM_SAPIO] = 0xfc; /* SAPI = 63, CR = 0, EA = 0 */
	m->m_data[TEIM_TEIO]  = 0xff;	/* TEI = 127, EA = 1 */
	m->m_data[TEIM_UIO]   = UI;	/* UI */
	m->m_data[TEIM_MEIO]  = MEI;	/* MEI */
	m->m_data[TEIM_MTO]   = type;	/* message type */

	switch (type) {
	case MT_ID_REQEST:
		isdn_tei_mk_rand_ri(sc);
		m->m_data[TEIM_RILO] = sc->sc_l2.l2_last_ril;
		m->m_data[TEIM_RIHO] = sc->sc_l2.l2_last_rih;
		m->m_data[TEIM_AIO] = (GROUP_TEI << 1) | 0x01;
		break;
	case MT_ID_CHK_RSP:
		isdn_tei_mk_rand_ri(sc);
		m->m_data[TEIM_RILO] = sc->sc_l2.l2_last_ril;
		m->m_data[TEIM_RIHO] = sc->sc_l2.l2_last_rih;
		m->m_data[TEIM_AIO] = (sc->sc_l2.l2_tei << 1) | 0x01;
		break;
	case MT_ID_VERIFY:
		m->m_data[TEIM_RILO] = 0;
		m->m_data[TEIM_RIHO] = 0;
		m->m_data[TEIM_AIO] = (sc->sc_l2.l2_tei << 1) | 0x01;
		break;
	default:
		error = EINVAL;
		m_freem(m);
		goto out;
	}
	sc->sc_l2.l2_stat.tx_tei++;
	
	switch (type) {
	case MT_ID_REQEST:
	case MT_ID_VERIFY:
		isdn_T202_start(sc);
		break;
	case MT_ID_CHK_RSP:
		break;
	default:
		break;
	}
	error = isdn_output(sc->sc_ifp, m, ISDN_D_CHAN, 0, 0xfc, 0xff);
out:	
	return (error);
}

/*---------------------------------------------------------------------------*
 *	generate some 16 bit "random" number used for TEI mgmt Ri field
 *---------------------------------------------------------------------------*/
static void
isdn_tei_mk_rand_ri(struct isdn_softc *sc)
{
	uint16_t val = 0;

	arc4rand(&val, sizeof(val), 0);

	sc->sc_l2.l2_last_rih = (val >> 8) & 0x00ff;
	sc->sc_l2.l2_last_ril = val & 0x00ff;
}
