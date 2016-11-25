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
/*-
 * Copyright (c) 1996-1999 Whistle Communications, Inc.
 * All rights reserved.
 * 
 * Subject to the following obligations and disclaimer of warranty, use and
 * redistribution of this software, in source or object code forms, with or
 * without modifications are expressly permitted by Whistle Communications;
 * provided, however, that:
 * 1. Any and all reproductions of the source or object code must include the
 *    copyright notice above and the following disclaimer of warranties; and
 * 2. No rights are granted, in any manner or form, to use Whistle
 *    Communications, Inc. trademarks, including the mark "WHISTLE
 *    COMMUNICATIONS" on advertising, endorsements, or otherwise except as
 *    such appears in the above copyright notice or in the software.
 * 
 * THIS SOFTWARE IS BEING PROVIDED BY WHISTLE COMMUNICATIONS "AS IS", AND
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, WHISTLE COMMUNICATIONS MAKES NO
 * REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, REGARDING THIS SOFTWARE,
 * INCLUDING WITHOUT LIMITATION, ANY AND ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 * WHISTLE COMMUNICATIONS DOES NOT WARRANT, GUARANTEE, OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF, OR THE RESULTS OF THE USE OF THIS
 * SOFTWARE IN TERMS OF ITS CORRECTNESS, ACCURACY, RELIABILITY OR OTHERWISE.
 * IN NO EVENT SHALL WHISTLE COMMUNICATIONS BE LIABLE FOR ANY DAMAGES
 * RESULTING FROM OR ARISING OUT OF ANY USE OF THIS SOFTWARE, INCLUDING
 * WITHOUT LIMITATION, ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * PUNITIVE, OR CONSEQUENTIAL DAMAGES, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES, LOSS OF USE, DATA OR PROFITS, HOWEVER CAUSED AND UNDER ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF WHISTLE COMMUNICATIONS IS ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Julian Elischer <julian@freebsd.org>
 *
 * $FreeBSD: head/sys/netgraph/ng_frame_relay.c 298813 2016-04-29 21:25:05Z pfg $
 * $Whistle: ng_frame_relay.c,v 1.20 1999/11/01 09:24:51 julian Exp $
 */
#include "opt_inet.h" 

#include "opt_isdn.h"
#include "opt_isdn_debug.h"
 
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/lock.h>
#include <sys/rwlock.h>
#include <sys/socket.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/if_types.h>
#include <net/netisr.h>

#include <netisdn/isdn.h>
#include <netisdn/isdn_var.h>

/*
 * XXX ...
 */

static  int 	isdn_dlci_len(char *);
static void 	isdn_rxd_ack(struct isdn_softc *, int);
static void 	isdn_input(struct mbuf *);

/*
 * XXX ...
 */

extern struct protosw isdnsw[];

/* 
 * Used to do FRMR headers 
 */
const struct isdn_frmr_seg makeup[] = {
	{ 0xfc, 2, 6 },
	{ 0xf0, 4, 4 },
	{ 0xfe, 1, 7 },
	{ 0xfc, 2, 6 }
};


/*
 * Default mtx(9) on global scope.
 */
struct mtx isdn_mtx;

MTX_SYSINIT(isdn_mtx, &isdn_mtx, "isdn_lock");

/*
 * Set containing ISDN channel. 
 */
struct isdn_head isdn_ifaddrhead;
struct rwlock isdn_ifaddr_lock;

RW_SYSINIT(isdn_ifadddr_lock, &isdn_ifaddr_lock, "isdn_ifaddr_lock");

/*
 * ISDN input queue is managed by netisr(9).
 */
static struct netisr_handler isdn_nh = {
	.nh_name 		= "isdn layer",
	.nh_handler 		= isdn_input,
	.nh_proto 		= NETISR_ISDN,
	.nh_policy 		= NETISR_POLICY_FLOW,
};

/*
 * ISDN initialisation.
 */
void
isdn_init(void)
{
	ISDN_IFADDR_LOCK_INIT();
	TAILQ_INIT(&isdn_ifaddrhead);
	netisr_register(&isdn_nh);
}

