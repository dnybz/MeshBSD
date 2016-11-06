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
 *	i4b_uframe.c - routines for handling U-frames
 *	-----------------------------------------------
 *
 *	$Id: i4b_uframe.c,v 1.8 2007/01/24 13:08:15 hubertf Exp $
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
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/i4b_debug.h>
#include <netisdn/i4b_ioctl.h>

#include <netisdn/i4b_l2.h>
#include <netisdn/i4b_l1l2.h>
#include <netisdn/i4b_isdnq931.h>
#include <netisdn/i4b_mbuf.h>

#include <netisdn/i4b_l2fsm.h>

/*---------------------------------------------------------------------------*
 *	process a received U-frame
 *---------------------------------------------------------------------------*/
void
i4b_rxd_u_frame(struct isdn_sc *sc, struct mbuf *m)
{
	u_char *ptr = m->m_data;

	int sapi = GETSAPI(*(ptr + OFF_SAPI));
	int tei = GETTEI(*(ptr + OFF_TEI));
	int pfbit = GETUPF(*(ptr + OFF_CNTL));

	switch (*(ptr + OFF_CNTL) & ~UPFBIT) {
/* 
 * commands 
 */
	case SABME:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			l2->stat.rx_sabme++;
			NDBGL2(L2_U_MSG, "SABME, sapi = %d, tei = %d", sapi, tei);
			l2->rxd_PF = pfbit;
			i4b_next_l2state(l2, l3, EV_RXSABME);
		}
		m_freem(m);
		break;
	case UI:
		if ((sapi == SAPI_L2M) && 
			(tei == GROUP_TEI) &&
			   (*(ptr + OFF_MEI) == MEI)) {
/* 
 * layer 2 management (SAPI = 63) 
 */
			l2->stat.rx_tei++;
			i4b_tei_rxframe(l2, l3, m);
		} else if ((sapi == SAPI_CCP) && (tei == GROUP_TEI)) {
/* 
 * call control (SAPI = 0) 
 */
			l2->stat.rx_ui++;
/* 
 * strip ui header 
 */
			m_adj(m, UI_HDR_LEN);
/* 
 * to upper layer 
 */
			i4b_dl_unit_data_ind(l2->l3, m);
		} else {
			l2->stat.err_rx_badui++;
			NDBGL2(L2_U_ERR, "unknown UI frame!");
			m_freem(m);
		}
		break;
	case DISC:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			l2->stat.rx_disc++;
			NDBGL2(L2_U_MSG, "DISC, sapi = %d, tei = %d", sapi, tei);
			l2->rxd_PF = pfbit;
			i4b_next_l2state(l2, l3, EV_RXDISC);
		}
		m_freem(m);
		break;
	case XID:
		
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			l2->stat.rx_xid++;
			NDBGL2(L2_U_MSG, "XID, sapi = %d, tei = %d", sapi, tei);
		}
		m_freem(m);
		break;
/* 
 * responses 
 */
	case DM:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
				l2->stat.rx_dm++;
			NDBGL2(L2_U_MSG, "DM, sapi = %d, tei = %d", sapi, tei);
			i4b_print_frame(m->m_len, m->m_data);
			l2->rxd_PF = pfbit;
			i4b_next_l2state(l2, l3, EV_RXDM);
		}
		m_freem(m);
		break;
	case UA:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			l2->stat.rx_ua++;
			NDBGL2(L2_U_MSG, "UA, sapi = %d, tei = %d", sapi, tei);
			l2->rxd_PF = pfbit;
			i4b_next_l2state(l2, l3, EV_RXUA);
		}
		m_freem(m);
		break;
	case FRMR:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			l2->stat.rx_frmr++;
			NDBGL2(L2_U_MSG, "FRMR, sapi = %d, tei = %d", sapi, tei);
			l2->rxd_PF = pfbit;
			i4b_next_l2state(l2, l3, EV_RXFRMR);
		}
		m_freem(m);
		break;
	default:
		if ((l2->tei_valid == TEI_VALID) && 
			(l2->tei == GETTEI(*(ptr+OFF_TEI)))) {
			NDBGL2(L2_U_ERR, "UNKNOWN TYPE ERROR, sapi = %d, "
				"tei = %d, frame = ", sapi, tei);
			i4b_print_frame(m->m_len, m->m_data);
		} else {
			NDBGL2(L2_U_ERR, "not mine -  UNKNOWN TYPE ERROR, "
				"sapi = %d, tei = %d, frame = ", sapi, tei);
			i4b_print_frame(m->m_len, m->m_data);
		}
		l2->stat.err_rx_badui++;
		m_freem(m);
		break;
	}
}

/*---------------------------------------------------------------------------*
 *	build U-frame for sending
 *---------------------------------------------------------------------------*/
struct mbuf *
i4b_build_u_frame(struct isdn_l2 *l2, crbit_to_nt_t crbit, 
	pbit_t pbit, u_char type)
{
	struct mbuf *m;

	if ((m = i4b_getmbuf(U_FRAME_LEN, M_DONTWAIT, MT_I4B_D)) == NULL)
		goto out;

	PUTSAPI(SAPI_CCP, crbit, m->m_data[OFF_SAPI]);

	PUTTEI(l2->tei, m->m_data[OFF_TEI]);

	if (pbit)
		m->m_data[OFF_CNTL] = type | UPBITSET;
	else
		m->m_data[OFF_CNTL] = type & ~UPBITSET;
out:
	return (m);
}

/*---------------------------------------------------------------------------*
 *	transmit SABME command
 *---------------------------------------------------------------------------*/
void
i4b_tx_sabme(struct isdn_l2 *l2, pbit_t pbit)
{
	struct mbuf *m;

	l2->stat.tx_sabme++;
	NDBGL2(L2_U_MSG, "tx SABME, tei = %d", l2->tei);
	m = i4b_build_u_frame(l2, CR_CMD_TO_NT, pbit, SABME);
	i4b_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit DM response
 *---------------------------------------------------------------------------*/
void
i4b_tx_dm(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	l2->stat.tx_dm++;
	NDBGL2(L2_U_MSG, "tx DM, tei = %d", l2->tei);
	m = i4b_build_u_frame(l2, CR_RSP_TO_NT, fbit, DM);
	i4b_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit DISC command
 *---------------------------------------------------------------------------*/
void
i4b_tx_disc(struct isdn_l2 *l2, pbit_t pbit)
{
	struct mbuf *m;

	l2->stat.tx_disc++;
	NDBGL2(L2_U_MSG, "tx DISC, tei = %d", l2->tei);
	m = i4b_build_u_frame(l2, CR_CMD_TO_NT, pbit, DISC);
	i4b_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit UA response
 *---------------------------------------------------------------------------*/
void
i4b_tx_ua(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	l2->stat.tx_ua++;
	NDBGL2(L2_U_MSG, "tx UA, tei = %d", l2->tei);
	m = i4b_build_u_frame(l2, CR_RSP_TO_NT, fbit, UA);
	i4b_output(l2, m, MBUF_FREE);
}

/*---------------------------------------------------------------------------*
 *	transmit FRMR response
 *---------------------------------------------------------------------------*/
void
i4b_tx_frmr(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	l2->stat.tx_frmr++;
	NDBGL2(L2_U_MSG, "tx FRMR, tei = %d", l2->tei);
	m = i4b_build_u_frame(l2, CR_RSP_TO_NT, fbit, FRMR);
	i4b_output(l2, m, MBUF_FREE);
}

