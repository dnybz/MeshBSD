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

/*
 * XXX ...
 */


/*---------------------------------------------------------------------------*
 *	stop all layer 3 timers
 *---------------------------------------------------------------------------*/
void 
isdn_l3_timer_stop(struct isdn_bc *bc)
{
	isdn_T303_stop(bc);
	isdn_T305_stop(bc);
	isdn_T308_stop(bc);
	isdn_T309_stop(bc);
	isdn_T310_stop(bc);
	isdn_T313_stop(bc);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T303 timeout function
 *---------------------------------------------------------------------------*/
static void
isdn_T303_timeout(struct isdn_bc *bc)
{
	NDBGL3(L3_T_ERR, "SETUP not answered, cr = %d", bc->bc_cr);
	isdn_l3_next_state(bc, EV_T303EXP);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T303 start
 *---------------------------------------------------------------------------*/
void
isdn_T303_start(struct isdn_bc *bc)
{
	if (bc->bc_T303 == TIMER_ACTIVE)
		return;

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
	bc->bc_T303 = TIMER_ACTIVE;

	START_TIMER(bc->bc_T303_callout, isdn_T303_timeout, bc, T303VAL);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T303 stop
 *---------------------------------------------------------------------------*/
void
isdn_T303_stop(struct isdn_bc *bc)
{
	if (bc->bc_T303 != TIMER_IDLE) {
		STOP_TIMER(bc->bc_T303_callout, isdn_T303_timeout, bc);
		bc->bc_T303 = TIMER_IDLE;
	}
	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T305 timeout function
 *---------------------------------------------------------------------------*/
static void
isdn_T305_timeout(struct isdn_bc *bc)
{
	NDBGL3(L3_T_ERR, "DISC not answered, cr = %d", bc->bc_cr);
	isdn_l3_next_state(bc, EV_T305EXP);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T305 start
 *---------------------------------------------------------------------------*/
void
isdn_T305_start(struct isdn_bc *bc)
{
	if (bc->bc_T305 == TIMER_ACTIVE)
		return;

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
	bc->bc_T305 = TIMER_ACTIVE;

	START_TIMER(bc->bc_T305_callout, isdn_T305_timeout, bc, T305VAL);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T305 stop
 *---------------------------------------------------------------------------*/
void
isdn_T305_stop(struct isdn_bc *bc)
{
	if (bc->bc_T305 != TIMER_IDLE)
	{
		STOP_TIMER(bc->bc_T305_callout, isdn_T305_timeout, bc);
		bc->bc_T305 = TIMER_IDLE;
	}

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T308 timeout function
 *---------------------------------------------------------------------------*/
static void
isdn_T308_timeout(struct isdn_bc *bc)
{
	NDBGL3(L3_T_ERR, "REL not answered, cr = %d", bc->bc_cr);
	isdn_l3_next_state(bc, EV_T308EXP);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T308 start
 *---------------------------------------------------------------------------*/
void
isdn_T308_start(struct isdn_bc *bc)
{
	if (bc->bc_T308 == TIMER_ACTIVE)
		return;

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
	bc->bc_T308 = TIMER_ACTIVE;

	START_TIMER(bc->bc_T308_callout, isdn_T308_timeout, bc, T308VAL);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T308 stop
 *---------------------------------------------------------------------------*/
void
isdn_T308_stop(struct isdn_bc *bc)
{
	if (bc->bc_T308 != TIMER_IDLE) {
		STOP_TIMER(bc->bc_T308_callout, isdn_T308_timeout, bc);
		bc->bc_T308 = TIMER_IDLE;
	}

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T309 timeout function
 *---------------------------------------------------------------------------*/
static void
isdn_T309_timeout(struct isdn_bc *bc)
{
	NDBGL3(L3_T_ERR, "datalink not reconnected, cr = %d", bc->bc_cr);
	isdn_l3_next_state(bc, EV_T309EXP);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T309 start
 *---------------------------------------------------------------------------*/
void
isdn_T309_start(struct isdn_bc *bc)
{
	if (bc->bc_T309 == TIMER_ACTIVE)
		return;

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
	bc->bc_T309 = TIMER_ACTIVE;

	START_TIMER(bc->bc_T309_callout, isdn_T309_timeout, bc, T309VAL);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T309 stop
 *---------------------------------------------------------------------------*/
void
isdn_T309_stop(struct isdn_bc *bc)
{
	if (bc->bc_T309 != TIMER_IDLE) {
		STOP_TIMER(bc->bc_T309_callout, isdn_T309_timeout, bc);
		bc->bc_T309 = TIMER_IDLE;
	}

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T310 timeout function
 *---------------------------------------------------------------------------*/
static void
isdn_T310_timeout(struct isdn_bc *bc)
{
	NDBGL3(L3_T_ERR, "CALL PROC timeout, cr = %d", bc->bc_cr);
	isdn_l3_next_state(bc, EV_T310EXP);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T310 start
 *---------------------------------------------------------------------------*/
void
isdn_T310_start(struct isdn_bc *bc)
{
	if (bc->bc_T310 == TIMER_ACTIVE)
		return;

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
	bc->bc_T310 = TIMER_ACTIVE;

	START_TIMER(bc->bc_T310_callout, isdn_T310_timeout, bc, T310VAL);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T310 stop
 *---------------------------------------------------------------------------*/
void
isdn_T310_stop(struct isdn_bc *bc)
{
	if (bc->bc_T310 != TIMER_IDLE) {
		STOP_TIMER(bc->bc_T310_callout, isdn_T310_timeout, bc);
		bc->bc_T310 = TIMER_IDLE;
	}

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T313 timeout function
 *---------------------------------------------------------------------------*/
static void
isdn_T313_timeout(struct isdn_bc *bc)
{
	NDBGL3(L3_T_ERR, "CONN ACK not received, cr = %d", bc->bc_cr);
	isdn_l3_next_state(bc, EV_T313EXP);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T313 start
 *---------------------------------------------------------------------------*/
void
isdn_T313_start(struct isdn_bc *bc)
{
	if (bc->bc_T313 == TIMER_ACTIVE)
		return;

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
	bc->bc_T313 = TIMER_ACTIVE;

	START_TIMER(bc->bc_T313_callout, isdn_T313_timeout, bc, T313VAL);
}

/*---------------------------------------------------------------------------*
 *	timer isdn_T313 stop
 *---------------------------------------------------------------------------*/
void
isdn_T313_stop(struct isdn_bc *bc)
{
	if (bc->bc_T313 != TIMER_IDLE) {
		bc->bc_T313 = TIMER_IDLE;
		STOP_TIMER(bc->bc_T313_callout, isdn_T313_timeout, bc);
	}

	NDBGL3(L3_T_MSG, "cr = %d", bc->bc_cr);
}