/*
 * Input processing. 
 */
 
static void
isdn_input(struct mbuf *m)
{	
	struct ifnet *ifp;
	struct isdn_ifaddr *ii;
	
	char *ptr;
	
	int dlci_len;
	int dlci;

	int nr;
	int ns;
	int p;
	
	M_ASSERTPKTHDR(m);
/*
 * XXX: Well, I've decided to encapsulate int by using FMRL semantics.
 */	
	if (m->m_pkthdr.len < ISDN_FRMRLEN) 
		goto out;

	if (m->m_len < ISDN_FRMRLEN) {
		if ((m = m_pullup(m, ISDN_FRMRLEN)) == NULL)
			return;
	}
	
	if ((ifp = m->m_pkthdr.rcvif) == NULL) 
		goto out;
			
	ptr = mtod(m, char *);
	
	dlci_len = isdn_dlci_len(ptr);

	switch (dlci_len) {
	case 2:
		SHIFTIN(makeup + 0, ptr[0], dlci);
		SHIFTIN(makeup + 1, ptr[1], dlci);
		break;
	case 3:
		SHIFTIN(makeup + 0, ptr[0], dlci);
		SHIFTIN(makeup + 1, ptr[1], dlci);
		SHIFTIN(makeup + 3, ptr[2], dlci);	/* 3 and 2 is correct */
		break;
	case 4:
		SHIFTIN(makeup + 0, ptr[0], dlci);
		SHIFTIN(makeup + 1, ptr[1], dlci);
		SHIFTIN(makeup + 2, ptr[2], dlci);
		SHIFTIN(makeup + 3, ptr[3], dlci);
		break;
	default:
		goto out;
	}

/*
 * XXX ... 
 */		


	if (dcli != ISDN_D_CHAN) {
/*
 * XXX: Well, any ethernet port aggregates n B-chans.
 */		
		goto out;
	} 
/*
 * XXX Be patient, I'll reimplement this stuff completely ...
 */		

	ISDN_IDADDR_RLOCK();
	
	TAILQ_FOREACH(isdn, &isdn_ifaddrhead, isdn_link) {
	if ((ii->ii_ifp == ifp) && 
		(ii->ii_tei.sllc.rd_tei == rd.rd_tei)) {
			break;
		}
	}
	if (ii == NULL) {
		ISDN_IDADDR_RUNLOCK();
		goto out;
	}
	ISDN_IDADDR_RUNLOCK();


	m_adj(m, ISDN_FRMRLEN);
	
	ptr = mtod(m, uint8_t *);
	
#ifdef ISDN_DEBUG
	(void)printf("%s: on=%s \n", __func__, ifp->if_xname);
#endif /* ISDN_DEBUG */

/*
 * LAPD Frame received. 
 */	
	if ((*(ptr + OFF_CNTL) & 0x01) == 0) {	
/*
 *	Process I frame, "I COMMAND" Q.921 03/93 pp 68 and pp 77
 */
		if (m->m_len < 4) {		
/* 
 * 6 oct - 2 chksum oct 
 */		
			ii->ii_stat.err_rx_len++;
			NDBGL2(L2_ERROR, 
				"%s: I-frame < 6 octetts!", __func__);
			goto out;
		}

		if (!((ii->ii_tei_valid == TEI_VALID) &&
		     (ii->ii_tei == GETTEI(*(ptr+OFF_TEI))))) {
			goto out;
		}

		if ((ii->ii_Q921_state != ST_MULTIFR) && 
			(ii->ii_Q921_state != ST_TIMREC)) {
			NDBGL2(L2_I_ERR, 
				"%s: state != (MF || TR)!", __func__);
			goto out;
		}
		nr = GETINR(*(ptr + OFF_INR));
		ns = GETINS(*(ptr + OFF_INS));
		p = GETIP(*(ptr + OFF_INR));
/* 
 * update frame count 
 */
		ii->ii_stat.rx_i++;
/* 
 * last packet ack 
 */
		isdn_rxd_ack(ii, nr);		
/* 
 * own receiver busy ? 
 */
		if (ii->ii_own_busy) {
			m_freem(m);	/* yes, discard information */
/* 
 * P bit == 1 ? 
 */
			if (p == 1)	{
/*
 * Tx RNR response.
 */				
				(void)isdn_tx_s_frame(ii, 
					CR_RSP_TO_NT, p, RNR);		

				ii->ii_ack_pend = 0;	/* clear ACK pending */
			}
		} else {
/* 
 * own receiver ready, if expected sequence number ?  
 */	
			if (ns == ii->ii_vr)	{
				M128INC(ii->ii_vr);	/* yes, update */

				ii->ii_rej_excpt = 0;	/* clr reject exception */

				m_adj(m, I_HDR_LEN);	/* strip i frame header */

				ii->ii_iframe_sent = 0;	/* reset i ack'd already */
/* 
 * pass data up 
 */
				isdn_q931_decode(ii, m);
			
				if (!ii->ii_iframe_sent) {
/*
 * Tx RR response.
 */				
					(void)isdn_tx_s_frame(ii, 
						CR_RSP_TO_NT, p, RR);
				
					ii->ii_ack_pend = 0;	/* clr ACK pending */
				}	
			} else {
/* 
 * ERROR, sequence number NOT expected 
 */		
				m_freem(m);	/* discard information */
/* 
 * already exception ? 
 */
				if (ii->ii_rej_excpt == 1) {
/* 
 * immediate resp ? 
 */
					if (p == 1)	{
/*
 * Tx RR response.
 */				
						(void)isdn_tx_s_frame(ii, 
							CR_RSP_TO_NT, p, RR);
						
						ii->ii_ack_pend = 0; /* clr ack pend */
					}
				} else {
/* 
 * not in exception cond 
 */			
					ii->ii_rej_excpt = 1;	/* set exception */
				
/*
 * Tx REJ response.
 */				
					(void)isdn_tx_s_frame(ii, 
						CR_RSP_TO_NT, p, REJ);			
				
					ii->ii_ack_pend = 0;	/* clr ack pending */
				}
			}
		}
/* 
 * sequence number ranges as expected ? 
 */
		if (isdn_l2_nr_ok(nr, ii->ii_va, ii->ii_vs)) {
			if (ii->ii_Q921_state == ST_TIMREC) {
				ii->ii_va = nr;
			} else {
/* 
 * yes, other side busy ? 
 */
				if (ii->ii_peer_busy) {
					ii->ii_va = nr;	/* yes, update ack count */
				} else {
/* 
 * other side ready 
 */		
					if (nr == ii->ii_vs) {
/* 
 * count expected ? 
 */								
						ii->ii_va = nr;	/* update ack */
						isdn_T200_stop(ii);
						isdn_T203_restart(ii);
					} else {
						if (nr != ii->ii_va) {
							ii->ii_va = nr;
							isdn_T200_restart(ii);
						}
					}
				}
			}
		} else {
/* 
 * sequence error 
 */		
			isdn_l2_nr_error_recovery(ii);	
			
			ii->ii_Q921_state = ST_AW_EST;
		}
			
	} else if ((*(ptr + OFF_CNTL) & 0x03) == 0x01 ) {
/*
 * Process S frame.
 */	
		if (m->m_len < 4) {	
/* 
 * 6 oct - 2 chksum oct 
 */	
			ii->ii_stat.err_rx_len++;
			NDBGL2(L2_ERROR, 
				"%s: S-frame < 6 octetts!", __func__);
			goto out;
		}
	
		if (!((ii->ii_tei_valid == TEI_VALID) &&
		     (ii->ii_tei == GETTEI(*(ptr+OFF_TEI))))) {
			goto out;
		}

		ii->ii_rxd_CR = GETCR(*(ptr + OFF_SAPI));
		ii->ii_rxd_PF = GETSPF(*(ptr + OFF_SNR));
		ii->ii_rxd_NR = GETSNR(*(ptr + OFF_SNR));

		isdn_rxd_ack(ii, ii->ii_rxd_NR);

		switch(*(ptr + OFF_SRCR)) {
		case RR:
			ii->ii_stat.rx_rr++; /* update statistics */
			NDBGL2(L2_S_MSG, 
				"rx'd RR, N(R) = %d", ii->ii_rxd_NR);
			isdn_l2_next_state(ii, EV_RXRR);
		break;
		case RNR:
			ii->ii_stat.rx_rnr++; /* update statistics */
			NDBGL2(L2_S_MSG, 
				"rx'd RNR, N(R) = %d", ii->ii_rxd_NR);
			isdn_l2_next_state(ii, EV_RXRNR);
			break;
		case REJ:
			ii->ii_stat.rx_rej++; /* update statistics */
			NDBGL2(L2_S_MSG, 
				"rx'd REJ, N(R) = %d", ii->ii_rxd_NR);
			isdn_l2_next_state(ii, EV_RXREJ);
			break;
		default:
			ii->ii_stat.err_rx_bads++; /* update statistics */
			NDBGL2(L2_S_ERR, 
				"%s: unknown code, frame = ");
			isdn_l2_print_frame(m->m_len, mtod(m, uint8_t *));
			break;
		}	
		
	} else if ((*(ptr + OFF_CNTL) & 0x03) == 0x03 ) {
/*
 * Process a received U frame.
 */
		if (m->m_len < 3) {
/* 
 * 5 oct - 2 chksum oct 
 */
			ii->ii_stat.err_rx_len++;
			NDBGL2(L2_ERROR,
				"%s: U-frame < 5 octetts!", __func__);
			goto out;
		}
		sapi = GETSAPI(*(ptr + OFF_SAPI));
		tei = GETTEI(*(ptr + OFF_TEI));
		p = GETUPF(*(ptr + OFF_CNTL));
	
		switch (*(ptr + OFF_CNTL) & ~UPFBIT) {
/* 
 * commands 
 */
		case SABME:
			if ((ii->ii_tei_valid == TEI_VALID) && 
				(ii->ii_tei == GETTEI(*(ptr+OFF_TEI)))) {
				ii->ii_stat.rx_sabme++;
				NDBGL2(L2_U_MSG, 
					"SABME, sapi = %d, tei = %d", sapi, tei);
				ii->ii_rxd_PF = p;
				isdn_l2_next_state(ii, EV_RXSABME);
			}
			break;
		case UI:
			if ((sapi == SAPI_L2M) && 
				(tei == GROUP_TEI) &&
				   (*(ptr + OFF_MEI) == MEI)) {
/* 
 * layer 2 management (SAPI = 63) 
 */
				ii->ii_stat.rx_tei++;
				isdn_tei_rxd(ii, m);
				return;
			} else if ((sapi == SAPI_CCP) && 
				(tei == GROUP_TEI)) {
/* 
 * call control (SAPI = 0) 
 */
				ii->ii_stat.rx_ui++;
/* 
 * strip ui header 
 */
				m_adj(m, UI_HDR_LEN);
/* 
 * to upper layer 
 */	
				isdn_q931_decode(ii, m);
				return;
			} else {
				ii->ii_stat.err_rx_badui++;
				NDBGL2(L2_U_ERR, "unknown UI frame!");
			}
			break;
		case DISC:
			if ((ii->ii_tei_valid == TEI_VALID) && 
				(ii->ii_tei == GETTEI(*(ptr+OFF_TEI)))) {
				ii->ii_stat.rx_disc++;
				NDBGL2(L2_U_MSG, 
					"DISC, sapi = %d, tei = %d", sapi, tei);
				ii->ii_rxd_PF = p;
				isdn_l2_next_state(ii, EV_RXDISC);
			}
			break;
		case XID:
		
			if ((ii->ii_tei_valid == TEI_VALID) && 
				(ii->ii_tei == GETTEI(*(ptr+OFF_TEI)))) {
				ii->ii_stat.rx_xid++;
				NDBGL2(L2_U_MSG, 
					"XID, sapi = %d, tei = %d", sapi, tei);
			}
			break;
/* 
 * responses 
 */
		case DM:
			if ((ii->ii_tei_valid == TEI_VALID) && 
				(ii->ii_tei == GETTEI(*(ptr+OFF_TEI)))) {
					ii->ii_stat.rx_dm++;
				NDBGL2(L2_U_MSG, 
					"DM, sapi = %d, tei = %d", sapi, tei);
				isdn_l2_print_frame(m->m_len, mtod(m, uint8_t *));
				ii->ii_rxd_PF = p;
				isdn_l2_next_state(ii, EV_RXDM);
			}
			break;
		case UA:
			if ((ii->ii_tei_valid == TEI_VALID) && 
				(ii->ii_tei == GETTEI(*(ptr+OFF_TEI)))) {
				ii->ii_stat.rx_ua++;
				NDBGL2(L2_U_MSG, 
					"UA, sapi = %d, tei = %d", sapi, tei);
				ii->ii_rxd_PF = p;
				isdn_l2_next_state(ii, EV_RXUA);
			}
			break;
		case FRMR:
			if ((ii->ii_tei_valid == TEI_VALID) && 
				(ii->ii_tei == GETTEI(*(ptr+OFF_TEI)))) {
				ii->ii_stat.rx_frmr++;
				NDBGL2(L2_U_MSG, 
					"FRMR, sapi = %d, tei = %d", sapi, tei);
				ii->ii_rxd_PF = p;
				isdn_l2_next_state(ii, EV_RXFRMR);
			}
			break;
		default:
			if ((ii->ii_tei_valid == TEI_VALID) && 
				(ii->ii_tei == GETTEI(*(ptr+OFF_TEI)))) {
				NDBGL2(L2_U_ERR, 
					"UNKNOWN TYPE ERROR, sapi = %d, "
					"tei = %d, frame = ", sapi, tei);
			} else {
				NDBGL2(L2_U_ERR, 
					"not mine -  UNKNOWN TYPE ERROR, "
					"sapi = %d, tei = %d, frame = ", sapi, tei);
			}
			isdn_l2_print_frame(m->m_len, mtod(m, uint8_t *));
			
			ii->ii_stat.err_rx_badui++;
			break;
		}
		
	} else {
		ii->ii_stat.err_rx_badf++;
		NDBGL2(L2_ERROR, 
				"%s: bad frame rx'd - ", __func__);
				
		isdn_l2_print_frame(m->m_len, mtod(m, uint8_t *));
	}
out:	
	m_freem(m);	
}

