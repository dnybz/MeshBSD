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
 *
 *---------------------------------------------------------------------------
 *
 *	i4b_l2fsm.c - layer 2 FSM
 *	-------------------------
 *
 *	$Id: i4b_l2fsm.c,v 1.13 2009/03/14 14:46:11 dsl Exp $
 *
 * $FreeBSD$
 *
 *      last edit-date: [Fri Jan  5 11:33:47 2001]
 *
 *---------------------------------------------------------------------------*/
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
#include "opt_isdn_debug.h"

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/i4b.h>

#include <netisdn/i4b_debug.h>
#include <netisdn/i4b_ioctl.h>

#include <netisdn/i4b_global.h>
#include <netisdn/i4b_l2.h>
#include <netisdn/i4b_l1l2.h>
#include <netisdn/i4b_isdnq931.h>
#include <netisdn/i4b_mbuf.h>

#include <netisdn/i4b_l2fsm.h>

#if I4B_DEBUG
static const char *l2state_text[N_STATES] = {
	"ST_TEI_UNAS",
	"ST_ASG_AW_TEI",
	"ST_EST_AW_TEI",
	"ST_TEI_ASGD",

	"ST_AW_EST",
	"ST_AW_REL",
	"ST_MULTIFR",
	"ST_TIMREC",

	"ST_SUBSET",
	"Illegal State"
};

static const char *l2event_text[N_EVENTS] = {
	"EV_DLESTRQ",
	"EV_DLUDTRQ",
	"EV_MDASGRQ",
	"EV_MDERRRS",
	"EV_PSDEACT",
	"EV_MDREMRQ",
	"EV_RXSABME",
	"EV_RXDISC",
	"EV_RXUA",
	"EV_RXDM",
	"EV_T200EXP",
	"EV_DLDATRQ",
	"EV_DLRELRQ",
	"EV_T203EXP",
	"EV_OWNBUSY",
	"EV_OWNRDY",
	"EV_RXRR",
	"EV_RXREJ",
	"EV_RXRNR",
	"EV_RXFRMR",
	"Illegal Event"
};
#endif

