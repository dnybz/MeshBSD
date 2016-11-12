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
 
#include "opt_inet.h"

#include "opt_isdn.h"
#include "opt_isdn_debug.h"

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/isdn.h>
#include <netisdn/isdn_var.h>

static void 	isdn_bc_init_callout(struct isdn_bc *);
static void 	isdn_bc_stop_callout(struct isdn_bc *);

/*
 * XXX: ... 
 */
 
struct isdn_bcq isdn_bcq;

/*
 * XXX: ... 
 */

struct isdn_bc * 	
isdn_bc_alloc(struct isdn_softc *sc)
{
	struct isdn_bc *bc;

	if ((bc = malloc(sizeof(*bc), M_IFADDR, M_NOWAIT|M_ZERO)) != NULL) { 
		TAILQ_INSERT_HEAD(&sc->sc_bcq, bc, bc_link);
		TAILQ_INSERT_HEAD(&isdn_bcq, bc, bc_chain);
		isdn_bc_init_callout(bc);
	}
	return (bc);
}


/*---------------------------------------------------------------------------*
 *      search calldescriptor
 *      ---------------------
 *      This routine searches for the calldescriptor for a passive controller
 *      given by unit number, callreference and callreference flag.
 *	It returns a pointer to the calldescriptor if found, else a NULL.
 *---------------------------------------------------------------------------*/
struct isdn_bc * 	
isdn_bc_by_cr(struct isdn_softc *sc, int cr, int crf)
{
	struct isdn_bc *bc;

	TAILQ_FOREACH(bc, &sc->sc_bcq, bc_link) {
		
		if ((bc->bc_cr == cr) && 
			(bc->bc_cr_flag == crf)) {
			break;
		}
	}

	if (bc != NULL) {
		NDBGL4(L4_MSG, "found b-cahnnel @ isdnif=%d id=%u cr=%d",
			sc->sc_ifp->if_index, bc->bc_id, bc->bc_cr);
		isdn_bc_init_callout(bc);
	}
	return (bc);
}

/*---------------------------------------------------------------------------*
 *	generate 7 bit "random" number used for outgoing Call Reference
 *---------------------------------------------------------------------------*/
uint8_t
isdn_bc_get_rand_cr(void)
{
	register int i;
	u_char val, retval;
	int called = 42;
	struct timeval t;

	val += ++called;

	for (i = 0; i < 50 ; i++, val++) {
		int found = 1;

		(void)read_random((char *)&val, sizeof(val));

		retval = val & 0x7f;

		if (retval == 0 || retval == 0x7f)
			continue;

		TAILQ_FOREACH(bc, &isdn_bcq, bc_chain) {
		
			if (bc->bc_cr == retval) {
				found = 0;
				break;
			}
		}

		if (found)
			return (retval);
	}
	return (0);	/* XXX */
}

/*
 * XXX: ... 
 */


static void
isdn_bc_stop_callout(struct isdn_bc *bc)
{
	if (bc->bc_callouts_inited) {
		callout_stop(&bc->bc_idle_timeout_handle);
		callout_stop(&bc->bc_T303_callout);
		callout_stop(&bc->bc_T305_callout);
		callout_stop(&bc->bc_T308_callout);
		callout_stop(&bc->bc_T309_callout);
		callout_stop(&bc->bc_T310_callout);
		callout_stop(&bc->bc_T313_callout);
		callout_stop(&bc->bc_T400_callout);
	}
}


void
isdn_bc_init_callout(struct isdn_bc *bc)
{
	if (bc->bc_callouts_inited == 0) {
		callout_init(&bc->bc_idle_timeout_handle, 0);
		callout_init(&bc->bc_T303_callout, 0);
		callout_init(&bc->bc_T305_callout, 0);
		callout_init(&bc->bc_T308_callout, 0);
		callout_init(&bc->bc_T309_callout, 0);
		callout_init(&bc->bc_T310_callout, 0);
		callout_init(&bc->bc_T313_callout, 0);
		callout_init(&bc->bc_T400_callout, 0);
		bc->bc_callouts_inited = 1;
	}
}