/*
 * Got s or i frame, check if valid ack for last sent frame.
 */
static void
isdn_rxd_ack(struct isdn_softc *sc, int nr)
{

	NDBGL2(L2_ERROR, "N(R)=%d, UA=%d, V(R)=%d, V(S)=%d, V(A)=%d",
		nr,
		ii->ii_ua_num,
		ii->ii_vr,
		ii->ii_vs,
		ii->ii_va);

	if (ii->ii_ua_num != UA_EMPTY) {

		M128DEC(nr);
/* 
 * Discard cached frame.
 */
		if (ii->ii_ua_num != nr) {
			NDBGL2(L2_ERROR, 
				"%s: ((N(R)-1)=%d) != (UA=%d) !!!", 
				__func__, nr, ii->ii_ua_num);
		}
		m_freem(ii->ii_ua_frame);
		ii->ii_ua_num = UA_EMPTY;
	}
}


/*
 * Count up the size of the address header if we don't already know.
 */
static int 	
isdn_dlci_len(char *hdr)
{
	int len;
	
	if (hdr[0] & BYTEX_EA)
		len = 0;
	else if (hdr[1] & BYTEX_EA)
		len = 2;
	else if (hdr[2] & BYTEX_EA)
		len = 3;
	else if (hdr[3] & BYTEX_EA)
		len = 4;
	else
		len = 0;
	
	return (len);
}
