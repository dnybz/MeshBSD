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
/*-
 * Copyright (c) 1982, 1986, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (C) 2001 WIDE Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)in.c	8.4 (Berkeley) 1/9/95
 */

#include "opt_inet.h" 

#include "opt_isdn.h"
#include "opt_isdn_debug.h"
 
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/systm.h>
#include <sys/sockio.h>
#include <sys/malloc.h>
#include <sys/priv.h>
#include <sys/socket.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/sysctl.h>
#include <sys/syslog.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_llatbl.h>
#include <net/if_types.h>
#include <net/if_var.h>

#include <netisdn/isdn.h>

int 	isdn_control(struct socket *, u_long, caddr_t, 
	struct ifnet *, struct thread *);


/*
 * Generic isdn control operations.
 *
 */ 
 
int
isdn_control(struct socket *so __unused, u_long cmd, caddr_t data, 
		struct ifnet *ifp, struct thread *td)
{
	
/*
 * XXX: please be patient... 
 */

#ifdef ISDN_DEBUG
	(void)printf("%s\n", __func__);
#endif /* ISDN_DEBUG */

	if (ifp == NULL) {
		error = EADDRNOTAVAIL;
		goto out;
	}
	
	if ((ifp->if_ioctl == NULL) 
		|| (ifp->if_addr == NULL)) { 
		error = ENXIO;
		goto out;
	}
/*
 * XXX: please be patient... 
 */

out:
	return (EOPNOTSUPP);	
}

/*
 * Operations on ISDN ARP cache.
 */

void *	isdn_domifattach(struct ifnet *);
void	isdn_domifdetach(struct ifnet *, void *);

/*
 * Morphism on IEEE802.{3,11} link-layer.
 */
struct isdn_llentry {
	struct llentry		il_base;
/*	
 * XXX: noot yet necessary...
 * 
	struct sockaddr_e167 	il_tel; 
 */
};

#define	ISDN_LLTBL_DEFAULT_HSIZE	32
#define	ISDN_LLTBL_HASH(chan, cr, sapi, tei, h) \
	(((((((chan) ^ cr)) ^ sapi)) ^ tei) & ((h) - 1))

/*
 * Called by LLE_FREE_LOCKED when number of references
 * drops to zero.
 */
static void
isdn_lltable_destroy_lle(struct llentry *lle)
{

	LLE_WUNLOCK(lle);
	isdn_lltable_destroy_lle_unlocked(lle);
}

 
static struct llentry *
isdn_lltable_new(struct isdn_rd *rd, u_int flags)
{
	struct isdn_llentry *lle;

	lle = malloc(sizeof(struct isdn_llentry), M_LLTABLE, M_NOWAIT | M_ZERO);
	if (lle == NULL)		/* NB: caller generates msg */
		return (NULL);

	lle->il_base.la_expire = time_uptime; /* mark expired */
	lle->il_base.lle_refcnt = 1;
	lle->il_base.lle_free = isdn_lltable_destroy_lle;
	
	LLE_LOCK_INIT(&lle->il_base);
	LLE_REQ_INIT(&lle->il_base);
	callout_init(&lle->il_base.lle_timer, 1);

	return (&lle->il_base);
}

#define ISDN_ARE_MASKED_RD_EQUAL(d, a, m)	(		\
	(((d ^ a) & m) == 0 )

static int
isdn_lltable_match_prefix(const struct sockaddr *saddr,
    const struct sockaddr *smask, u_int flags, struct llentry *lle)
{
	struct isdn_rd addr, mask, lle_addr;

	addr = ((const struct sockaddr_isdn *)saddr)->sisdn_rd;
	mask = ((const struct sockaddr_isdn *)smask)->sisdn_rd;
	
	lle_addr = lle->r_l3addr.rd;

	if (ISDN_ARE_MASKED_RD_EQUAL(lle_addr, addr, mask) == 0)
		return (0);

	if (lle->la_flags & LLE_IFADDR) {
/*
 * Delete LLE_IFADDR records IFF address & flag matches.
 * Note that addr is the interface address within prefix
 * being matched.
 * Note also we should handle 'ifdown' cases without removing
 * ifaddr macs.
 */
		if (addr == lle_addr && (flags & LLE_STATIC) != 0)
			return (1);
		return (0);
	}
/* 
 * flags & LLE_STATIC means deleting both dynamic and static entries 
 */
	if ((flags & LLE_STATIC) || !(lle->la_flags & LLE_STATIC))
		return (1);

	return (0);
}

