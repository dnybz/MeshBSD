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
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/lock.h>
#include <sys/rwlock.h>
#include <sys/socket.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/if_types.h>
#include <net/if_var.h>

#include <netisdn/i4b.h>
#include <netisdn/i4b_var.h>

/*
 * XXX ...
 */

int
isdn_output(struct isdn_softc *sc, struct mbuf *m, uint8_t chan, 
	uint8_t proto, uint8_t sapi, uint8_t tei)
{	
	struct sockaddr_isdn sisdn;
	struct isdn_rd *rd;
	int error;

	bzero(&sisdn, sizeof(sisdn));
	
	sisdn.sisdn_type = AF_ISDN;
	sisdn.sisdn_len = sizeof(sisdn);
	sisdn.sisdn_rd.rd_chan = chan;
	sisdn.sisdn_rd.rd_proto = proto;
	sisdn.sisdn_rd.rd_sapi = sapi;
	sisdn.sisdn_rd.rd_tei = tei;

	M_PREPEND(m, sizeof(*rd), (M_ZERO|M_NOWAIT));
	if (m == NULL) {
		error = ENOBUFS;
		goto out;
	}		
	rd = mtod(m, struct isdn_rd *);
	rd->rd_chan = sisdn.sisdn_rd.rd_chan;
	rd->rd_proto = sisdn.sisdn_rd.rd_proto;
	rd->rd_sapi = sisdn.sisdn_rd.rd_sapi; 	
	rd->rd_tei = sisdn.sisdn_rd.rd_tei;	 
/*
 * XXX ...
 */
	error = (*ifp->if_output)
		(ifp, m, (const struct sockaddr *)&sisdn, NULL);
out:	
	return (error);
}


