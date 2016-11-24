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
 *
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

#include <netisdn/i4b_global.h>
#include <netisdn/i4b_l2.h>

#include <netisdn/i4b_l2fsm.h>

#ifdef ISDN_DEBUG
static const char *isdn_dl_state_text[N_STATES] = {
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

static const char *isdn_dl_event_text[N_EVENTS] = {
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
#endif 	/* ISDN_DEBUG */

static void 	F_TU01(struct isdn_ifaddr *);
static void 	F_TU03(struct isdn_ifaddr *);

static void 	F_TA03(struct isdn_ifaddr *);
static void 	F_TA04(struct isdn_ifaddr *);
static void 	F_TA05(struct isdn_ifaddr *);

static void 	F_TE03(struct isdn_ifaddr *);
static void 	F_TE04(struct isdn_ifaddr *);
static void 	F_TE05(struct isdn_ifaddr *);

static void 	F_T01(struct isdn_ifaddr *);
static void 	F_T05(struct isdn_ifaddr *);
static void 	F_T06(struct isdn_ifaddr *);
static void 	F_T07(struct isdn_ifaddr *);
static void 	F_T08(struct isdn_ifaddr *);
static void 	F_T09(struct isdn_ifaddr *);
static void 	F_T10(struct isdn_ifaddr *);
static void 	F_T13(struct isdn_ifaddr *);

static void 	F_AE01(struct isdn_ifaddr *);
static void 	F_AE05(struct isdn_ifaddr *);
static void 	F_AE06(struct isdn_ifaddr *);
static void 	F_AE07(struct isdn_ifaddr *);
static void 	F_AE08(struct isdn_ifaddr *);
static void 	F_AE09(struct isdn_ifaddr *);
static void 	F_AE10(struct isdn_ifaddr *);
static void 	F_AE11(struct isdn_ifaddr *);
static void 	F_AE12(struct isdn_ifaddr *);

static void 	F_AR05(struct isdn_ifaddr *);
static void 	F_AR06(struct isdn_ifaddr *);
static void 	F_AR07(struct isdn_ifaddr *);
static void 	F_AR08(struct isdn_ifaddr *);
static void 	F_AR09(struct isdn_ifaddr *);
static void 	F_AR10(struct isdn_ifaddr *);
static void 	F_AR11(struct isdn_ifaddr *);

static void 	F_MF01(struct isdn_ifaddr *);
static void 	F_MF05(struct isdn_ifaddr *);
static void 	F_MF06(struct isdn_ifaddr *);
static void 	F_MF07(struct isdn_ifaddr *);
static void 	F_MF08(struct isdn_ifaddr *);
static void 	F_MF09(struct isdn_ifaddr *);
static void 	F_MF10(struct isdn_ifaddr *);
static void 	F_MF11(struct isdn_ifaddr *);
static void 	F_MF12(struct isdn_ifaddr *);
static void 	F_MF13(struct isdn_ifaddr *);
static void 	F_MF14(struct isdn_ifaddr *);
static void 	F_MF15(struct isdn_ifaddr *);
static void 	F_MF16(struct isdn_ifaddr *);
static void 	F_MF17(struct isdn_ifaddr *);
static void 	F_MF18(struct isdn_ifaddr *);
static void 	F_MF19(struct isdn_ifaddr *);
static void 	F_MF20(struct isdn_ifaddr *);

static void 	F_TR01(struct isdn_ifaddr *);
static void 	F_TR05(struct isdn_ifaddr *);
static void 	F_TR06(struct isdn_ifaddr *);
static void 	F_TR07(struct isdn_ifaddr *);
static void 	F_TR08(struct isdn_ifaddr *);
static void 	F_TR09(struct isdn_ifaddr *);
static void 	F_TR10(struct isdn_ifaddr *);
static void 	F_TR11(struct isdn_ifaddr *);
static void 	F_TR12(struct isdn_ifaddr *);
static void 	F_TR13(struct isdn_ifaddr *);
static void 	F_TR15(struct isdn_ifaddr *);
static void 	F_TR16(struct isdn_ifaddr *);
static void 	F_TR17(struct isdn_ifaddr *);
static void 	F_TR18(struct isdn_ifaddr *);
static void 	F_TR19(struct isdn_ifaddr *);
static void 	F_TR20(struct isdn_ifaddr *);
static void 	F_ILL(struct isdn_ifaddr *);
static void 	F_NCNA(struct isdn_ifaddr *);

/*---------------------------------------------------------------------------*
 *	FSM illegal state default action
 *---------------------------------------------------------------------------*/
static void
F_ILL(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_ERR, "FSM function F_ILL executing");
}

/*---------------------------------------------------------------------------*
 *	FSM No change, No action
 *---------------------------------------------------------------------------*/
static void
F_NCNA(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "FSM function F_NCNA executing");
}