static void
isdn_lltable_free_entry(struct lltable *llt, struct llentry *lle)
{
	struct ifnet *ifp;
	size_t pkts_dropped;

	LLE_WLOCK_ASSERT(lle);
	KASSERT(llt != NULL, ("lltable is NULL"));
/* 
 * Unlink entry from table if not already 
 */
	if ((lle->la_flags & LLE_LINKED) != 0) {
		ifp = llt->llt_ifp;
		IF_AFDATA_WLOCK_ASSERT(ifp);
		lltable_unlink_entry(llt, lle);
	}
/* 
 * cancel timer 
 */
	if (callout_stop(&lle->lle_timer) > 0)
		LLE_REMREF(lle);
/* 
 * Drop hold queue 
 */
	pkts_dropped = llentry_free(lle);
	ARPSTAT_ADD(dropped, pkts_dropped);
}
 
static int
isdn_lltable_rtcheck(struct ifnet *ifp, u_int flags, 
	const struct sockaddr *l3addr)
{
/*
 * XXX: not yet... btw. an routing 
 * XXX: table based on E.167 addr
 * XXX: is planned, but before I'll
 * XXX: do that... I'll refactor
 * XXX: i4b(4) completely. 
 *
	struct rt_addrinfo info;
	struct sockaddr_in rt_key, rt_mask;
	struct sockaddr rt_gateway;
	int rt_flags;

	KASSERT(l3addr->sa_family == AF_E167,
	    ("sisdn_family %d", l3addr->sa_family));

	bzero(&rt_key, sizeof(rt_key));
	rt_key.sin_len = sizeof(rt_key);
	bzero(&rt_mask, sizeof(rt_mask));
	rt_mask.sin_len = sizeof(rt_mask);
	bzero(&rt_gateway, sizeof(rt_gateway));
	rt_gateway.sa_len = sizeof(rt_gateway);

	bzero(&info, sizeof(info));
	info.rti_info[RTAX_DST] = (struct sockaddr *)&rt_key;
	info.rti_info[RTAX_NETMASK] = (struct sockaddr *)&rt_mask;
	info.rti_info[RTAX_GATEWAY] = (struct sockaddr *)&rt_gateway;

	if (rib_lookup_info(ifp->if_fib, l3addr, NHR_REF, 0, &info) != 0)
		return (EINVAL);

	rt_flags = info.rti_flags;
 *
 * If the gateway for an existing host route matches the target L3
 * address, which is a special route inserted by some implementation
 * such as MANET, and the interface is of the correct type, then
 * allow for ARP to proceed.
 *
	if (rt_flags & RTF_GATEWAY) {
		if (!(rt_flags & RTF_HOST) || !info.rti_ifp ||
		    info.rti_ifp->if_type != IFT_ETHER ||
		    (info.rti_ifp->if_flags & (IFF_NOARP | IFF_STATICARP)) != 0 ||
		    (void)memcmp(rt_gateway.sa_data, l3addr->sa_data,
		    sizeof(in_addr_t)) != 0) {
			rib_free_info(&info);
			return (EINVAL);
		}
	}
	rib_free_info(&info);

 *
 * Make sure that at least the destination address is covered
 * by the route. This is for handling the case where 2 or more
 * interfaces have the same prefix. An incoming packet arrives
 * another interface.
 *
	if (!(rt_flags & RTF_HOST) && info.rti_ifp != ifp) {
		const char *sa, *mask, *addr, *lim;
		int len;

		mask = (const char *)&rt_mask;
 *
 * Just being extra cautious to avoid some custom
 * code getting into trouble.
 *
		if ((info.rti_addrs & RTA_NETMASK) == 0)
			return (EINVAL);

		sa = (const char *)&rt_key;
		addr = (const char *)l3addr;
		len = ((const struct sockaddr_in *)l3addr)->s167_len;
		lim = addr + len;

		for ( ; addr < lim; sa++, mask++, addr++) {
			if ((*sa ^ *addr) & *mask) {
#ifdef DIAGNOSTIC
				log(LOG_INFO, "E.167 address: ", ...);
#endif
				return (EINVAL);
			}
		}
	}
 */
	return (0);
}
 
 
static inline uint32_t
isdn_lltable_hash_rd(const struct isdn_rd rd, uint32_t hsize)
{

	return (ISDN_LLTBL_HASH(rd.rd_chan, rd.rd_cr,
		rd.rd_sapi, rd.rd_tei, hsize));
}

static uint32_t
isdn_lltable_hash(const struct llentry *lle, uint32_t hsize)
{

	return (isdn_lltable_hash_rd(lle->r_l3addr.rd, hsize));
}

