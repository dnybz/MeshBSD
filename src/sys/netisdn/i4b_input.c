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

static void i4b_input(struct mbuf *m)

extern struct protosw i4bsw[];

/*
 * Default mtx(9) on global scope.
 */
struct mtx i4b_mtx;

MTX_SYSINIT(i4b_mtx, &i4b_mtx, "i4b_lock");

/*
 * Defines set containing nhlfe. 
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

	if ((ifp = m->m_pkthdr.rcvif) == NULL)
		goto bad;
	
	if ((ifp->if_flags & IFF_ISDN) == 0)
		goto bad;
/*
 * XXX; be patient... well I'll map some callback fn here...
 */

#ifdef I4B_DEBUG
	(void)printf("%s: on=%s \n", __func__, ifp->if_xname);
#endif /* I4B_DEBUG */

	return;	
bad:
	m_freem(m);	
}

