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

#ifndef _NETISDN_ISDN_VAR_H_
#define _NETISDN_ISDN_VAR_H_

#ifdef _KERNEL

#include <sys/queue.h>

/*
 * Software context.
 */
struct isdn_softc {
	struct ifnet 	*sc_ifp; 	
	struct isdn_l2 	sc_l2;
	struct isdn_l3 	sc_l3;
	struct rwlock 	sc_lock;
};

#define SC_LOCK_INIT(sc, d, t) \
	rw_init_flags(&(sc)->sc_lock, (t), RW_RECURSE |  RW_DUPOK)
#define SC_LOCK_DESTROY(sc) 	rw_destroy(&(sc)->sc_lock)
#define SC_RLOCK(sc) 		rw_rlock(&(sc)->sc_lock)
#define SC_WLOCK(sc) 		rw_wlock(&(sc)->sc_lock)
#define SC_TRY_RLOCK(sc) 	rw_try_rlock(&(sc)->sc_lock)
#define SC_TRY_WLOCK(sc) 	rw_try_wlock(&(sc)->sc_lock)
#define SC_RUNLOCK(sc) 		rw_runlock(&(sc)->sc_lock)
#define SC_WUNLOCK(sc) 		rw_wunlock(&(sc)->sc_lock)
#define	SC_TRY_UPGRADE(sc) 	rw_try_upgrade(&(sc)->sc_lock)
#define	SC_DOWNGRADE(sc) 	rw_downgrade(&(sc)->sc_lock)
#define	SC_WLOCKED(sc) 		rw_wowned(&(sc)->sc_lock)
#define	SC_LOCK_ASSERT(sc) 	rw_assert(&(sc)->sc_lock, RA_LOCKED)
#define	SC_RLOCK_ASSERT(sc) 	rw_assert(&(sc)->sc_lock, RA_RLOCKED)
#define	SC_WLOCK_ASSERT(sc) 	rw_assert(&(sc)->sc_lock, RA_WLOCKED)
#define	SC_UNLOCK_ASSERT(sc) 	rw_assert(&(sc)->sc_lock, RA_UNLOCKED)

/*
 * Denotes AF_ISDN partition on domain family..
 */ 
struct isdn_ifinfo {
	struct lltable		*iii_llt;	/* isdn_arp cache */
	struct isdn_softc 	*iii_sc;
};
#define ISDN_LLTABLE(ifp)	\
	(((struct isdn_ifinfo *)(ifp)->if_afdata[AF_ISDN])->iii_llt)
#define ISDN_SOFTC(ifp) \
	(((struct isdn_ifinfo *)(ifp)->if_afdata[AF_ISDN])->iii_sc)

/*
 * Describes an ISDN channel on IEEE802.{3,11} link-layer.
 */
struct isdn_ifaddr {
	struct ifaddr 	ii_ifa;		/* protocol-independent info */
#define ii_addr 	ii_ifa.ifa_addr
#define ii_netmask 	ii_ifa.ifa_netmask
#define ii_dstaddr 	ii_ifa.ifa_dstaddr
#define ii_ifp 	ii_ifa.ifa_ifp	
#define ii_flags 	ii_ifa.ifa_flags
#define ii_metric 	ii_ifa.ifa_metric
	TAILQ_ENTRY(isdn_ifaddr)	ii_link;
/* 
 * < channel, proto ,sapi, tei >  
 */	
	struct sockaddr_isdn 	ii_seg;	
/* 
 * < telno, subaddr > 
 */
	struct sockaddr_e167 	ii_no; 	 
};

#endif /* _KERNEL */
#endif /* _NETISDN_ISDN_VAR_H_ */
