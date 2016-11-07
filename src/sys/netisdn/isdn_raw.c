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

#include "opt_inet.h"
#include "opt_isdn.h"

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/errno.h>
#include <sys/protosw.h>
#include <sys/sockio.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/systm.h>
#include <sys/sysraw.h>

#include <net/if.h>
#include <net/if_types.h>

/*
 * Defines interface maps to socket 
 * layer for accessing ISDN layer by 
 * control plane.
 */

#include <netisdn/i4b.h>

extern int	i4b_control(struct socket *, u_long, caddr_t, struct ifnet *,
    struct thread *);
    
#define ISDN_RAW_SNDQ	576
#define ISDN_RAW_RCVQ	576

SYSCTL_DECL(_net_isdn);
SYSCTL_NODE(_net, AF_ISDN, isdn, CTLFLAG_RW, 0, "ISDN Family");

u_long i4b_raw_sendspace = ISDN_RAW_SNDQ;
u_long i4b_raw_recvspace = ISDN_RAW_RCVQ;

SYSCTL_ULONG(_net_isdn, OID_AUTO, sendspace, CTLFLAG_RW,
    &i4b_raw_sendspace, 0, "Maximum outgoing raw ISDN PDU size");
SYSCTL_ULONG(_net_isdn, OID_AUTO, recvspace, CTLFLAG_RW,
    &i4b_raw_recvspace, 0, "Maximum space for incoming ISDN PDU");

/*
 * Raw socket.
 */

static int
i4b_attach(struct socket *so, int proto, struct thread *td)
{
	
	return (soreserve(so, i4b_raw_sendspace, i4b_raw_recvspace));
}

struct pr_usrreqs i4b_raw_usrreqs = {
	.pru_attach =	i4b_raw_attach,
	.pru_control =	i4b_control,
};

/*
 * Control socket.
 * 
 
struct i4b_pcb {
	struct socket	 *pcb_so;
	LIST_ENTRY(i4b_pcb) socks;
	struct isdn_call_desc pcb_cd;
};

MTX_SYSINIT(i4b_pcb_mtx, &i4b_pcb_mtx, "i4b_pcb", MTX_DEF);

static LIST_HEAD(, i4b_pcb) i4b_pcb_list;

static struct mtx	i4b_pcb_mtx;

static int
i4b_attach(struct socket *so, int proto, struct thread *td)
{
	struct i4b_pcb *const pcbp = sotoi4b_pcb(so);
	int error;

	if (pcbp != NULL)
		return (EISCONN);
	
	error = soreserve(so, i4b_raw_sendspace, i4b_raw_recvspace);
	if (error)
		return (error);
	
	pcbp = malloc(sizeof(struct ngpcb), M_PCB, M_WAITOK | M_ZERO);
	pcbp->type = type;

	so->so_pcb = (caddr_t)pcbp;
	pcbp->pcb_so = so;

	mtx_lock(&i4b_pcb_mtx);
	LIST_INSERT_HEAD(&i4b_pcb_list, pcbp, socks);
	mtx_unlock(&i4b_pcb_mtx);
	
	return (error);
}

 *
 */
