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

#include "opt_i4b.h"
#include "opt_i4b_debug.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <sys/domain.h>
#include <sys/proc.h>
#include <sys/protosw.h>
#include <sys/queue.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/route.h>

static struct pr_usrreqs nousrreqs;

#include <netisdn/i4b.h>

FEATURE(isdn, "ISDN over Ethernet");

extern void	i4b_init(void);

/*
 * ISDN protocol family.
 */
 
struct protosw isdnsw[] = {
{ 
	.pr_type =		0,			
	.pr_domain =	&isdndomain,		
	.pr_init =		i4b_init,
	.pr_usrreqs	=	&nousrreqs,
},
{ 
	.pr_type =		SOCK_RAW,			
	.pr_domain =	&isdndomain,		
	.pr_init =		i4b_init,
	.pr_usrreqs	=	&nousrreqs,
},
{ /* control socket */
	.pr_type =		SOCK_DGRAM,		
	.pr_domain =	&isdndomain,	
	.pr_flags =		PR_ATOMIC|PR_ADDR,	
  	.pr_usrreqs = 	&i4b_raw_usrreqs,
},
{ /* raw wildcard */
	.pr_type =		SOCK_RAW,		
	.pr_domain =	&isdndomain,	
	.pr_flags =		PR_ATOMIC|PR_ADDR,
  	.pr_usrreqs = 	&i4b_raw_usrreqs,
},
};

extern void *	i4b_domifattach(struct ifnet *);
extern void	i4b_domifdetach(struct ifnet *, void *);

/*
 * Defines ISDN domain.
 */

struct domain isdndomain = {
	.dom_family = 		AF_ISDN, 
	.dom_name = 		"isdn", 
	.dom_protosw =		isdnsw,
	
	.dom_protoswNPROTOSW =	
		&isdnsw[sizeof(isdnsw)/sizeof(isdnsw[0])],
	
	.dom_ifattach =		i4b_domifattach,
	.dom_ifdetach = 	i4b_domifdetach
};
DOMAIN_SET(isdn);

