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

	if ((ifp->if_flags & IFF_ISDN) == 0) {
		error = EADDRNOTAVAIL;
 		goto out;			
 	}

/*
 * XXX: please be patient... 
 */

out:
	return (EOPNOTSUPP);	
}

void *	isdn_domifattach(struct ifnet *);
void	isdn_domifdetach(struct ifnet *, void *);

/*
 * Bind ARP cache, see kern/uiopc_domain.c 
 * and net/if.c for further details.
 */
void *
isdn_domifattach(struct ifnet *ifp)
{
	struct isdn_ifinfo *iii = NULL;
	struct lltable *llt;

	if (ifp == NULL)
		goto out;

	iii = malloc(sizeof(*iii), M_IFADDR, M_WAITOK|M_ZERO);
	
	if ((llt = lltable_init(ifp, AF_ISDN)) != NULL) {

/*
 * XXX: common operations on ARP cache...
 *
		llt->llt_prefix_free = isdn_lltable_prefix_free;
		llt->llt_lookup = isdn_lltable_lookup;
		llt->llt_dump = isdn_lltable_dump;
 */		
		iii->iii_llt = llt;
	} else {
		free(iii, M_IFADDR);
		iii = NULL;
	}
out:
	return (iii);
}

/*
 * Detach cache.
 */
void
isdn_domifdetach(struct ifnet *ifp, void *aux)
{
	struct isdn_ifinfo *iii;
	struct lltable *llt;
	
	if ((iii = (struct isdn_ifinfo *)aux) == NULL)
		return;

	if (ifp == NULL)
		return;
		
	if ((llt = iii->iii_llt) == NULL)
		return;

	switch (ifp->if_type) {
	case IFT_ETHER:
	case IFT_LOOP:
		
		if (ifp->if_flags & IFF_ISDN) {
			ifp->if_flags &= ~IFF_ISDN;
		}
		break;
	default:
		break;
	}
	lltable_free(llt);
	free(iii, M_IFADDR);
} 