static void
isdn_lltable_fill_sa_entry(const struct llentry *lle, struct sockaddr *sa)
{
	struct sockaddr_isdn *sisdn;

	sisdn = (struct sockaddr_isdn *)sa;
	bzero(sisdn, sizeof(*sisdn));
	sisdn->sisdn_family = AF_ISDN;
	sisdn->sisdn_len = sizeof(*sisdn);
	sisdn->sisdn_rd = lle->r_l3addr.rd;
}

static inline struct llentry *
isdn_lltable_find_rd(struct lltable *llt, struct isdn_rd rd)
{
	struct llentry *lle;
	struct llentries *lleh;
	u_int hashidx;

	hashidx = isdn_lltable_hash_rd(rd, llt->llt_hsize);
	lleh = &llt->lle_head[hashidx];
	LIST_FOREACH(lle, lleh, lle_next) {
		if (lle->la_flags & LLE_DELETED)
			continue;
		if (lle->r_l3addr.rd == rd)
			break;
	}

	return (lle);
}

static void
isdn_lltable_delete_entry(struct lltable *llt, struct llentry *lle)
{

	lle->la_flags |= LLE_DELETED;
	EVENTHANDLER_INVOKE(lle_event, lle, LLENTRY_DELETED);
#ifdef DIAGNOSTIC
	log(LOG_INFO, "ifaddr cache = %p is deleted\n", lle);
#endif
	llentry_free(lle);
}

static struct llentry *
isdn_lltable_alloc(struct lltable *llt, u_int flags, 
 	const struct sockaddr *l3addr)
{
	const struct sockaddr_isdn *sisdn = 
		(const struct sockaddr_isdn *)l3addr;
	
	struct ifnet *ifp = llt->llt_ifp;
	struct llentry *lle;
	char linkhdr[LLE_MAX_LINKHDR];
	size_t linkhdrsize;
	int lladdr_off;

	KASSERT(l3addr->sa_family == AF_ISDN,
	    ("sisdn_family %d", l3addr->sa_family));

/*
 * XXX: RADIX Trie is not yet necessary ...
 *
 * A route that covers the given address must have
 * been installed 1st because we are doing a resolution,
 * verify this.
 */
	if (!(flags & LLE_IFADDR) &&
	    isdn_lltable_rtcheck(ifp, flags, l3addr) != 0)
		return (NULL);

	lle = isdn_lltable_new(sin->sin_addr, flags);
	if (lle == NULL) {
		log(LOG_INFO, "lla_lookup: new lle malloc failed\n");
		return (NULL);
	}
	lle->la_flags = flags;
	if (flags & LLE_STATIC)
		lle->r_flags |= RLLE_VALID;
	if ((flags & LLE_IFADDR) == LLE_IFADDR) {
		linkhdrsize = LLE_MAX_LINKHDR;
		if (lltable_calc_llheader(ifp, AF_INET, IF_LLADDR(ifp),
		    linkhdr, &linkhdrsize, &lladdr_off) != 0) {
			isdn_lltable_destroy_lle_unlocked(lle);
			return (NULL);
		}
		lltable_set_entry_addr(ifp, lle, linkhdr, linkhdrsize,
		    lladdr_off);
		lle->la_flags |= LLE_STATIC;
		lle->r_flags |= (RLLE_VALID | RLLE_IFADDR);
	}

	return (lle);
}


/*
 * Return NULL if not found or marked for deletion.
 * If found return lle read locked.
 */
static struct llentry *
isdn_lltable_lookup(struct lltable *llt, u_int flags, 
	const struct sockaddr *l3addr)
{
	const struct sockaddr_isdn *sisdn = (const struct sockaddr_in *)l3addr;
	struct llentry *lle;

	IF_AFDATA_LOCK_ASSERT(llt->llt_ifp);
	KASSERT(l3addr->sa_family == AF_ISDN,
	    ("sisdn_family %d", l3addr->sa_family));
	lle = isdn_lltable_find_rd(llt, sisdn->sisdn_rd);

	if (lle == NULL)
		goto out;

	KASSERT((flags & (LLE_UNLOCKED|LLE_EXCLUSIVE)) !=
	    (LLE_UNLOCKED|LLE_EXCLUSIVE),
	    ("wrong lle request flags: 0x%X",
	    flags));

	if (flags & LLE_UNLOCKED)
		goto out;

	if (flags & LLE_EXCLUSIVE)
		LLE_WLOCK(lle);
	else
		LLE_RLOCK(lle);
out:
	return (lle);
}

struct isdn_arpc {
	struct rt_msghdr	arpc_rtm;
	struct sockaddr_isdn	arpc_sisdn;
	struct sockaddr_dl	arpc_sdl;
};