static void 	F_TU01(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TU03(struct isdn_l2 *, struct isdn_l3 *);

static void 	F_TA03(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TA04(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TA05(struct isdn_l2 *, struct isdn_l3 *);

static void 	F_TE03(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TE04(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TE05(struct isdn_l2 *, struct isdn_l3 *);

static void 	F_T01(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_T05(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_T06(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_T07(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_T08(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_T09(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_T10(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_T13(struct isdn_l2 *, struct isdn_l3 *);

static void 	F_AE01(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AE05(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AE06(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AE07(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AE08(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AE09(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AE10(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AE11(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AE12(struct isdn_l2 *, struct isdn_l3 *);

static void 	F_AR05(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AR06(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AR07(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AR08(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AR09(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AR10(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_AR11(struct isdn_l2 *, struct isdn_l3 *);

static void 	F_MF01(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF05(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF06(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF07(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF08(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF09(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF10(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF11(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF12(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF13(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF14(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF15(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF16(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF17(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF18(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF19(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_MF20(struct isdn_l2 *, struct isdn_l3 *);

static void 	F_TR01(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR05(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR06(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR07(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR08(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR09(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR10(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR11(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR12(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR13(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR15(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR16(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR17(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR18(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR19(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_TR20(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_ILL(struct isdn_l2 *, struct isdn_l3 *);
static void 	F_NCNA(struct isdn_l2 *, struct isdn_l3 *);

/*---------------------------------------------------------------------------*
 *	FSM illegal state default action
 *---------------------------------------------------------------------------*/
static void
F_ILL(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_ERR, "FSM function F_ILL executing");
}

/*---------------------------------------------------------------------------*
 *	FSM No change, No action
 *---------------------------------------------------------------------------*/
static void
F_NCNA(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_NCNA executing");
}

/*---------------------------------------------------------------------------*
 *	layer 2 state transition table
 *---------------------------------------------------------------------------*/
struct l2state_tab {
/* 
 * function to execute 
 */
	void (*func)(struct isdn_l2 *, struct isdn_l3 *);	
/* 
 * next state 
 */
	int newstate;
} l2state_tab[N_EVENTS][N_STATES] = {

/* STATE:	
	ST_TEI_UNAS,			
	ST_ASG_AW_TEI,			
	ST_EST_AW_TEI,			
	ST_TEI_ASGD,		
	ST_AW_EST,		
	ST_AW_REL,		
	ST_MULTIFR,		
	ST_TIMREC,		
	ST_SUBSET,		
	ILLEGAL STATE	*/

	{	
/*
 * EV_DLESTRQ
 */
		{F_TU01, ST_EST_AW_TEI},	
		{F_NCNA, ST_EST_AW_TEI},	
		{F_ILL,	ST_ILL},		
		{F_T01,	ST_AW_EST},     
		{F_AE01, ST_AW_EST},	
		{F_ILL,	ST_ILL},	
		{F_MF01, ST_AW_EST},	
		{F_TR01, ST_AW_EST},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_DLUDTRQ
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_MDASGRQ
 */
	{	
		{F_TU03, ST_TEI_ASGD},		
		{F_TA03, ST_TEI_ASGD},		
		{F_TE03, ST_AW_EST},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/* 
 * EV_MDERRRS
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_TA04, ST_TEI_UNAS},		
		{F_TE04, ST_TEI_UNAS},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*EV_PSDEACT*/
	{	
		{F_ILL,	ST_ILL},		
		{F_TA05, ST_TEI_UNAS},		
		{F_TE05, ST_TEI_UNAS},		
		{F_T05,	ST_TEI_ASGD},	
		{F_AE05, ST_TEI_ASGD},	
		{F_AR05, ST_TEI_ASGD},	
		{F_MF05, ST_TEI_ASGD},	
		{F_TR05, ST_TEI_ASGD},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_MDREMRQ
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_T06,	ST_TEI_UNAS},	
		{F_AE06, ST_TEI_UNAS},	
		{F_AR06, ST_TEI_UNAS},	
		{F_MF06, ST_TEI_UNAS},	
		{F_TR06, ST_TEI_UNAS},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_RXSABME
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_T07,	ST_SUBSET},	
		{F_AE07, ST_AW_EST},	
		{F_AR07, ST_AW_REL},	
		{F_MF07, ST_MULTIFR},	
		{F_TR07, ST_MULTIFR},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/* 
 * EV_RXDISC 
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_T08,	ST_TEI_ASGD},	
		{F_AE08, ST_AW_EST},	
		{F_AR08, ST_AW_REL},	
		{F_MF08, ST_TEI_ASGD},	
		{F_TR08, ST_TEI_ASGD},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/* 
 * EV_RXUA  
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_T09,	ST_TEI_ASGD},	
		{F_AE09, ST_SUBSET},	
		{F_AR09, ST_SUBSET},	
		{F_MF09, ST_MULTIFR},	
		{F_TR09, ST_TIMREC},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_RXDM  
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_T10,	ST_SUBSET},	
		{F_AE10, ST_SUBSET},	
		{F_AR10, ST_SUBSET},	
		{F_MF10, ST_SUBSET},	
		{F_TR10, ST_AW_EST},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/* 
 * EV_T200EXP
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_AE11, ST_SUBSET},	
		{F_AR11, ST_SUBSET},	
		{F_MF11, ST_TIMREC},	
		{F_TR11, ST_SUBSET},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/* 
 * EV_DLDATRQ
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_AE12, ST_AW_EST},	
		{F_ILL,	ST_ILL},	
		{F_MF12, ST_MULTIFR},	
		{F_TR12, ST_TIMREC},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_DLRELRQ
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_T13,	ST_TEI_ASGD},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_MF13, ST_AW_REL},	
		{F_TR13, ST_AW_REL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_T203EXP
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_MF14, ST_TIMREC},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/* 
 * EV_OWNBUSY
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_MF15, ST_MULTIFR},	
		{F_TR15, ST_TIMREC},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_OWNRDY 
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_MF16, ST_MULTIFR},	
		{F_TR16, ST_TIMREC},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_RXRR   
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_MF17, ST_SUBSET},	
		{F_TR17, ST_SUBSET},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/* 
 * EV_RXREJ  
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_MF18, ST_SUBSET},	
		{F_TR18, ST_SUBSET},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_RXRNR  
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_MF19, ST_SUBSET},	
		{F_TR19, ST_SUBSET},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/*
 * EV_RXFRMR 
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_MF20, ST_AW_EST},	
		{F_TR20, ST_AW_EST},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	},
/* 
 * EV_ILL    
 */
	{	
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},		
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL},	
		{F_ILL,	ST_ILL} 
	}
};

/*---------------------------------------------------------------------------*
 *	event handler, executes function and sets new state
 *---------------------------------------------------------------------------*/
void 
isdn_l2_next_state(struct isdn_l2 *l2, struct isdn_l3 *l3, int event)
{
	int currstate, newstate;
	int (*savpostfsmfunc)(struct isdn_l3 *) = NULL;

	/* check event number */
	if (event > N_EVENTS)
		panic("i4b_l2fsm.c: event > N_EVENTS");

	/* get current state and check it */
	if ((currstate = l2->Q921_state) > N_STATES) 	/* failsafe */
		panic("i4b_l2fsm.c: currstate > N_STATES");

	/* get new state and check it */
	if ((newstate = l2state_tab[event][currstate].newstate) > N_STATES)
		panic("i4b_l2fsm.c: newstate > N_STATES");


	if (newstate != ST_SUBSET) {	
/* 
 * state function does NOT set new state 
 */
		NDBGL2(L2_F_MSG, "FSM event [%s]: [%s/%d => %s/%d]", 
			l2event_text[event], 
			l2state_text[currstate], 
			currstate, 
			l2state_text[newstate], 
			newstate);
	}
/* 
 * execute state transition function 
 */
	(*l2state_tab[event][currstate].func)(l2, l3);

	if (newstate == ST_SUBSET) {	
/* 
 * state function DOES set new state 
 */
		NDBGL2(L2_F_MSG, "FSM S-event [%s]: [%s => %s]", 
			l2event_text[event], 
			l2state_text[currstate], 
			l2state_text[l2->Q921_state]);
	}
/* 
 * check for illegal new state 
 */
	if (newstate == ST_ILL) {
		newstate = currstate;
		NDBGL2(L2_F_ERR, "FSM illegal state, state = %s, event = %s!", 
		l2state_text[currstate], 
		l2event_text[event]);
	}
/* 
 * check if state machine function has to set new state 
 */
	if (newstate != ST_SUBSET)
		l2->Q921_state = newstate;        /* no, we set new state */

	if (l2->postfsmfunc != NULL) {
		NDBGL2(L2_F_MSG, "FSM executing postfsmfunc!");
/* 
 * try to avoid an endless loop 
 */
		savpostfsmfunc = l2->postfsmfunc;
		l2->postfsmfunc = NULL;
        
        (*savpostfsmfunc)(l2->postfsmarg);
	}
}

#if I4B_DEBUG
/*---------------------------------------------------------------------------*
 *	return pointer to current state description
 *---------------------------------------------------------------------------*/
const char *
i4b_print_l2state(struct isdn_l2 *l2)
{
	return(l2state_text[l2->Q921_state]);
}
#endif

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_UNAS event dl establish request
 *---------------------------------------------------------------------------*/
static void
F_TU01(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TU01 executing");
	i4b_mdl_assign_ind(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_UNAS event mdl assign request
 *---------------------------------------------------------------------------*/
static void
F_TU03(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TU03 executing");
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_ASG_AW_TEI event mdl assign request
 *---------------------------------------------------------------------------*/
static void
F_TA03(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TA03 executing");
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_ASG_AW_TEI event mdl error response
 *---------------------------------------------------------------------------*/
static void
F_TA04(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TA04 executing");
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_ASG_AW_TEI event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_TA05(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TA05 executing");
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_EST_AW_TEI event mdl assign request
 *---------------------------------------------------------------------------*/
static void
F_TE03(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TE03 executing");
	isdn_l2_establish(l2);
	l2->l3initiated = 1;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_EST_AW_TEI event mdl error response
 *---------------------------------------------------------------------------*/
static void
F_TE04(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TE04 executing");
	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_ind;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_EST_AW_TEI event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_TE05(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TE05 executing");
	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_ind;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event dl establish request
 *---------------------------------------------------------------------------*/
static void
F_T01(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_T01 executing");
	isdn_l2_establish(l2);
	l2->l3initiated = 1;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_T05(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_T05 executing");
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event mdl remove request
 *---------------------------------------------------------------------------*/
static void
F_T06(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_T06 executing");
/*
 * XXX
 */	
	i4b_mdl_assign_ind(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event rx'd SABME
 *---------------------------------------------------------------------------*/
static void
F_T07(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_T07 executing");

/* XXX */
#ifdef NOTDEF
	if (NOT able to establish) {
		i4b_tx_dm(l2, l2->rxd_PF);
		l2->Q921_state = ST_TEI_ASGD;
		return;
	}
#endif

	i4b_clear_exception_conditions(l2);

	i4b_mdl_status_ind(l2->l3, STI_L2STAT, LAYER_ACTIVE);

	i4b_tx_ua(l2, l2->rxd_PF);

	l2->vs = 0;
	l2->va = 0;
	l2->vr = 0;

	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_establish_ind;

	i4b_T203_start(l2);

	l2->Q921_state = ST_MULTIFR;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event rx'd DISC
 *---------------------------------------------------------------------------*/
static void
F_T08(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_T08 executing");
	i4b_mdl_status_ind(l2->l3, STI_L2STAT, LAYER_IDLE);
	i4b_tx_ua(l2, l2->rxd_PF);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event rx'd UA
 *---------------------------------------------------------------------------*/
static void
F_T09(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_T09 executing");
	i4b_mdl_error_ind(l2, "F_T09", MDL_ERR_C);
	i4b_mdl_error_ind(l2, "F_T09", MDL_ERR_D);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event rx'd DM
 *---------------------------------------------------------------------------*/
static void
F_T10(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_T10 executing");

	if (l2->rxd_PF) 
		l2->Q921_state = ST_TEI_ASGD;
	else {
#ifdef NOTDEF
		if (NOT able_to_etablish) {
			l2->Q921_state = ST_TEI_ASGD;
			return;
		}
#endif
		isdn_l2_establish(l2);

		l2->l3initiated = 1;

		l2->Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event dl release request
 *---------------------------------------------------------------------------*/
static void
F_T13(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_T13 executing");
	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_cnf;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event dl establish request
 *---------------------------------------------------------------------------*/
static void
F_AE01(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AE01 executing");

	IF_DRAIN(&l2->i_queue);

	l2->l3initiated = 1;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_AE05(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AE05 executing");

	IF_DRAIN(&l2->i_queue);

	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_ind;

	i4b_T200_stop(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event mdl remove request
 *---------------------------------------------------------------------------*/
static void
F_AE06(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AE06 executing");

	IF_DRAIN(&l2->i_queue);

	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_ind;

	i4b_T200_stop(l2);
/*
 * XXX
 */		
	i4b_mdl_assign_ind(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event rx'd SABME
 *---------------------------------------------------------------------------*/
static void
F_AE07(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AE07 executing");
	i4b_mdl_status_ind(l2->l3, STI_L2STAT, LAYER_ACTIVE);
	i4b_tx_ua(l2, l2->rxd_PF);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event rx'd DISC
 *---------------------------------------------------------------------------*/
static void
F_AE08(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AE08 executing");
	i4b_tx_dm(l2, l2->rxd_PF);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event rx'd UA
 *---------------------------------------------------------------------------*/
static void
F_AE09(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AE09 executing");

	if (l2->rxd_PF == 0) {
		i4b_mdl_error_ind(l2, "F_AE09", MDL_ERR_D);
		l2->Q921_state = ST_AW_EST;
	} else {
		if (l2->l3initiated) {
			l2->l3initiated = 0;
			l2->vr = 0;
			l2->postfsmarg = l2->l3;
			l2->postfsmfunc = i4b_dl_establish_cnf;
		} else {
			if (l2->vs != l2->va) {
				IF_DRAIN(&l2->i_queue);
				l2->postfsmarg = l2->l3;
				l2->postfsmfunc = i4b_dl_establish_ind;
			}
		}
		i4b_mdl_status_ind(l2->l3, STI_L2STAT, LAYER_ACTIVE);

		i4b_T200_stop(l2);
		i4b_T203_start(l2);

		l2->vs = 0;
		l2->va = 0;

		l2->Q921_state = ST_MULTIFR;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event rx'd DM
 *---------------------------------------------------------------------------*/
static void
F_AE10(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AE10 executing");

	if (l2->rxd_PF == 0) 
		l2->Q921_state = ST_AW_EST;
	else {
		IF_DRAIN(&l2->i_queue);

		l2->postfsmarg = l2->l3;
		l2->postfsmfunc = i4b_dl_release_ind;

		i4b_T200_stop(l2);

		l2->Q921_state = ST_TEI_ASGD;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event T200 expiry
 *---------------------------------------------------------------------------*/
static void
F_AE11(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AE11 executing");

	if (l2->RC >= N200) {
		IF_DRAIN(&l2->i_queue);

		i4b_mdl_error_ind(l2, "F_AE11", MDL_ERR_G);

		l2->postfsmarg = l2->l3;
		l2->postfsmfunc = i4b_dl_release_ind;

		l2->Q921_state = ST_TEI_ASGD;
	} else {
		l2->RC++;

		i4b_tx_sabme(l2, P1);

		i4b_T200_start(l2);

		l2->Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event dl data request
 *---------------------------------------------------------------------------*/
static void
F_AE12(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AE12 executing");

	if (l2->l3initiated == 0) 
		i4b_i_frame_queued_up(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_AR05(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AR05 executing");

	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_cnf;

	i4b_T200_stop(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event mdl remove request
 *---------------------------------------------------------------------------*/
static void
F_AR06(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AR06 executing");

	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_cnf;

	i4b_T200_stop(l2);
/*
 * XXX
 */	
	i4b_mdl_assign_ind(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event rx'd SABME
 *---------------------------------------------------------------------------*/
static void
F_AR07(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AR07 executing");
	i4b_tx_dm(l2, l2->rxd_PF);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event rx'd DISC
 *---------------------------------------------------------------------------*/
static void
F_AR08(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AR08 executing");
	i4b_mdl_status_ind(l2->l3, STI_L2STAT, LAYER_IDLE);
	i4b_tx_ua(l2, l2->rxd_PF);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event rx'd UA
 *---------------------------------------------------------------------------*/
static void
F_AR09(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AR09 executing");

	if (l2->rxd_PF) {
		l2->postfsmarg = l2->l3;
		l2->postfsmfunc = i4b_dl_release_cnf;

		i4b_T200_stop(l2);

		l2->Q921_state = ST_TEI_ASGD;
	} else {
		i4b_mdl_error_ind(l2, "F_AR09", MDL_ERR_D);

		l2->Q921_state = ST_AW_REL;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event rx'd DM
 *---------------------------------------------------------------------------*/
static void
F_AR10(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AR10 executing");

	if (l2->rxd_PF) {
		l2->postfsmarg = l2->l3;
		l2->postfsmfunc = i4b_dl_release_cnf;

		i4b_T200_stop(l2);

		l2->Q921_state = ST_TEI_ASGD;
	} else 
		l2->Q921_state = ST_AW_REL;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event T200 expiry
 *---------------------------------------------------------------------------*/
static void
F_AR11(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_AR11 executing");

	if (l2->RC >= N200) {
		i4b_mdl_error_ind(l2, "F_AR11", MDL_ERR_H);

		l2->postfsmarg = l2->l3;
		l2->postfsmfunc = i4b_dl_release_cnf;

		l2->Q921_state = ST_TEI_ASGD;
	} else {
		l2->RC++;

		i4b_tx_disc(l2, P1);

		i4b_T200_start(l2);

		l2->Q921_state = ST_AW_REL;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event dl establish request
 *---------------------------------------------------------------------------*/
static void
F_MF01(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF01 executing");

	IF_DRAIN(&l2->i_queue);

	isdn_l2_establish(l2);

	l2->l3initiated = 1;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_MF05(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF05 executing");

	IF_DRAIN(&l2->i_queue);

	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_ind;

	i4b_T200_stop(l2);
	i4b_T203_stop(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event mdl remove request
 *---------------------------------------------------------------------------*/
static void
F_MF06(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF06 executing");

	IF_DRAIN(&l2->i_queue);

	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_ind;

	i4b_T200_stop(l2);
	i4b_T203_stop(l2);
/*
 * XXX
 */	
	i4b_mdl_assign_ind(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd SABME
 *---------------------------------------------------------------------------*/
static void
F_MF07(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF07 executing");

	i4b_clear_exception_conditions(l2);

	i4b_mdl_status_ind(l2->l3, STI_L2STAT, LAYER_ACTIVE);

	i4b_tx_ua(l2, l2->rxd_PF);

	i4b_mdl_error_ind(l2, "F_MF07", MDL_ERR_F);

	if (l2->vs != l2->va) {
		IF_DRAIN(&l2->i_queue);

		l2->postfsmarg = l2->l3;
		l2->postfsmfunc = i4b_dl_establish_ind;
	}

	i4b_T200_stop(l2);
	i4b_T203_start(l2);

	l2->vs = 0;
	l2->va = 0;
	l2->vr = 0;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd DISC
 *---------------------------------------------------------------------------*/
static void
F_MF08(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF08 executing");

	IF_DRAIN(&l2->i_queue);
	
	i4b_mdl_status_ind(l2->l3, STI_L2STAT, LAYER_IDLE);
	i4b_tx_ua(l2, l2->rxd_PF);

	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_ind;

	i4b_T200_stop(l2);
	i4b_T203_stop(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd UA
 *---------------------------------------------------------------------------*/
static void
F_MF09(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF09 executing");
	
	if (l2->rxd_PF)
		i4b_mdl_error_ind(l2, "F_MF09", MDL_ERR_C);
	else
		i4b_mdl_error_ind(l2, "F_MF09", MDL_ERR_D);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd DM
 *---------------------------------------------------------------------------*/
static void
F_MF10(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF10 executing");

	if (l2->rxd_PF) {
		i4b_mdl_error_ind(l2, "F_MF10", MDL_ERR_B);

		l2->Q921_state = ST_MULTIFR;
	} else {
		i4b_mdl_error_ind(l2, "F_MF10", MDL_ERR_E);

		isdn_l2_establish(l2);

		l2->l3initiated = 0;

		l2->Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event T200 expiry
 *---------------------------------------------------------------------------*/
static void
F_MF11(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF11 executing");

	l2->RC = 0;

	i4b_transmit_enquire(l2);

	l2->RC++;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event dl data request
 *---------------------------------------------------------------------------*/
static void
F_MF12(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF12 executing");

	i4b_i_frame_queued_up(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event dl release request
 *---------------------------------------------------------------------------*/
static void
F_MF13(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF13 executing");

	IF_DRAIN(&l2->i_queue);

	l2->RC = 0;

	i4b_tx_disc(l2, P1);

	i4b_T203_stop(l2);
	i4b_T200_restart(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event T203 expiry
 *---------------------------------------------------------------------------*/
static void
F_MF14(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF14 executing");

	i4b_transmit_enquire(l2);

	l2->RC = 0;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event set own rx busy
 *---------------------------------------------------------------------------*/
static void
F_MF15(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF15 executing");

	if (l2->own_busy == 0) {
		l2->own_busy = 1;

		i4b_tx_rnr_response(l2, F0); /* wrong in Q.921 03/93 p 64 */

		l2->ack_pend = 0;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event clear own rx busy
 *---------------------------------------------------------------------------*/
static void
F_MF16(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF16 executing");

	if (l2->own_busy != 0) {
		l2->own_busy = 0;

		i4b_tx_rr_response(l2, F0); /* wrong in Q.921 03/93 p 64 */

		l2->ack_pend = 0;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd RR
 *---------------------------------------------------------------------------*/
static void
F_MF17(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF17 executing");

	l2->peer_busy = 0;

	if (l2->rxd_CR == CR_CMD_FROM_NT) {
		if (l2->rxd_PF == 1) 
			i4b_enquiry_response(l2);
		
	} else {
		if (l2->rxd_PF == 1) 
			i4b_mdl_error_ind(l2, "F_MF17", MDL_ERR_A);
	}

	if (i4b_l2_nr_ok(l2->rxd_NR, l2->va, l2->vs)) {
		if (l2->rxd_NR == l2->vs) {
			l2->va = l2->rxd_NR;
			i4b_T200_stop(l2);
			i4b_T203_restart(l2);
		} else if (l2->rxd_NR != l2->va) {
			l2->va = l2->rxd_NR;
			i4b_T200_restart(l2);
		}
		l2->Q921_state = ST_MULTIFR;
	} else {
		i4b_nr_error_recovery(l2);
		l2->Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd REJ
 *---------------------------------------------------------------------------*/
static void
F_MF18(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF18 executing");

	l2->peer_busy = 0;

	if (l2->rxd_CR == CR_CMD_FROM_NT) {
		if (l2->rxd_PF == 1) 
			i4b_enquiry_response(l2);	
	} else {
		if (l2->rxd_PF == 1) 
			i4b_mdl_error_ind(l2, "F_MF18", MDL_ERR_A);
	}

	if (i4b_l2_nr_ok(l2->rxd_NR, l2->va, l2->vs)) {
		l2->va = l2->rxd_NR;
		i4b_T200_stop(l2);
		i4b_T203_start(l2);
		i4b_invoke_retransmission(l2, l2->rxd_NR);
		l2->Q921_state = ST_MULTIFR;
	} else {
		i4b_nr_error_recovery(l2);
		l2->Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd RNR
 *---------------------------------------------------------------------------*/
static void
F_MF19(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF19 executing");

	l2->peer_busy = 1;

	if (l2->rxd_CR == CR_CMD_FROM_NT) {
		if (l2->rxd_PF == 1) 
			i4b_enquiry_response(l2);
	} else {
		if (l2->rxd_PF == 1)
			i4b_mdl_error_ind(l2, "F_MF19", MDL_ERR_A);
	}

	if (i4b_l2_nr_ok(l2->rxd_NR, l2->va, l2->vs)) {
		l2->va = l2->rxd_NR;
		i4b_T203_stop(l2);
        i4b_T200_restart(l2);
		l2->Q921_state = ST_MULTIFR;
	} else {
		i4b_nr_error_recovery(l2);
		l2->Q921_state = ST_AW_EST;
    }
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd FRMR
 *---------------------------------------------------------------------------*/
static void
F_MF20(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_MF20 executing");

	i4b_mdl_error_ind(l2, "F_MF20", MDL_ERR_K);

	isdn_l2_establish(l2);

	l2->l3initiated = 0;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event dl establish request
 *---------------------------------------------------------------------------*/
static void
F_TR01(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR01 executing");

	IF_DRAIN(&l2->i_queue);

	isdn_l2_establish(l2);

	l2->l3initiated = 1;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_TR05(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR05 executing");

	IF_DRAIN(&l2->i_queue);

	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_ind;

	i4b_T200_stop(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event mdl remove request
 *---------------------------------------------------------------------------*/
static void
F_TR06(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR06 executing");

	IF_DRAIN(&l2->i_queue);

	l2->postfsmarg = l2->l3;
	l2->postfsmfunc = i4b_dl_release_ind;

	i4b_T200_stop(l2);

/*XXX*/	i4b_mdl_assign_ind(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd SABME
 *---------------------------------------------------------------------------*/
static void
F_TR07(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR07 executing");

	i4b_clear_exception_conditions(l2);

	i4b_mdl_status_ind(l2->l3, STI_L2STAT, LAYER_ACTIVE);

	i4b_tx_ua(l2, l2->rxd_PF);

	i4b_mdl_error_ind(l2, "F_TR07", MDL_ERR_F);

	if (l2->vs != l2->va)
	{
		IF_DRAIN(&l2->i_queue);

		l2->postfsmarg = l2->l3;
		l2->postfsmfunc = i4b_dl_establish_ind;
	}

	i4b_T200_stop(l2);
	i4b_T203_start(l2);

	l2->vs = 0;
	l2->va = 0;
	l2->vr = 0;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd DISC
 *---------------------------------------------------------------------------*/
static void
F_TR08(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR08 executing");

	IF_DRAIN(&l2->i_queue);
	i4b_mdl_status_ind(l3, STI_L2STAT, LAYER_IDLE);
	i4b_tx_ua(l2, l2->rxd_PF);

	l2->postfsmarg = l3;
	l2->postfsmfunc = i4b_dl_release_ind;

	i4b_T200_stop(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd UA
 *---------------------------------------------------------------------------*/
static void
F_TR09(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR09 executing");
	if (l2->rxd_PF)
		i4b_mdl_error_ind(l2, "F_TR09", MDL_ERR_C);
	else
		i4b_mdl_error_ind(l2, "F_TR09", MDL_ERR_D);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd DM
 *---------------------------------------------------------------------------*/
static void
F_TR10(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR10 executing");

	if (l2->rxd_PF) 
		i4b_mdl_error_ind(l2, "F_TR10", MDL_ERR_B);
	else 
		i4b_mdl_error_ind(l2, "F_TR10", MDL_ERR_E);

	isdn_l2_establish(l2);

	l2->l3initiated = 0;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event T200 expiry
 *---------------------------------------------------------------------------*/
static void
F_TR11(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR11 executing");

	if (l2->RC >= N200) {
		i4b_mdl_error_ind(l2, "F_TR11", MDL_ERR_I);

		isdn_l2_establish(l2);

		l2->l3initiated = 0;

		l2->Q921_state = ST_AW_EST;
	} else {
		i4b_transmit_enquire(l2);

		l2->RC++;

		l2->Q921_state = ST_TIMREC;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event dl data request
 *---------------------------------------------------------------------------*/
static void
F_TR12(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR12 executing");

	i4b_i_frame_queued_up(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event dl release request
 *---------------------------------------------------------------------------*/
static void
F_TR13(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR13 executing");

	IF_DRAIN(&l2->i_queue);

	l2->RC = 0;

	i4b_tx_disc(l2, P1);

	i4b_T200_restart(l2);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event set own rx busy
 *---------------------------------------------------------------------------*/
static void
F_TR15(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR15 executing");

	if (l2->own_busy == 0) {
		l2->own_busy = 1;

		i4b_tx_rnr_response(l2, F0);

		l2->ack_pend = 0;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event clear own rx busy
 *---------------------------------------------------------------------------*/
static void
F_TR16(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR16 executing");

	if (l2->own_busy != 0) {
		l2->own_busy = 0;

		i4b_tx_rr_response(l2, F0);	/* this is wrong	 */
						/* in Q.921 03/93 p 74 ! */
		l2->ack_pend = 0;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd RR
 *---------------------------------------------------------------------------*/
static void
F_TR17(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR17 executing");

	l2->peer_busy = 0;

	if (l2->rxd_CR == CR_CMD_FROM_NT) {
		if (l2->rxd_PF == 1)
			i4b_enquiry_response(l2);
		
	} else {
		if (l2->rxd_PF == 1) {
			if (i4b_l2_nr_ok(l2->rxd_NR, l2->va, l2->vs)) {
				l2->va = l2->rxd_NR;
				i4b_T200_stop(l2);
				i4b_T203_start(l2);
				i4b_invoke_retransmission(l2, l2->rxd_NR);
				l2->Q921_state = ST_MULTIFR;
				return;
			} else {
				i4b_nr_error_recovery(l2);
				l2->Q921_state = ST_AW_EST;
				return;
			}
		}
	}

	if (i4b_l2_nr_ok(l2->rxd_NR, l2->va, l2->vs)) {
		l2->va = l2->rxd_NR;
		l2->Q921_state = ST_TIMREC;
	} else {
		i4b_nr_error_recovery(l2);
		l2->Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event
 *---------------------------------------------------------------------------*/
static void
F_TR18(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR18 executing");

	l2->peer_busy = 0;

	if (l2->rxd_CR == CR_CMD_FROM_NT) {
		if (l2->rxd_PF == 1) 
			i4b_enquiry_response(l2);
	} else {
		if (l2->rxd_PF == 1) {
			if (i4b_l2_nr_ok(l2->rxd_NR, l2->va, l2->vs)) {
				l2->va = l2->rxd_NR;
				i4b_T200_stop(l2);
				i4b_T203_start(l2);
				i4b_invoke_retransmission(l2, l2->rxd_NR);
				l2->Q921_state = ST_MULTIFR;
				return;
			} else {
				i4b_nr_error_recovery(l2);
				l2->Q921_state = ST_AW_EST;
				return;
			}
		}
	}

	if (i4b_l2_nr_ok(l2->rxd_NR, l2->va, l2->vs)) {
		l2->va = l2->rxd_NR;
		l2->Q921_state = ST_TIMREC;
	} else {
		i4b_nr_error_recovery(l2);
		l2->Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd RNR
 *---------------------------------------------------------------------------*/
static void
F_TR19(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR19 executing");

	l2->peer_busy = 0;

	if (l2->rxd_CR == CR_CMD_FROM_NT) {
		if (l2->rxd_PF == 1) 
			i4b_enquiry_response(l2);
	} else {
		if (l2->rxd_PF == 1) {
			if (i4b_l2_nr_ok(l2->rxd_NR, l2->va, l2->vs)) {
				l2->va = l2->rxd_NR;
				i4b_T200_restart(l2);
				i4b_invoke_retransmission(l2, l2->rxd_NR);
				l2->Q921_state = ST_MULTIFR;
				return;
			} else {
				i4b_nr_error_recovery(l2);
				l2->Q921_state = ST_AW_EST;
				return;
			}
		}
	}

	if (i4b_l2_nr_ok(l2->rxd_NR, l2->va, l2->vs)) {
		l2->va = l2->rxd_NR;
		l2->Q921_state = ST_TIMREC;
	} else {
		i4b_nr_error_recovery(l2);
		l2->Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd FRMR
 *---------------------------------------------------------------------------*/
static void
F_TR20(struct isdn_l2 *l2, struct isdn_l3 *l3)
{
	NDBGL2(L2_F_MSG, "FSM function F_TR20 executing");

	i4b_mdl_error_ind(l2, "F_TR20", MDL_ERR_K);

	isdn_l2_establish(l2);

	l2->l3initiated = 0;
}
