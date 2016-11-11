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
 *	i4b_tei.c - tei handling procedures
 *	-----------------------------------
 *
 *	$Id: i4b_tei.c,v 1.10 2007/01/24 13:08:15 hubertf Exp $
 *
 * $FreeBSD$
 *
 *      last edit-date: [Fri Jan  5 11:33:47 2001]
 *
 *---------------------------------------------------------------------------*/

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


/*---------------------------------------------------------------------------*
 *	handle a received TEI management frame
 *---------------------------------------------------------------------------*/
void
isdn_l2_rxd_tei(struct isdn_softc *sc, struct mbuf *m)
{
	u_char *ptr = m->m_data;

	switch (*(ptr + OFF_MT)) {
	case MT_ID_ASSIGN:
		
		if (*(ptr + OFF_RIL) == sc->sc_l2.l2_last_ril) &&
			(*(ptr + OFF_RIH) == sc->sc_l2.l2_last_rih)) {
			
			sc->sc_l2.l2_tei = GET_TEIFROMAI(*(ptr+OFF_AI));
			sc->sc_l2.l2_tei_valid = TEI_VALID;

			if (sc->sc_l2.l2_T202 == TIMER_ACTIVE)
				i4b_T202_stop(l2sc);
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
			
			static int lasttei = -1;

			if (sc->sc_l2.l2_tei != lasttei) {
				NDBGL2(L2_TEI_MSG, 
					"TEI ID Check Req - TEI = %d", 
					sc->sc_l2.l2_tei);
				lasttei = sc->sc_l2.l2_tei;
			}

			if (sc->sc_l2.l2_T202 == TIMER_ACTIVE)
				i4b_T202_stop(l2sc);
			
			isdn_l2_chk_tei_resp(l2sc);
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
 *	allocate and fill up a TEI management frame for sending
 *---------------------------------------------------------------------------*/
static struct mbuf *
build_tei_mgmt_frame(l2_softc_t *l2sc, uint8_t type)
{
	struct mbuf *m;

	if ((m = isdn_getmbuf(TEI_MGMT_FRM_LEN, M_DONTWAIT, MT_I4B_D)) == NULL)
		return(NULL);

	m->m_data[TEIM_SAPIO] = 0xfc;	/* SAPI = 63, CR = 0, EA = 0 */
	m->m_data[TEIM_TEIO]  = 0xff;	/* TEI = 127, EA = 1 */
	m->m_data[TEIM_UIO]   = UI;	/* UI */
	m->m_data[TEIM_MEIO]  = MEI;	/* MEI */
	m->m_data[TEIM_MTO]   = type;	/* message type */

	switch (type) {
	case MT_ID_REQEST:
		i4b_make_rand_ri(l2sc);
		m->m_data[TEIM_RILO] = sc->sc_l2.l2_last_ril;
		m->m_data[TEIM_RIHO] = sc->sc_l2.l2_last_rih;
		m->m_data[TEIM_AIO] = (GROUP_TEI << 1) | 0x01;
		break;
	case MT_ID_CHK_RSP:
		i4b_make_rand_ri(l2sc);
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
		m_freem(m);
		panic("build_tei_mgmt_frame: invalid type");
		break;
	}
	sc->sc_l2.l2_stat.tx_tei++;
	
	return(m);
}

/*---------------------------------------------------------------------------*
 *	isdn_l2_assign_tei - TEI assignment procedure (Q.921, 5.3.2, pp 24)
 *	T202func and N202 _MUST_ be set prior to calling this function !
 *---------------------------------------------------------------------------*/
void
isdn_l2_assign_tei(l2_softc_t *l2sc)
{
	struct mbuf *m;

	NDBGL2(L2_TEI_MSG, "tx TEI ID_Request");

	m = build_tei_mgmt_frame(l2sc, MT_ID_REQEST);

	if (m == NULL)
		panic("isdn_l2_assign_tei: no mbuf");

	i4b_T202_start(l2sc);
/*
 * XXX
 * 
	sc->sc_l2.l2_driver->ph_data_req(sc->sc_l2.l2_l1_token, m, MBUF_FREE);
 */
}

/*---------------------------------------------------------------------------*
 *	isdn_l2_assign_tei - TEI verify procedure (Q.921, 5.3.5, pp 29)
 *	T202func and N202 _MUST_ be set prior to calling this function !
 *---------------------------------------------------------------------------*/
void
isdn_l2_verify_tei(l2_softc_t *l2sc)
{
	struct mbuf *m;

	NDBGL2(L2_TEI_MSG, "tx TEI ID_Verify");

	m = build_tei_mgmt_frame(l2sc, MT_ID_VERIFY);

	if (m == NULL)
		panic("isdn_l2_verify_tei: no mbuf");

	i4b_T202_start(l2sc);
/*
 * XXX
 * 
	sc->sc_l2.l2_driver->ph_data_req(sc->sc_l2.l2_l1_token, m, MBUF_FREE);
 */
}

/*---------------------------------------------------------------------------*
 *	isdn_l2_chk_tei_resp - TEI check response procedure (Q.921, 5.3.5, pp 29)
 *---------------------------------------------------------------------------*/
void
isdn_l2_chk_tei_resp(l2_softc_t *l2sc)
{
	struct mbuf *m;
	static int lasttei = 0;

	if (sc->sc_l2.l2_tei != lasttei) {
		lasttei = sc->sc_l2.l2_tei;
		NDBGL2(L2_TEI_MSG, "tx TEI ID_Check_Response");
	}

	m = build_tei_mgmt_frame(l2sc, MT_ID_CHK_RSP);

	if (m == NULL)
		panic("isdn_l2_chk_tei_resp: no mbuf");
/*
 * XXX
 * 
	sc->sc_l2.l2_driver->ph_data_req(sc->sc_l2.l2_l1_token, m, MBUF_FREE);
 */
}

/*---------------------------------------------------------------------------*
 *	generate some 16 bit "random" number used for TEI mgmt Ri field
 *---------------------------------------------------------------------------*/
void
i4b_make_rand_ri(l2_softc_t *l2sc)
{
	u_short val;

	(void)read_random((char *)&val, sizeof(val));

	sc->sc_l2.l2_last_rih = (val >> 8) & 0x00ff;
	sc->sc_l2.l2_last_ril = val & 0x00ff;
}
