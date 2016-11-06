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

#include <netisdn/i4b.h>
#include <netisdn/i4b_var.h>

/*
 * XXX ...
 */

static void i4b_input(struct mbuf *);

/*
 * XXX ...
 */

extern struct protosw i4bsw[];

/*
 * Default mtx(9) on global scope.
 */
struct mtx i4b_mtx;

MTX_SYSINIT(i4b_mtx, &i4b_mtx, "i4b_lock");

/*
 * Set containing ISDN channel. 
 */
struct i4b_head i4b_ifaddrhead;
struct rwlock i4b_ifaddr_lock;

RW_SYSINIT(i4b_ifadddr_lock, &i4b_ifaddr_lock, "i4b_ifaddr_lock");

/*
 * ISDN input queue is managed by netisr(9).
 */
static struct netisr_handler i4b_nh = {
	.nh_name 		= "isdn layer",
	.nh_handler 		= i4b_input,
	.nh_proto 		= NETISR_ISDN,
	.nh_policy 		= NETISR_POLICY_FLOW,
};

/*
 * ISDN initialisation.
 */
void
i4b_init(void)
{
	TAILQ_INIT(&i4b_ifaddrhead);
	netisr_register(&i4b_nh);
}

/*
 * Input processing. 
 */
 
static void
i4b_input(struct mbuf *m)
{	
	struct ifnet *ifp;
	struct isdn_sc *sc;
	struct isdn_rd *rd;
	
	uint8_t *ptr;
	
	uint8_t chan;
	uint8_t proto;

	M_ASSERTPKTHDR(m);
		
	if (m->m_pkthdr.len < ISDN_HDRLEN) 
		goto bad;

	if (m->m_len < MPLS_ISDN) {
		if ((m = m_pullup(m, MPLS_HDRLEN)) == NULL)
			goto out;
	}
	
	if ((ifp = m->m_pkthdr.rcvif) == NULL)
		goto bad;
	
	if ((ifp->if_flags & IFF_ISDN) == 0)
		goto bad;

	IF_AFDATA_RLOCK(ifp);
	sc = ISDN_SOFTC(ifp);
	IF_AFDATA_RUNLOCK(ifp);	

	rd = mtod(m, struct isdn_rd *);	
	
	chan = rd->rd_chan;
	proto = rd->rd_proto;
	
	m_adj(m, ISDN_HDRLEN);
	
	frame = m->m_data;
	
#ifdef I4B_DEBUG
	(void)printf("%s: on=%s \n", __func__, ifp->if_xname);
#endif /* I4B_DEBUG */

	switch (chan) {
	case ISDN_D_CHAN:
/*
 * LAPD Frame received. 
 */	
		if ((*(ptr + OFF_CNTL) & 0x01) == 0) {
/* 
 * 6 oct - 2 chksum oct 
 */
			if (m->m_len < 4) {
				l2->stat.err_rx_len++;
				NDBGL2(L2_ERROR, "ERROR, I-frame < 6 octetts!");
				goto bad;
			}
			i4b_rxd_i_frame(sc, m);
		} else if ((*(ptr + OFF_CNTL) & 0x03) == 0x01 ) {
/* 
 * 6 oct - 2 chksum oct 
 */
			if (m->m_len < 4) {
				l2->stat.err_rx_len++;
				NDBGL2(L2_ERROR, "ERROR, S-frame < 6 octetts!");
				goto bad;
			}
			i4b_rxd_s_frame(sc, m);
		} else if ((*(ptr + OFF_CNTL) & 0x03) == 0x03 ) {
/* 
 * 5 oct - 2 chksum oct 
 */
			if (m->m_len < 3) {
				l2->stat.err_rx_len++;
				NDBGL2(L2_ERROR, "ERROR, U-frame < 5 octetts!");
				goto bad;
			}
			i4b_rxd_u_frame(sc, m);
		} else {
			l2->stat.err_rx_badf++;
			NDBGL2(L2_ERROR, "ERROR, bad frame rx'd - ");
			i4b_print_frame(m->m_len, m->m_data);
			goto bad;
		}
		break;
	case ISDN_B1_CHAN: 
	case ISDN_B2_CHAN:
			
			/* FALLTHROUGH */
/*
 * XXX: Well, any ethernet port aggregates two B-chans.
 */	
		break;
	default:
		NDBGL2(L2_ERROR, "ERROR, unknown frame rx'd - ");
		i4b_print_frame(m->m_len, m->m_data);
		goto bad;
	}	
out:	
	return;	
bad:
	m_freem(m);	
}