/*---------------------------------------------------------------------------*
 *	layer 2 state transition table
 *---------------------------------------------------------------------------*/
struct isdn_dl_state_tab {
/* 
 * function to execute 
 */
	void (*lst_fn)(struct isdn_ifaddr *);	
/* 
 * next state 
 */
	int lst_next;
} isdn_dl_state_tab[N_EVENTS][N_STATES] = {

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
isdn_dl_next_state(struct isdn_ifaddr *ii, int event)
{
	int curr, next;
	int (*post_fsm_fn)(struct isdn_ifaddr *);
/* 
 * check event number 
 */
	if (event > N_EVENTS)
		panic("%s: event > N_EVENTS", __func__);
/* 
 * get current state and check it 
 */
	if ((curr = ii->ii_Q921_state) > N_STATES) 	/* failsafe */
		panic("%s: curr > N_STATES", __func__);
/* 
 * get new state and check it 
 */
	if ((next = isdn_dl_state_tab[event][curr].lst_next) > N_STATES)
		panic("%s: next > N_STATES", __func__);


	if (next != ST_SUBSET) {	
/* 
 * state function does NOT set new state 
 */
		NDBGL2(L2_F_MSG, "%s event [%s]: [%s/%d => %s/%d]", 
			__func__,
			isdn_dl_event_text[event], 
			isdn_dl_state_text[curr], 
			curr, 
			isdn_dl_state_text[next], 
			next);
	}
/* 
 * execute state transition function 
 */
	(*isdn_dl_state_tab[event][curr].lst_fn)(ii);

	if (next == ST_SUBSET) {	
/* 
 * state function DOES set new state 
 */
		NDBGL2(L2_F_MSG, "%s S-event [%s]: [%s => %s]", 
			__func__,
			isdn_dl_event_text[event], 
			isdn_dl_state_text[curr], 
			isdn_dl_state_text[ii->ii_Q921_state]);
	}
/* 
 * check for illegal new state 
 */
	if (next == ST_ILL) {
		next = curr;
		NDBGL2(L2_F_ERR, "%s illegal state, state = %s, event = %s!", 
		__func__,
		isdn_dl_state_text[curr], 
		isdn_dl_event_text[event]);
	}
/* 
 * check if state machine function has to set new state 
 */
	if (next != ST_SUBSET)
		ii->ii_Q921_state = next;        /* no, we set new state */

	if (ii->ii_post_fsm_fn != NULL) {
		NDBGL2(L2_F_MSG, "%s executing post_fsm_fn!", __func__);
/* 
 * try to avoid an endless loop 
 */
		post_fsm_fn = ii->ii_post_fsm_fn;
		ii->ii_post_fsm_fn = NULL;
        
        (void)(*post_fsm_fn)(ii);
	}
}

/*---------------------------------------------------------------------------*
 *	return pointer to current state description
 *---------------------------------------------------------------------------*/
#if ISDN_DEBUG
const char *
i4b_print_l2state(struct isdn_ifaddr *ii)
{
	return (isdn_dl_state_text[ii->ii_Q921_state]);
}
#endif 	/* ISDN_DEBUG */

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_UNAS event dl establish request
 *---------------------------------------------------------------------------*/
static void
F_TU01(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	isdn_lme_assign_ind(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_UNAS event mdl assign request
 *---------------------------------------------------------------------------*/
static void
F_TU03(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_ASG_AW_TEI event mdl assign request
 *---------------------------------------------------------------------------*/
static void
F_TA03(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_ASG_AW_TEI event mdl error response
 *---------------------------------------------------------------------------*/
static void
F_TA04(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_ASG_AW_TEI event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_TA05(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_EST_AW_TEI event mdl assign request
 *---------------------------------------------------------------------------*/
static void
F_TE03(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	isdn_dl_establish(ii);
	ii->ii_l3_init = 1;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_EST_AW_TEI event mdl error response
 *---------------------------------------------------------------------------*/
static void
F_TE04(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	ii->ii_post_fsm_fn = isdn_dl_release_ind;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_EST_AW_TEI event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_TE05(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	ii->ii_post_fsm_fn = isdn_dl_release_ind;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event dl establish request
 *---------------------------------------------------------------------------*/
static void
F_T01(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	isdn_dl_establish(ii);
	ii->ii_l3_init = 1;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_T05(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event mdl remove request
 *---------------------------------------------------------------------------*/
static void
F_T06(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
/*
 * XXX
 */	
	isdn_lme_assign_ind(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event rx'd SABME
 *---------------------------------------------------------------------------*/
static void
F_T07(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

/* XXX */
#ifdef NOTDEF
	if (NOT able to establish) {
		(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, DM);
		ii->ii_Q921_state = ST_TEI_ASGD;
		return;
	}
#endif

	isdn_dl_clear_exeption_cond(ii);

	isdn_mdl_status_ind(ii->ii_l3, STI_L2STAT, LAYER_ACTIVE);

	(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, UA);

	ii->ii_vs = 0;
	ii->ii_va = 0;
	ii->ii_vr = 0;

	ii->ii_post_fsm_fn = isdn_dl_establish_ind;

	isdn_T203_start(ii);

	ii->ii_Q921_state = ST_MULTIFR;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event rx'd DISC
 *---------------------------------------------------------------------------*/
static void
F_T08(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	isdn_mdl_status_ind(ii->ii_l3, STI_L2STAT, LAYER_IDLE);
	(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, UA);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event rx'd UA
 *---------------------------------------------------------------------------*/
static void
F_T09(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	isdn_lme_error_ind(ii, "F_T09", MDL_ERR_C);
	isdn_lme_error_ind(ii, "F_T09", MDL_ERR_D);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event rx'd DM
 *---------------------------------------------------------------------------*/
static void
F_T10(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_rxd_PF) 
		ii->ii_Q921_state = ST_TEI_ASGD;
	else {
#ifdef NOTDEF
		if (NOT able_to_etablish) {
			ii->ii_Q921_state = ST_TEI_ASGD;
			return;
		}
#endif
		isdn_dl_establish(ii);

		ii->ii_l3_init = 1;

		ii->ii_Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TEI_ASGD event dl release request
 *---------------------------------------------------------------------------*/
static void
F_T13(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	ii->ii_post_fsm_fn = isdn_dl_release_cnf;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event dl establish request
 *---------------------------------------------------------------------------*/
static void
F_AE01(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	ii->ii_l3_init = 1;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_AE05(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	ii->ii_post_fsm_fn = isdn_dl_release_ind;

	isdn_T200_stop(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event mdl remove request
 *---------------------------------------------------------------------------*/
static void
F_AE06(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	ii->ii_post_fsm_fn = isdn_dl_release_ind;

	isdn_T200_stop(ii);
/*
 * XXX
 */		
	isdn_lme_assign_ind(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event rx'd SABME
 *---------------------------------------------------------------------------*/
static void
F_AE07(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	isdn_mdl_status_ind(ii->ii_l3, STI_L2STAT, LAYER_ACTIVE);
	(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, UA);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event rx'd DISC
 *---------------------------------------------------------------------------*/
static void
F_AE08(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, DM);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event rx'd UA
 *---------------------------------------------------------------------------*/
static void
F_AE09(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_rxd_PF == 0) {
		isdn_lme_error_ind(ii, "F_AE09", MDL_ERR_D);
		ii->ii_Q921_state = ST_AW_EST;
	} else {
		if (ii->ii_l3_init) {
			ii->ii_l3_init = 0;
			ii->ii_vr = 0;
			ii->ii_post_fsm_fn = i4b_dl_establish_cnf;
		} else {
			if (ii->ii_vs != ii->ii_va) {
				IF_DRAIN(&ii->ii_i_queue);
				ii->ii_post_fsm_fn = isdn_dl_establish_ind;
			}
		}
		isdn_mdl_status_ind(ii->ii_l3, STI_L2STAT, LAYER_ACTIVE);

		isdn_T200_stop(ii);
		isdn_T203_start(ii);

		ii->ii_vs = 0;
		ii->ii_va = 0;

		ii->ii_Q921_state = ST_MULTIFR;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event rx'd DM
 *---------------------------------------------------------------------------*/
static void
F_AE10(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_rxd_PF == 0) 
		ii->ii_Q921_state = ST_AW_EST;
	else {
		IF_DRAIN(&ii->ii_i_queue);

		ii->ii_post_fsm_fn = isdn_dl_release_ind;

		isdn_T200_stop(ii);

		ii->ii_Q921_state = ST_TEI_ASGD;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event T200 expiry
 *---------------------------------------------------------------------------*/
static void
F_AE11(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_RC >= N200) {
		IF_DRAIN(&ii->ii_i_queue);

		isdn_lme_error_ind(ii, "F_AE11", MDL_ERR_G);

		ii->ii_post_fsm_fn = isdn_dl_release_ind;

		ii->ii_Q921_state = ST_TEI_ASGD;
	} else {
		ii->ii_RC++;

		(void)isdn_tx_u_frame(ii, CR_CMD_TO_NT, P1, SABME);

		isdn_T200_start(ii);

		ii->ii_Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_EST event dl data request
 *---------------------------------------------------------------------------*/
static void
F_AE12(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_l3_init == 0) 
		isdn_dl_queue_i_frame(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_AR05(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	ii->ii_post_fsm_fn = isdn_dl_release_cnf;

	isdn_T200_stop(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event mdl remove request
 *---------------------------------------------------------------------------*/
static void
F_AR06(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	ii->ii_post_fsm_fn = isdn_dl_release_cnf;

	isdn_T200_stop(ii);
/*
 * XXX
 */	
	isdn_lme_assign_ind(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event rx'd SABME
 *---------------------------------------------------------------------------*/
static void
F_AR07(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	
	(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, DM);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event rx'd DISC
 *---------------------------------------------------------------------------*/
static void
F_AR08(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	isdn_mdl_status_ind(ii->ii_l3, STI_L2STAT, LAYER_IDLE);
	(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, UA);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event rx'd UA
 *---------------------------------------------------------------------------*/
static void
F_AR09(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_rxd_PF) {
		ii->ii_post_fsm_fn = isdn_dl_release_cnf;

		isdn_T200_stop(ii);

		ii->ii_Q921_state = ST_TEI_ASGD;
	} else {
		isdn_lme_error_ind(ii, "F_AR09", MDL_ERR_D);

		ii->ii_Q921_state = ST_AW_REL;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event rx'd DM
 *---------------------------------------------------------------------------*/
static void
F_AR10(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_rxd_PF) {
		ii->ii_post_fsm_fn = isdn_dl_release_cnf;

		isdn_T200_stop(ii);

		ii->ii_Q921_state = ST_TEI_ASGD;
	} else 
		ii->ii_Q921_state = ST_AW_REL;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_AW_REL event T200 expiry
 *---------------------------------------------------------------------------*/
static void
F_AR11(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_RC >= N200) {
		isdn_lme_error_ind(ii, "F_AR11", MDL_ERR_H);

		ii->ii_post_fsm_fn = isdn_dl_release_cnf;

		ii->ii_Q921_state = ST_TEI_ASGD;
	} else {
		ii->ii_RC++;

		(void)isdn_tx_u_frame(ii, CR_CMD_TO_NT, P1, DISC);

		isdn_T200_start(ii);

		ii->ii_Q921_state = ST_AW_REL;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event dl establish request
 *---------------------------------------------------------------------------*/
static void
F_MF01(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	isdn_dl_establish(ii);

	ii->ii_l3_init = 1;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_MF05(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	ii->ii_post_fsm_fn = isdn_dl_release_ind;

	isdn_T200_stop(ii);
	isdn_T203_stop(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event mdl remove request
 *---------------------------------------------------------------------------*/
static void
F_MF06(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	ii->ii_post_fsm_fn = isdn_dl_release_ind;

	isdn_T200_stop(ii);
	isdn_T203_stop(ii);
/*
 * XXX
 */	
	isdn_lme_assign_ind(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd SABME
 *---------------------------------------------------------------------------*/
static void
F_MF07(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	isdn_dl_clear_exeption_cond(ii);
/* 
 * XXX ...
 *
	isdn_mdl_status_ind(ii->ii_l3, STI_L2STAT, LAYER_ACTIVE);
 */
	(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, UA);

	isdn_lme_error_ind(ii, "F_MF07", MDL_ERR_F);

	if (ii->ii_vs != ii->ii_va) {
		IF_DRAIN(&ii->ii_i_queue);
/*
 * XXX ...
 */
		ii->ii_post_fsm_fn = isdn_dl_establish_ind;
	}

	isdn_T200_stop(ii);
	isdn_T203_start(ii);

	ii->ii_vs = 0;
	ii->ii_va = 0;
	ii->ii_vr = 0;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd DISC
 *---------------------------------------------------------------------------*/
static void
F_MF08(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);
	
	isdn_mdl_status_ind(ii->ii_l3, STI_L2STAT, LAYER_IDLE);
	
	(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, UA);

	ii->ii_post_fsm_fn = isdn_dl_release_ind;

	isdn_T200_stop(ii);
	isdn_T203_stop(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd UA
 *---------------------------------------------------------------------------*/
static void
F_MF09(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	
	if (ii->ii_rxd_PF)
		isdn_lme_error_ind(ii, "F_MF09", MDL_ERR_C);
	else
		isdn_lme_error_ind(ii, "F_MF09", MDL_ERR_D);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd DM
 *---------------------------------------------------------------------------*/
static void
F_MF10(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_rxd_PF) {
		isdn_lme_error_ind(ii, "F_MF10", MDL_ERR_B);

		ii->ii_Q921_state = ST_MULTIFR;
	} else {
		isdn_lme_error_ind(ii, "F_MF10", MDL_ERR_E);

		isdn_dl_establish(ii);

		ii->ii_l3_init = 0;

		ii->ii_Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event T200 expiry
 *---------------------------------------------------------------------------*/
static void
F_MF11(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	ii->ii_RC = 0;

	isdn_dl_tx_enquire(ii);

	ii->ii_RC++;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event dl data request
 *---------------------------------------------------------------------------*/
static void
F_MF12(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	isdn_dl_queue_i_frame(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event dl release request
 *---------------------------------------------------------------------------*/
static void
F_MF13(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	ii->ii_RC = 0;

	(void)isdn_tx_u_frame(ii, CR_CMD_TO_NT, P1, DISC);

	isdn_T203_stop(ii);
	isdn_T200_restart(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event T203 expiry
 *---------------------------------------------------------------------------*/
static void
F_MF14(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	isdn_dl_tx_enquire(ii);

	ii->ii_RC = 0;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event set own rx busy
 *---------------------------------------------------------------------------*/
static void
F_MF15(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_own_busy == 0) {
		ii->ii_own_busy = 1;
/* 
 * wrong in Q.921 03/93 p 64 
 */
		(void)isdn_tx_s_frame(ii, CR_RSP_TO_NT, F0, RNR);	
		
		ii->ii_ack_pend = 0;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event clear own rx busy
 *---------------------------------------------------------------------------*/
static void
F_MF16(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_own_busy != 0) {
		ii->ii_own_busy = 0;
/* 
 * wrong in Q.921 03/93 p 64 
 */
		(void)isdn_tx_s_frame(ii, CR_RSP_TO_NT, F0, RR);	
		
		ii->ii_ack_pend = 0;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd RR
 *---------------------------------------------------------------------------*/
static void
F_MF17(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	ii->ii_peer_busy = 0;

	if (ii->ii_rxd_CR == CR_CMD_FROM_NT) {
		if (ii->ii_rxd_PF == 1) 
			isdn_dl_enquiry_resp(ii);
		
	} else {
		if (ii->ii_rxd_PF == 1) 
			isdn_lme_error_ind(ii, "F_MF17", MDL_ERR_A);
	}

	if (isdn_dl_nr_ok(ii->ii_rxd_NR, 
		ii->ii_va, ii->ii_vs)) {
		
		if (ii->ii_rxd_NR == ii->ii_vs) {
			ii->ii_va = ii->ii_rxd_NR;
			isdn_T200_stop(ii);
			isdn_T203_restart(ii);
		} else if (ii->ii_rxd_NR != ii->ii_va) {
			ii->ii_va = ii->ii_rxd_NR;
			isdn_T200_restart(ii);
		}
		ii->ii_Q921_state = ST_MULTIFR;
	} else {
		isdn_dl_nr_error_recovery(ii);
		ii->ii_Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd REJ
 *---------------------------------------------------------------------------*/
static void
F_MF18(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	ii->ii_peer_busy = 0;

	if (ii->ii_rxd_CR == CR_CMD_FROM_NT) {
		if (ii->ii_rxd_PF == 1) 
			isdn_dl_enquiry_resp(ii);	
	} else {
		if (ii->ii_rxd_PF == 1) 
			isdn_lme_error_ind(ii, "F_MF18", MDL_ERR_A);
	}

	if (isdn_dl_nr_ok(ii->ii_rxd_NR, ii->ii_va, ii->ii_vs)) {
		ii->ii_va = ii->ii_rxd_NR;
		isdn_T200_stop(ii);
		isdn_T203_start(ii);
		isdn_dl_invoke_rtx(ii, ii->ii_rxd_NR);
		ii->ii_Q921_state = ST_MULTIFR;
	} else {
		isdn_dl_nr_error_recovery(ii);
		ii->ii_Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd RNR
 *---------------------------------------------------------------------------*/
static void
F_MF19(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	ii->ii_peer_busy = 1;

	if (ii->ii_rxd_CR == CR_CMD_FROM_NT) {
		if (ii->ii_rxd_PF == 1) 
			isdn_dl_enquiry_resp(ii);
	} else {
		if (ii->ii_rxd_PF == 1)
			isdn_lme_error_ind(ii, "F_MF19", MDL_ERR_A);
	}

	if (isdn_dl_nr_ok(ii->ii_rxd_NR, ii->ii_va, ii->ii_vs)) {
		ii->ii_va = ii->ii_rxd_NR;
		isdn_T203_stop(ii);
        isdn_T200_restart(ii);
		ii->ii_Q921_state = ST_MULTIFR;
	} else {
		isdn_dl_nr_error_recovery(ii);
		ii->ii_Q921_state = ST_AW_EST;
    }
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_MULTIFR event rx'd FRMR
 *---------------------------------------------------------------------------*/
static void
F_MF20(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	isdn_lme_error_ind(ii, "F_MF20", MDL_ERR_K);

	isdn_dl_establish(ii);

	ii->ii_l3_init = 0;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event dl establish request
 *---------------------------------------------------------------------------*/
static void
F_TR01(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	isdn_dl_establish(ii);

	ii->ii_l3_init = 1;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event persistent deactivation
 *---------------------------------------------------------------------------*/
static void
F_TR05(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	ii->ii_post_fsm_fn = isdn_dl_release_ind;

	isdn_T200_stop(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event mdl remove request
 *---------------------------------------------------------------------------*/
static void
F_TR06(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	ii->ii_post_fsm_fn = isdn_dl_release_ind;

	isdn_T200_stop(ii);

/*XXX*/	isdn_lme_assign_ind(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd SABME
 *---------------------------------------------------------------------------*/
static void
F_TR07(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	isdn_dl_clear_exeption_cond(ii);

	isdn_mdl_status_ind(ii->ii_l3, STI_L2STAT, LAYER_ACTIVE);

	(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, UA);

	isdn_lme_error_ind(ii, "F_TR07", MDL_ERR_F);

	if (ii->ii_vs != ii->ii_va)
	{
		IF_DRAIN(&ii->ii_i_queue);

		ii->ii_post_fsm_fn = isdn_dl_establish_ind;
	}

	isdn_T200_stop(ii);
	isdn_T203_start(ii);

	ii->ii_vs = 0;
	ii->ii_va = 0;
	ii->ii_vr = 0;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd DISC
 *---------------------------------------------------------------------------*/
static void
F_TR08(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);
	isdn_mdl_status_ind(l3, STI_L2STAT, LAYER_IDLE);
	
	(void)isdn_tx_u_frame(ii, CR_RSP_TO_NT, ii->ii_rxd_PF, UA);

	ii->ii_post_fsm_fn = isdn_dl_release_ind;

	isdn_T200_stop(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd UA
 *---------------------------------------------------------------------------*/
static void
F_TR09(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);
	if (ii->ii_rxd_PF)
		isdn_lme_error_ind(ii, "F_TR09", MDL_ERR_C);
	else
		isdn_lme_error_ind(ii, "F_TR09", MDL_ERR_D);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd DM
 *---------------------------------------------------------------------------*/
static void
F_TR10(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_rxd_PF) 
		isdn_lme_error_ind(ii, "F_TR10", MDL_ERR_B);
	else 
		isdn_lme_error_ind(ii, "F_TR10", MDL_ERR_E);

	isdn_dl_establish(ii);

	ii->ii_l3_init = 0;
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event T200 expiry
 *---------------------------------------------------------------------------*/
static void
F_TR11(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_RC >= N200) {
		isdn_lme_error_ind(ii, "F_TR11", MDL_ERR_I);

		isdn_dl_establish(ii);

		ii->ii_l3_init = 0;

		ii->ii_Q921_state = ST_AW_EST;
	} else {
		isdn_dl_tx_enquire(ii);

		ii->ii_RC++;

		ii->ii_Q921_state = ST_TIMREC;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event dl data request
 *---------------------------------------------------------------------------*/
static void
F_TR12(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	isdn_dl_queue_i_frame(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event dl release request
 *---------------------------------------------------------------------------*/
static void
F_TR13(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	IF_DRAIN(&ii->ii_i_queue);

	ii->ii_RC = 0;

	(void)isdn_tx_u_frame(ii, CR_CMD_TO_NT, P1, DISC);

	isdn_T200_restart(ii);
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event set own rx busy
 *---------------------------------------------------------------------------*/
static void
F_TR15(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_own_busy == 0) {
		ii->ii_own_busy = 1;
/*
 * Tx RNR response.
 */				
		(void)isdn_tx_s_frame(ii, CR_RSP_TO_NT, F0, RNR);		

		ii->ii_ack_pend = 0;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event clear own rx busy
 *---------------------------------------------------------------------------*/
static void
F_TR16(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	if (ii->ii_own_busy != 0) {
		ii->ii_own_busy = 0;
/*
 * Tx RNR response, this is wrong in Q.921 03/93 p 74 !
 */				
		(void)isdn_tx_s_frame(ii, CR_RSP_TO_NT, F0, RR);		

		ii->ii_ack_pend = 0;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd RR
 *---------------------------------------------------------------------------*/
static void
F_TR17(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	ii->ii_peer_busy = 0;

	if (ii->ii_rxd_CR == CR_CMD_FROM_NT) {
		if (ii->ii_rxd_PF == 1)
			isdn_dl_enquiry_resp(ii);
		
	} else {
		if (ii->ii_rxd_PF == 1) {
			if (isdn_dl_nr_ok(ii->ii_rxd_NR, 
					ii->ii_va, ii->ii_vs)) {
				ii->ii_va = ii->ii_rxd_NR;
				isdn_T200_stop(ii);
				isdn_T203_start(ii);
				isdn_dl_invoke_rtx(ii, ii->ii_rxd_NR);
				ii->ii_Q921_state = ST_MULTIFR;
				return;
			} else {
				isdn_dl_nr_error_recovery(ii);
				ii->ii_Q921_state = ST_AW_EST;
				return;
			}
		}
	}

	if (isdn_dl_nr_ok(ii->ii_rxd_NR, 
			ii->ii_va, ii->ii_vs)) {
		ii->ii_va = ii->ii_rxd_NR;
		ii->ii_Q921_state = ST_TIMREC;
	} else {
		isdn_dl_nr_error_recovery(ii);
		ii->ii_Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event
 *---------------------------------------------------------------------------*/
static void
F_TR18(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	ii->ii_peer_busy = 0;

	if (ii->ii_rxd_CR == CR_CMD_FROM_NT) {
		if (ii->ii_rxd_PF == 1) 
			isdn_dl_enquiry_resp(ii);
	} else {
		if (ii->ii_rxd_PF == 1) {
			if (isdn_dl_nr_ok(ii->ii_rxd_NR, 
					ii->ii_va, ii->ii_vs)) {
				ii->ii_va = ii->ii_rxd_NR;
				isdn_T200_stop(ii);
				isdn_T203_start(ii);
				isdn_dl_invoke_rtx(ii, ii->ii_rxd_NR);
				ii->ii_Q921_state = ST_MULTIFR;
				return;
			} else {
				isdn_dl_nr_error_recovery(ii);
				ii->ii_Q921_state = ST_AW_EST;
				return;
			}
		}
	}

	if (isdn_dl_nr_ok(ii->ii_rxd_NR, 
			ii->ii_va, ii->ii_vs)) {
		ii->ii_va = ii->ii_rxd_NR;
		ii->ii_Q921_state = ST_TIMREC;
	} else {
		isdn_dl_nr_error_recovery(ii);
		ii->ii_Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd RNR
 *---------------------------------------------------------------------------*/
static void
F_TR19(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	ii->ii_peer_busy = 0;

	if (ii->ii_rxd_CR == CR_CMD_FROM_NT) {
		if (ii->ii_rxd_PF == 1) 
			isdn_dl_enquiry_resp(ii);
	} else {
		if (ii->ii_rxd_PF == 1) {
			if (isdn_dl_nr_ok(ii->ii_rxd_NR, 
					ii->ii_va, ii->ii_vs)) {
				ii->ii_va = ii->ii_rxd_NR;
				isdn_T200_restart(ii);
				isdn_dl_invoke_rtx(ii, ii->ii_rxd_NR);
				ii->ii_Q921_state = ST_MULTIFR;
				return;
			} else {
				isdn_dl_nr_error_recovery(ii);
				ii->ii_Q921_state = ST_AW_EST;
				return;
			}
		}
	}

	if (isdn_dl_nr_ok(ii->ii_rxd_NR, 
			ii->ii_va, ii->ii_vs)) {
		ii->ii_va = ii->ii_rxd_NR;
		ii->ii_Q921_state = ST_TIMREC;
	} else {
		isdn_dl_nr_error_recovery(ii);
		ii->ii_Q921_state = ST_AW_EST;
	}
}

/*---------------------------------------------------------------------------*
 *	FSM state ST_TIMREC event rx'd FRMR
 *---------------------------------------------------------------------------*/
static void
F_TR20(struct isdn_ifaddr *ii)
{
	NDBGL2(L2_F_MSG, "%s executing", __func__);

	isdn_lme_error_ind(ii, "F_TR20", MDL_ERR_K);

	isdn_dl_establish(ii);

	ii->ii_l3_init = 0;
}
