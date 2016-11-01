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
 *	i4b_sframe.c - s frame handling routines
 *	----------------------------------------
 *
 *	$Id: i4b_sframe.c,v 1.8 2005/12/11 12:25:06 christos Exp $
 *
 * $FreeBSD$
 *
 *      last edit-date: [Fri Jan  5 11:33:47 2001]
 *
 *---------------------------------------------------------------------------*/

#include "i4bq921.h"

#define	NI4BQ921

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
#include <netisdn/i4b_l3l4.h>

/*---------------------------------------------------------------------------*
 *	process s frame
 *---------------------------------------------------------------------------*/
void
i4b_rxd_s_frame(struct isdn_l2 *l2, struct isdn_l3 *l3, struct mbuf *m)
{
	u_char *ptr = m->m_data;

	if(!((l2->tei_valid == TEI_VALID) &&
	     (l2->tei == GETTEI(*(ptr+OFF_TEI))))) {
		i4b_Dfreembuf(m);
		return;
	}

	l2->rxd_CR = GETCR(*(ptr + OFF_SAPI));
	l2->rxd_PF = GETSPF(*(ptr + OFF_SNR));
	l2->rxd_NR = GETSNR(*(ptr + OFF_SNR));

	i4b_rxd_ack(l2, l3, l2->rxd_NR);

	switch(*(ptr + OFF_SRCR)) {
	case RR:
		l2->stat.rx_rr++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd RR, N(R) = %d", l2->rxd_NR);
		i4b_next_l2state(l2, l3, EV_RXRR);
		break;
	case RNR:
		l2->stat.rx_rnr++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd RNR, N(R) = %d", l2->rxd_NR);
		i4b_next_l2state(l2, l3, EV_RXRNR);
		break;
	case REJ:
		l2->stat.rx_rej++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd REJ, N(R) = %d", l2->rxd_NR);
		i4b_next_l2state(l2, l3, EV_RXREJ);
		break;
	default:
		l2->stat.err_rx_bads++; /* update statistics */
		NDBGL2(L2_S_ERR, "ERROR, unknown code, frame = ");
		i4b_print_frame(m->m_len, m->m_data);
		break;
	}
	i4b_Dfreembuf(m);
}

/*---------------------------------------------------------------------------*
 *	transmit RR command
 *---------------------------------------------------------------------------*/
void
i4b_tx_rr_command(struct isdn_l2 *l2, pbit_t pbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RR, isdnif = %d", l2->l3->isdnif);

	m = i4b_build_s_frame(l2, CR_CMD_TO_NT, pbit, RR);

	l2->driver->ph_data_req(l2->l1_token, m, MBUF_FREE);

	l2->stat.tx_rr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RR response
 *---------------------------------------------------------------------------*/
void
i4b_tx_rr_response(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RR, isdnif = %d", l2->l3->isdnif);

	m = i4b_build_s_frame(l2, CR_RSP_TO_NT, fbit, RR);

	l2->driver->ph_data_req(l2->l1_token, m, MBUF_FREE);

	l2->stat.tx_rr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RNR command
 *---------------------------------------------------------------------------*/
void
i4b_tx_rnr_command(struct isdn_l2 *l2, pbit_t pbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RNR, isdnif = %d", l2->l3->isdnif);

	m = i4b_build_s_frame(l2, CR_CMD_TO_NT, pbit, RNR);

	l2->driver->ph_data_req(l2->l1_token, m, MBUF_FREE);

	l2->stat.tx_rnr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RNR response
 *---------------------------------------------------------------------------*/
void
i4b_tx_rnr_response(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RNR, isdnif = %d", l2->l3->isdnif);

	m = i4b_build_s_frame(l2, CR_RSP_TO_NT, fbit, RNR);

	l2->driver->ph_data_req(l2->l1_token, m, MBUF_FREE);

	l2->stat.tx_rnr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit REJ response
 *---------------------------------------------------------------------------*/
void
i4b_tx_rej_response(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx REJ, isdnif = %d", l2->l3->isdnif);

	m = i4b_build_s_frame(l2, CR_RSP_TO_NT, fbit, REJ);

	l2->driver->ph_data_req(l2->l1_token, m, MBUF_FREE);

	l2->stat.tx_rej++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	build S-frame for sending
 *---------------------------------------------------------------------------*/
struct mbuf *
i4b_build_s_frame(struct isdn_l2 *l2, crbit_to_nt_t crbit, 
	pbit_t pbit, u_char type)
{
	struct mbuf *m;

	if((m = i4b_Dgetmbuf(S_FRAME_LEN)) == NULL)
		return (NULL);

	PUTSAPI(SAPI_CCP, crbit, m->m_data[OFF_SAPI]);

	PUTTEI(l2->tei, m->m_data[OFF_TEI]);

	m->m_data[OFF_SRCR] = type;

	m->m_data[OFF_SNR] = (l2->vr << 1) | (pbit & 0x01);

	return (m);
}

#endif /* NI4BQ921 */
