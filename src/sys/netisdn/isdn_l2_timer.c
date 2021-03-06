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

#include <netisdn/isdn_dl.h>
#include <netisdn/isdn_mbuf.h>
#include <netisdn/isdn_dl_fsm.h>

/*
 * XXX; MP sync. prmitives are not yet implemented ...
 */


static void 	isdn_T200_timeout(struct isdn_ifaddr *);
static void 	isdn_T202_timeout(struct isdn_ifaddr *);
static void 	isdn_T203_timeout(struct isdn_ifaddr *);

/*---------------------------------------------------------------------------*
 *	Q.921 timer T200 timeout function
 *---------------------------------------------------------------------------*/
static void
isdn_T200_timeout(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_T_ERR, "isdnif %d, RC = %d", 
		ii->ii_ifp->if_index, ii->ii_RC);
	
	isdn_dl_next_state(sc, EV_T200EXP);
}

/*---------------------------------------------------------------------------*	
 *	Q.921 timer T202 timeout function
 *---------------------------------------------------------------------------*/
static void
isdn_T202_timeout(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_T_ERR, "isdnif %d, N202 = %d", 
		ii->ii_ifp->if_index, ii->ii_N202);

	if (--(ii->ii_N202))
		(*ii->ii_T202_fn)(sc);
}

/*---------------------------------------------------------------------------*
 *	Q.921 timer T203 timeout function
 *---------------------------------------------------------------------------*/
static void
isdn_T203_timeout(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_T_ERR, "isdnif %d", ii->ii_ifp->if_index);
	
	isdn_dl_next_state(sc, EV_T203EXP);
}

/*---------------------------------------------------------------------------*
 *	Q.921 timer T200 start
 *---------------------------------------------------------------------------*/
void
isdn_T200_start(struct isdn_ifaddr *ii)
{
	if (ii->ii_T200 == TIMER_ACTIVE)
		return;

	NDBGL2(L2_T_MSG, "isdnif %d", ii->ii_ifp->if_index);
	ii->ii_T200 = TIMER_ACTIVE;

	START_TIMER(ii->ii_T200_callout, 
		isdn_T200_timeout, sc, T200DEF);
}

/*---------------------------------------------------------------------------*
 *	Q.921 timer T200 stop
 *---------------------------------------------------------------------------*/
void
isdn_T200_stop(struct isdn_ifaddr *ii)
{
	if (ii->ii_T200 != TIMER_IDLE) {
		STOP_TIMER(ii->ii_T200_callout, 
			isdn_T200_timeout, sc);
		ii->ii_T200 = TIMER_IDLE;
	}
	
	NDBGL2(L2_T_MSG, "isdnif %d", ii->ii_ifp->if_index);
}

/*---------------------------------------------------------------------------*
 *	Q.921 timer T200 restart
 *---------------------------------------------------------------------------*/
void
isdn_T200_restart(struct isdn_ifaddr *ii)
{
	
	if (ii->ii_T200 != TIMER_IDLE) {
		STOP_TIMER(ii->ii_T200_callout, 
			isdn_T200_timeout, sc);
	} else 
		ii->ii_T200 = TIMER_ACTIVE;
	
	START_TIMER(ii->ii_T200_callout, 
		isdn_T200_timeout, sc, T200DEF);
	
	NDBGL2(L2_T_MSG, "isdnif %d", ii->ii_ifp->if_index);
}


/*---------------------------------------------------------------------------*
 *	Q.921 timer T202 start
 *---------------------------------------------------------------------------*/
void
isdn_T202_start(struct isdn_ifaddr *ii)
{
	if (ii->ii_N202 == TIMER_ACTIVE)
		return;

	NDBGL2(L2_T_MSG, "isdnif %d", ii->ii_ifp->if_index);
	
	ii->ii_N202 = N202DEF;
	ii->ii_T202 = TIMER_ACTIVE;

	START_TIMER(ii->ii_T202_callout, 
		isdn_T202_timeout, sc, T202DEF);
}

/*---------------------------------------------------------------------------*
 *	Q.921 timer T202 stop
 *---------------------------------------------------------------------------*/
void
isdn_T202_stop(struct isdn_ifaddr *ii)
{
	SC_WLOCK(sc);
	
	if (ii->ii_T202 != TIMER_IDLE) {
		STOP_TIMER(ii->ii_T202_callout, 
			isdn_T202_timeout, sc);
		ii->ii_T202 = TIMER_IDLE;
	}
	SC_WUNLOCK(sc);
	
	NDBGL2(L2_T_MSG, "isdnif %d", ii->ii_ifp->if_index);
}

/*---------------------------------------------------------------------------*
 *	Q.921 timer T203 start
 *---------------------------------------------------------------------------*/
void
isdn_T203_start(struct isdn_ifaddr *ii)
{
	if (ii->ii_T203 == TIMER_ACTIVE)
		return;

	NDBGL2(L2_T_MSG, "isdnif %d", ii->ii_ifp->if_index);
	ii->ii_T203 = TIMER_ACTIVE;

	START_TIMER(ii->ii_T203_callout, 
		isdn_T203_timeout, sc, T203DEF);
}

/*---------------------------------------------------------------------------*
 *	Q.921 timer T203 stop
 *---------------------------------------------------------------------------*/
void
isdn_T203_stop(struct isdn_ifaddr *ii)
{
	
	if (ii->ii_T203 != TIMER_IDLE) {
		STOP_TIMER(ii->ii_T203_callout, 
			isdn_T203_timeout, sc);
		ii->ii_T203 = TIMER_IDLE;
	}
	
	NDBGL2(L2_T_MSG, "isdnif %d", ii->ii_ifp->if_index);
}

/*---------------------------------------------------------------------------*
 *	Q.921 timer T203 restart
 *---------------------------------------------------------------------------*/
void
isdn_T203_restart(struct isdn_ifaddr *ii)
{
	if (ii->ii_T203 != TIMER_IDLE) {
		STOP_TIMER(ii->ii_T203_callout, 
			isdn_T203_timerout, sc);
	} else 
		ii->ii_T203 = TIMER_ACTIVE;

	START_TIMER(ii->ii_T203_callout, 
		isdn_T203_timerout, sc, T203DEF);

	NDBGL2(L2_T_MSG, "isdnif %d", ii->ii_ifp->if_index);
}