static int
isdn_lltable_dump_entry(struct lltable *llt, struct llentry *lle,
    struct sysctl_req *wr)
{
	struct ifnet *ifp = llt->llt_ifp;
/* 
 * XXX stack use 
 */
	struct isdn_arpc arpc;
	struct sockaddr_dl *sdl;
	int error = 0;

	bzero(&arpc, sizeof(arpc));
/* 
 * skip deleted entries 
 */
	if ((lle->la_flags & LLE_DELETED) == LLE_DELETED)
		goto out;
/* 
 * XXX: Skip if jailed. 
 */
	lltable_fill_sa_entry(lle,(struct sockaddr *)&arpc.arpc_sisdn);
	if (prison_if(wr->td->td_ucred, 
		(struct sockaddr *)&arpc.arpc_sisdn) != 0)
		goto out;
/*
 * produce a msg made of:
 *  struct rt_msghdr;
 *  struct sockaddr_isdn; (RD)
 *  struct sockaddr_dl;
 */
	arpc.arpc_rtm.rtm_msglen = sizeof(arpc);
	arpc.arpc_rtm.rtm_version = RTM_VERSION;
	arpc.arpc_rtm.rtm_type = RTM_GET;
	arpc.arpc_rtm.rtm_flags = RTF_UP;
	arpc.arpc_rtm.rtm_addrs = RTA_DST | RTA_GATEWAY;
/* 
 * publish 
 */
	if (lle->la_flags & LLE_PUB)
		arpc.arpc_rtm.rtm_flags |= RTF_ANNOUNCE;

	sdl = &arpc.arpc_sdl;
	sdl->sdl_family = AF_LINK;
	sdl->sdl_len = sizeof(*sdl);
	sdl->sdl_index = ifp->if_index;
	sdl->sdl_type = ifp->if_type;
	
	if ((lle->la_flags & LLE_VALID) == LLE_VALID) {
		sdl->sdl_alen = ifp->if_addrlen;
		bcopy(lle->ll_addr, LLADDR(sdl), ifp->if_addrlen);
	} else {
		sdl->sdl_alen = 0;
		bzero(LLADDR(sdl), ifp->if_addrlen);
	}
	arpc.arpc_rtm.rtm_rmx.rmx_expire =
	lle->la_flags & LLE_STATIC ? 0 : lle->la_expire;
	arpc.arpc_rtm.rtm_flags |= (RTF_HOST | RTF_LLDATA);
	
	if (lle->la_flags & LLE_STATIC)
		arpc.arpc_rtm.rtm_flags |= RTF_STATIC;
	
	if (lle->la_flags & LLE_IFADDR)
		arpc.arpc_rtm.rtm_flags |= RTF_PINNED;
	
	arpc.arpc_rtm.rtm_index = ifp->if_index;
	error = SYSCTL_OUT(wr, &arpc, sizeof(arpc));
out:
	return (error);
}

static struct lltable *
isdn_lltattach(struct ifnet *ifp)
{
	struct lltable *llt;

	llt = lltable_allocate_htbl(ISDN_LLTBL_DEFAULT_HSIZE);
 	llt->llt_af = AF_ISDN;
 	llt->llt_ifp = ifp;

	llt->llt_lookup = isdn_lltable_lookup;
	llt->llt_alloc_entry = isdn_lltable_alloc;
	llt->llt_delete_entry = isdn_lltable_delete_entry;
	llt->llt_dump_entry = isdn_lltable_dump_entry;
	llt->llt_hash = isdn_lltable_hash;
	llt->llt_fill_sa_entry = isdn_lltable_fill_sa_entry;
	llt->llt_free_entry = isdn_lltable_free_entry;
	llt->llt_match_prefix = isdn_lltable_match_prefix;
 	lltable_link(llt);

	return (llt);
}
 
/*
 * Bind ARP cache, see kern/uiopc_domain.c 
 * and net/if.c for further details.
 */
void *
isdn_domifattach(struct ifnet *ifp)
{
	struct isdn_ifinfo *iii;

	iii = malloc(sizeof(*iii), M_IFADDR, M_WAITOK|M_ZERO);
	iii->iii_llt = isdn_lltattach(ifp);
	iii->iii_sc = malloc(sizeof(*iii->iii_sc), M_IFADDR, M_WAITOK|M_ZERO);
/*
 * XXX: ...
 */
	TAILQ_INIT(&iii->iii_sc.sc_hd);
/*
 * XXX: ...
 */	
	return (iii);
}

/*
 * Detach cache.
 */
void
isdn_domifdetach(struct ifnet *ifp, void *aux)
{
	struct isdn_ifinfo *iii = (struct isdn_ifinfo *)aux;
	
	free(iii->iii_sc, M_IFADDR);
	lltable_free(iii->iii_llt);
	free(iii, M_IFADDR);
} 


