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
 *	isdn_sframe.c - s frame handling routines
 *	----------------------------------------
 *
 *	$Id: isdn_sframe.c,v 1.8 2005/12/11 12:25:06 christos Exp $
 *
 * $FreeBSD$
 *
 *      last edit-date: [Fri Jan  5 11:33:47 2001]
 *
 *---------------------------------------------------------------------------*/
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
#include "opt_isdnq921.h"

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/isdn_debug.h>
#include <netisdn/isdn_ioctl.h>

#include <netisdn/isdn_l2.h>
#include <netisdn/isdn_l1l2.h>
#include <netisdn/isdn_isdnq931.h>
#include <netisdn/isdn_mbuf.h>
#include <netisdn/isdn_l2fsm.h>
#include <netisdn/isdn_l3l4.h>

/*---------------------------------------------------------------------------*
 *	process s frame
 *---------------------------------------------------------------------------*/
void
isdn_rxd_s_frame(struct isdn_sc *sc, struct mbuf *m)
{
	u_char *ptr;
	struct isdn_l2 *l2;
	
	ptr = m->m_data;
	l2 = &sc->sc_l2;

	if (!((l2->l2_tei_valid == TEI_VALID) &&
	     (l2->l2_tei == GETTEI(*(ptr+OFF_TEI))))) {
		goto out;
	}

	l2->l2_rxd_CR = GETCR(*(ptr + OFF_SAPI));
	l2->l2_rxd_PF = GETSPF(*(ptr + OFF_SNR));
	l2->l2_rxd_NR = GETSNR(*(ptr + OFF_SNR));

	isdn_rxd_ack(l2, l2->l2_rxd_NR);

	switch(*(ptr + OFF_SRCR)) {
	case RR:
		l2->l2_stat.rx_rr++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd RR, N(R) = %d", l2->l2_rxd_NR);
		isdn_next_l2state(l2, l3, EV_RXRR);
		break;
	case RNR:
		l2->l2_stat.rx_rnr++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd RNR, N(R) = %d", l2->l2_rxd_NR);
		isdn_next_l2state(l2, l3, EV_RXRNR);
		break;
	case REJ:
		l2->l2_stat.rx_rej++; /* update statistics */
		NDBGL2(L2_S_MSG, "rx'd REJ, N(R) = %d", l2->l2_rxd_NR);
		isdn_next_l2state(l2, l3, EV_RXREJ);
		break;
	default:
		l2->l2_stat.err_rx_bads++; /* update statistics */
		NDBGL2(L2_S_ERR, "ERROR, unknown code, frame = ");
		isdn_print_frame(m->m_len, m->m_data);
		break;
	}
out:	
	m_freem(m);
}

/*---------------------------------------------------------------------------*
 *	transmit RR cmd
 *---------------------------------------------------------------------------*/
void
isdn_tx_rr_cmd(struct isdn_l2 *l2, pbit_t pbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RR, isdnif = %d", l2->l2_l3->isdnif);

	m = isdn_build_s_frame(l2, CR_CMD_TO_NT, pbit, RR);

	isdn_output(l2, m, MBUF_FREE);

	l2->l2_stat.tx_rr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RR resp
 *---------------------------------------------------------------------------*/
void
isdn_tx_rr_resp(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RR, isdnif = %d", l2->l2_l3->isdnif);

	m = isdn_build_s_frame(l2, CR_RSP_TO_NT, fbit, RR);

	isdn_output(l2, m, MBUF_FREE);

	l2->l2_stat.tx_rr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RNR cmd
 *---------------------------------------------------------------------------*/
void
isdn_tx_rnr_cmd(struct isdn_l2 *l2, pbit_t pbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RNR, isdnif = %d", l2->l2_l3->isdnif);

	m = isdn_build_s_frame(l2, CR_CMD_TO_NT, pbit, RNR);

	isdn_output(l2, m, MBUF_FREE);

	l2->l2_stat.tx_rnr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit RNR resp
 *---------------------------------------------------------------------------*/
void
isdn_tx_rnr_resp(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx RNR, isdnif = %d", l2->l2_l3->isdnif);

	m = isdn_build_s_frame(l2, CR_RSP_TO_NT, fbit, RNR);

	isdn_output(l2, m, MBUF_FREE);

	l2->l2_stat.tx_rnr++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	transmit REJ resp
 *---------------------------------------------------------------------------*/
void
isdn_tx_rej_resp(struct isdn_l2 *l2, fbit_t fbit)
{
	struct mbuf *m;

	NDBGL2(L2_S_MSG, "tx REJ, isdnif = %d", l2->l2_l3->isdnif);

	m = isdn_build_s_frame(l2, CR_RSP_TO_NT, fbit, REJ);

	isdn_output(l2, m, MBUF_FREE);

	l2->l2_stat.tx_rej++; /* update statistics */
}

/*---------------------------------------------------------------------------*
 *	build S-frame for sending
 *---------------------------------------------------------------------------*/
struct mbuf *
isdn_build_s_frame(struct isdn_l2 *l2, crbit_to_nt_t crbit, 
	pbit_t pbit, u_char type)
{
	struct mbuf *m;

	if ((m = isdn_Dgetmbuf(S_FRAME_LEN, M_DONTWAIT, MT_I4B_D)) == NULL)
		goto out;

	PUTSAPI(SAPI_CCP, crbit, m->m_data[OFF_SAPI]);

	PUTTEI(l2->l2_tei, m->m_data[OFF_TEI]);

	m->m_data[OFF_SRCR] = type;

	m->m_data[OFF_SNR] = (l2->l2_vr << 1) | (pbit & 0x01);
out:
	return (m);
}
