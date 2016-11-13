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

static void 
F_00A(struct isdn_bc *), 
	F_00H(struct isdn_bc *), 
	F_00I(struct isdn_bc *);

static void 
F_00J(struct isdn_bc *);

static void 
F_01B(struct isdn_bc *), 
	F_01K(struct isdn_bc *), 
	F_01L(struct isdn_bc *);
static void 
F_01M(struct isdn_bc *), 
	F_01N(struct isdn_bc *), 
	F_01U(struct isdn_bc *);
static void 
F_01O(struct isdn_bc *);

static void 
F_03C(struct isdn_bc *), 
	F_03N(struct isdn_bc *), 
	F_03O(struct isdn_bc *);
static void 
F_03P(struct isdn_bc *), 
	F_03Y(struct isdn_bc *);

static void 
F_04O(struct isdn_bc *);

static void 
F_06D(struct isdn_bc *), 
	F_06E(struct isdn_bc *), 
	F_06F(struct isdn_bc *);
static void 
F_06G(struct isdn_bc *), 
	F_06J(struct isdn_bc *), 
	F_06Q(struct isdn_bc *);

static void 
F_07E(struct isdn_bc *), 
	F_07F(struct isdn_bc *), 
	F_07G(struct isdn_bc *);

static void 
F_08R(struct isdn_bc *), 
	F_08Z(struct isdn_bc *);

static void 
F_09D(struct isdn_bc *), 
	F_09E(struct isdn_bc *), 
	F_09F(struct isdn_bc *);
static void 
F_09G(struct isdn_bc *);

static void 
F_11J(struct isdn_bc *), 
	F_11Q(struct isdn_bc *), 
	F_11V(struct isdn_bc *);

static void 
F_12C(struct isdn_bc *), 
	F_12J(struct isdn_bc *);

static void 
F_19I(struct isdn_bc *), 
	F_19J(struct isdn_bc *), 
	F_19K(struct isdn_bc *);
static void 
F_19W(struct isdn_bc *);

static void 
F_NCNA(struct isdn_bc *), 
	F_STENQ(struct isdn_bc *), 
	F_STAT(struct isdn_bc *);
static void 
F_INFO(struct isdn_bc *), 
	F_RELCP(struct isdn_bc *), 
	F_REL(struct isdn_bc *);
static void 
F_DISC(struct isdn_bc *), 
	F_DCRQ(struct isdn_bc *), 
	F_UEM(struct isdn_bc *);
static void 
F_SIGN(struct isdn_bc *), 
	F_DLEI(struct isdn_bc *), 
	F_ILL(struct isdn_bc *);
static void 
F_309TO(struct isdn_bc *), 
	F_DECF(struct isdn_bc *), 
	F_FCTY(struct isdn_bc *);
static void 
F_DECF1(struct isdn_bc *), 
	F_DECF2(struct isdn_bc *), 
	F_DECF3(struct isdn_bc *);
static void 
F_DLRI(struct isdn_bc *), 
	F_DLRIA(struct isdn_bc *), 
	F_DECF4(struct isdn_bc *);

#if I4B_DEBUG
static const char *isdn_l3_state_text[N_STATES] = {
	 "ST_U0 - Null",
	 "ST_U1 - Out Init",
	 "ST_U3 - Out Proc",
	 "ST_U4 - Out Delv",
	 "ST_U6 - In Pres",
	 "ST_U7 - In Rxd",
	 "ST_U8 - In ConReq",
	 "ST_U9 - In Proc",
	"ST_U10 - Active",
	"ST_U11 - Disc Req",
	"ST_U12 - Disc Ind",
	"ST_U19 - Rel Req",

	"ST_IWA - In Wait EST-Accept",
	"ST_IWR - In Wait EST-Reject",
	"ST_OW - Out Wait EST",
	"ST_IWL - In Wait EST-Alert",

	"ST_SUSE - Subroutine sets state",

	"Illegal State"
};

static const char *isdn_l3_event_text[N_EVENTS] = {
	"EV_SETUPRQ - L4 SETUP REQ",	/* setup request from L4		*/
	"EV_DISCRQ - L4 DISC REQ",	/* disconnect request from L4		*/
	"EV_RELRQ - L4 REL REQ",	/* release request from L4		*/
	"EV_ALERTRQ - L4 ALERT REQ",	/* alerting request from L4		*/
	"EV_SETACRS - L4 accept RSP",	/* setup response accept from l4	*/
	"EV_SETRJRS - L4 reject RSP",	/* setup response reject from l4	*/
	"EV_SETDCRS - L4 ignore RSP",	/* setup response dontcare from l4	*/

	"EV_SETUP - rxd SETUP",		/* incoming SETUP message from L2	*/
	"EV_STATUS - rxd STATUS",	/* incoming STATUS message from L2	*/
	"EV_RELEASE - rxd REL",		/* incoming RELEASE message from L2	*/
	"EV_RELCOMP - rxd REL COMPL",	/* incoming RELEASE COMPLETE from L2	*/
	"EV_SETUPAK - rxd SETUP ACK",	/* incoming SETUP ACK message from L2	*/
	"EV_CALLPRC - rxd CALL PROC",	/* incoming CALL PROCEEDING from L2	*/
	"EV_ALERT - rxd ALERT",		/* incoming ALERT message from L2	*/
	"EV_CONNECT - rxd CONNECT",	/* incoming CONNECT message from L2	*/
	"EV_PROGIND - rxd PROG IND",	/* incoming Progress IND from L2	*/
	"EV_DISCONN - rxd DISC",	/* incoming DISCONNECT message from L2	*/
	"EV_CONACK - rxd CONN ACK",	/* incoming CONNECT ACK message from L2	*/
	"EV_STATENQ - rxd STAT ENQ",	/* incoming STATUS ENQ message from L2	*/
	"EV_INFO - rxd INFO",		/* incoming INFO message from L2	*/
	"EV_FACILITY - rxd FACILITY",	/* incoming FACILITY message 		*/

	"EV_T303EXP - T303 timeout",	/* Timer T303 expired			*/
	"EV_T305EXP - T305 timeout",	/* Timer T305 expired			*/
	"EV_T308EXP - T308 timeout",	/* Timer T308 expired			*/
	"EV_T309EXP - T309 timeout",	/* Timer T309 expired			*/
	"EV_T310EXP - T310 timeout",	/* Timer T310 expired			*/
	"EV_T313EXP - T313 timeout",	/* Timer T313 expired			*/

	"EV_DLESTIN - L2 DL_Est_Ind",	/* dl establish indication from l2	*/
	"EV_DLRELIN - L2 DL_Rel_Ind",	/* dl release indication from l2	*/
	"EV_DLESTCF - L2 DL_Est_Cnf",	/* dl establish confirm from l2		*/
	"EV_DLRELCF - L2 DL_Rel_Cnf",	/* dl release confirm from l2		*/

	"EV_ILL - Illegal event!!" 	/* Illegal */
};
#endif

/*---------------------------------------------------------------------------*
 *	layer 3 state transition table
 *---------------------------------------------------------------------------*/
struct isdn_l3_state_tab {
/* 
 * function to execute 
 */
	void (*lst_fn)(struct isdn_bc *);
/* 
 * next state 
 */
	int lst_next;
} isdn_l3_state_tab[N_EVENTS][N_STATES] = {

/* STATE:	
	
	ST_U0			
	ST_U1			
	ST_U3			
	ST_U4			
	ST_U6			
	ST_U7			
	ST_U8			
	ST_U9			
	ST_U10			
	ST_U11			
	ST_U12			
	ST_U19			
	ST_IWA			
	ST_IWR			
	ST_OW			
	ST_IWL			
	ST_SUBSET		
	ST_ILL	      
*/
	{
/*
 * EV_SETUPRQ
 */			
		{F_00A,  ST_SUSE},	
		{F_ILL,	 ST_ILL},	
		{F_ILL,  ST_ILL},	
		{F_ILL,  ST_ILL},	
		{F_ILL,  ST_ILL},	
		{F_ILL,  ST_ILL},	
		{F_ILL,	 ST_ILL},	
		{F_ILL,	 ST_ILL},	
		{F_ILL,	 ST_ILL},	
		{F_ILL,	 ST_ILL},	
		{F_ILL,	 ST_ILL},	
		{F_ILL,	 ST_ILL},	
		{F_ILL,  ST_ILL},	
		{F_ILL,  ST_ILL},	
		{F_ILL,  ST_ILL},	
		{F_ILL,  ST_ILL},	
		{F_ILL, ST_ILL},        
		{F_ILL, ST_ILL}
	},
/*
 * EV_DISCRQ 
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_01B,	 ST_U11},	
 		{F_DCRQ, ST_U11},	
 		{F_DCRQ, ST_U11},	
 		{F_ILL,  ST_ILL},	
 		{F_DCRQ, ST_U11},	
 		{F_DCRQ, ST_U11},	
 		{F_DCRQ, ST_U11},	
 		{F_DCRQ, ST_U11},	
 		{F_ILL,	 ST_ILL},	
 		{F_NCNA, ST_U12},	
 		{F_ILL,	 ST_ILL},	
 		{F_DCRQ, ST_U11},	
 		{F_DCRQ, ST_U11},	
 		{F_DCRQ, ST_U11},	
 		{F_DCRQ, ST_U11},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_RELRQ  
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_03C,  ST_U19},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_12C,	 ST_U19},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/* 
 * EV_ALERTRQ
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_06D,  ST_SUSE},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_09D,	 ST_U7},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/* 
 * EV_SETACRS
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_06E,  ST_SUSE},	
 		{F_07E,	 ST_U8},	
 		{F_ILL,	 ST_ILL},	
 		{F_09E,	 ST_U8},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/* 
 * EV_SETRJRS
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_06F,  ST_SUSE},	
 		{F_07F,	 ST_U0},	
 		{F_ILL,	 ST_ILL},	
 		{F_09F,	 ST_U0},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_SETDCRS
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_06G,  ST_U0},	
 		{F_07G,	 ST_U0},	
 		{F_ILL,	 ST_ILL},	
 		{F_09G,	 ST_U0},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/* STATE:	

	ST_U0			
	ST_U1			
	ST_U3			
	ST_U4			
	ST_U6			
	ST_U7			
	ST_U8			
	ST_U9			
	ST_U10			
	ST_U11			
	ST_U12			
	ST_U19			
	ST_IWA			
	ST_IWR			
	ST_OW			
	ST_IWL			
	ST_SUBSET		
	ST_ILL	     
*/	
	{
/*
 * EV_SETUP  
 */		
		{F_00H,  ST_U6},	
 		{F_SIGN, ST_U1},	
 		{F_SIGN, ST_U3},	
 		{F_SIGN, ST_U4},	
 		{F_SIGN, ST_U6},	
 		{F_SIGN, ST_U7},	
 		{F_SIGN, ST_U8},	
 		{F_SIGN, ST_U9},	
 		{F_SIGN, ST_U10},	
 		{F_SIGN, ST_U11},	
 		{F_SIGN, ST_U12},	
 		{F_SIGN, ST_U19},	
 		{F_SIGN, ST_IWA},	
 		{F_SIGN, ST_IWR},	
 		{F_SIGN, ST_OW},	
 		{F_SIGN, ST_IWL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_STATUS 
 */	
	{
		{F_00I,  ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_19I,	 ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_STAT, ST_SUSE},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_RELEASE
 */	
 	{
 		{F_00J,  ST_U0},	
 		{F_UEM,	 ST_SUSE},	
 		{F_REL,  ST_U0},	
 		{F_REL,  ST_U0},	
 		{F_06J,	 ST_U0},	
 		{F_REL,	 ST_U0},	
 		{F_REL,	 ST_U0},	
 		{F_REL,	 ST_U0},	
 		{F_REL,	 ST_U0},	
 		{F_11J,	 ST_U0},	
 		{F_12J,	 ST_U0},	
 		{F_19J,	 ST_U0},	
 		{F_REL,	 ST_U0},	
 		{F_REL,	 ST_U0},	
 		{F_REL,	 ST_U0},	
 		{F_REL,  ST_U0},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_RELCOMP
 */	
	{
		{F_NCNA, ST_U0},	
 		{F_01K,	 ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_19K,	 ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_RELCP,ST_U0},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_SETUPAK
 */	
 	{
 		{F_UEM,  ST_SUSE},	
 		{F_01L,	 ST_U3},	
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,  ST_SUSE},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_CALLPRC
 */	
 	{
 		{F_UEM,  ST_SUSE},	
 		{F_01M,	 ST_U3},	
 		{F_NCNA, ST_U3},	
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,  ST_SUSE},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_ALERT  
 */	
 	{
 		{F_UEM,  ST_SUSE},	
 		{F_01N,	 ST_U4},	
 		{F_03N,  ST_U4},	
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,  ST_SUSE},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_CONNECT
 */	
 	{
 		{F_UEM,  ST_SUSE},	
 		{F_01O,	 ST_U10},	
 		{F_03O,  ST_U10},	
 		{F_04O,  ST_U10},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,  ST_SUSE},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/* 
 * EV_PROGIND
 */	
 	{
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_03P,  ST_U3},	
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,  ST_SUSE},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_DISCONN
 */	
 	{
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_DISC, ST_U12},	
 		{F_DISC, ST_U12},	
 		{F_06Q,	 ST_U12},	
 		{F_DISC, ST_U12},	
 		{F_DISC, ST_U12},	
 		{F_DISC, ST_U12},	
 		{F_DISC, ST_U12},	
 		{F_11Q,	 ST_U19},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_DISC, ST_U12},	
 		{F_DISC, ST_U12},	
 		{F_DISC, ST_U12},	
 		{F_DISC, ST_U12},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_CONACK 
 */	
 	{
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_08R,	 ST_U10},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,	 ST_SUSE},	
 		{F_UEM,  ST_SUSE},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_STATENQ
 */	
 	{
 		{F_STENQ,ST_U0},	
 		{F_STENQ,ST_U1},	
 		{F_STENQ,ST_U3},	
 		{F_STENQ,ST_U4},	
 		{F_STENQ,ST_U6},	
 		{F_STENQ,ST_U7},	
 		{F_STENQ,ST_U8},	
 		{F_STENQ,ST_U9},	
 		{F_STENQ,ST_U10},	
 		{F_STENQ,ST_U11},	
 		{F_STENQ,ST_U12},	
 		{F_STENQ,ST_U19},	
 		{F_STENQ,ST_IWA},	
 		{F_STENQ,ST_IWR},	
 		{F_STENQ,ST_OW},	
 		{F_STENQ,ST_OW},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/* 
 * EV_INFO   
 */	
 	{
 		{F_UEM,  ST_SUSE},	
 		{F_UEM,  ST_SUSE},	
 		{F_INFO, ST_U3},	
 		{F_INFO, ST_U4},	
 		{F_UEM,	 ST_SUSE},	
 		{F_INFO, ST_U7},	
 		{F_INFO, ST_U8},	
 		{F_INFO, ST_U9},	
 		{F_INFO, ST_U10},	
 		{F_INFO, ST_U11},	
 		{F_INFO, ST_U12},	
 		{F_UEM,  ST_SUSE},	
 		{F_INFO, ST_IWA},	
 		{F_INFO, ST_IWR},	
 		{F_INFO, ST_OW},	
 		{F_INFO, ST_OW},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_FACILITY
 */	
 	{
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_FCTY, ST_SUSE},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}},
/* 
	STATE:	
		
		ST_U0			
		ST_U1			
		ST_U3			
		ST_U4			
		ST_U6			
		ST_U7			
		ST_U8			
		ST_U9			
		ST_U10			
		ST_U11			
		ST_U12			
		ST_U19			
		ST_IWA			
		ST_IWR			
		ST_OW			
		ST_IWL			
		ST_SUBSET		
		ST_ILL	      
*/
	{
/*
 * EV_T303EXP
 */		
		{F_ILL,  ST_ILL},	
 		{F_01U,	 ST_SUSE},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_T305EXP
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_11V,	 ST_U19},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_T308EXP
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_19W,	 ST_SUSE},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/* 
 * EV_T309EXP
 */	
 	{
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_309TO,ST_U0},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_T310EXP
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_03Y,  ST_U11},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_T313EXP
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_08Z,	 ST_U11},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/* STATE:	
		
		ST_U0			
		ST_U1			
		ST_U3			
		ST_U4			
		ST_U6			
		ST_U7			
		ST_U8			
		ST_U9			
		ST_U10			
		ST_U11			
		ST_U12			
		ST_U19			
		ST_IWA			
		ST_IWR			
		ST_OW			
		ST_IWL			
		ST_SUBSET		
		ST_ILL	      
*/	
	{
/*
 * EV_DLESTIN
 */		
		{F_ILL,  ST_ILL},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U3},	
 		{F_DLEI, ST_U4},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_DLEI, ST_U1},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_DLRELIN
 */	
 	{
 		{F_NCNA, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRIA,ST_U10},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_DLRI, ST_U0},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_DLESTCF
 */	
 	{
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF, ST_SUSE},	
 		{F_DECF2,ST_U8},	
 		{F_DECF3,ST_U0},	
 		{F_DECF1,ST_U1},	
 		{F_DECF4,ST_U7},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_DLRELCF
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	},
/*
 * EV_ILL    
 */	
 	{
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,  ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	 ST_ILL},	
 		{F_ILL,	ST_ILL},        
 		{F_ILL, ST_ILL}
 	}
};

/*---------------------------------------------------------------------------*
 *	event handler
 *---------------------------------------------------------------------------*/
void 
isdn_l3_next_state(struct isdn_bc *bc, int event)
{
	int curr, next;

	if (event > N_EVENTS)
		panic("i4b_l3fsm.c: event > N_EVENTS");

	curr = bc->bc_Q931_state;

	if (curr > N_STATES)
		panic("i4b_l3fsm.c: curr > N_STATES");

	next = isdn_l3_state_tab[event][curr].lst_next;

	if (next > N_STATES)
		panic("i4b_l3fsm.c: lst_next > N_STATES");

	NDBGL3(L3_F_MSG, "L3 FSM event [%s]: [%s => %s]",
		isdn_l3_event_text[event],
		isdn_l3_state_text[curr],
		isdn_l3_state_text[next]);
/* 
 * execute function 
 */
 	(*isdn_l3_state_tab[event][curr].func)(bc);

	if (lst_next == ST_ILL) {
		lst_next = curr;
		NDBGL3(L3_F_ERR, "FSM illegal state, state = %s, event = %s!",
				isdn_l3_state_text[next],
				isdn_l3_event_text[event]);
	}

	if (next != ST_SUSE)
		bc->bc_Q931_state = next;
}

/*---------------------------------------------------------------------------*
 *	return pointer to current state description
 *---------------------------------------------------------------------------*/
#if I4B_DEBUG
const char *
idsn_l3_print_state(struct isdn_bc *bc)
{
	return (isdn_l3_state_text[bc->bc_Q931_state]);
}
#endif

/*---------------------------------------------------------------------------*
 *	L3 FSM state U0 event L4 setup req
 *---------------------------------------------------------------------------*/
static void 
F_00A(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);

	if (bc->bc_sc->sc_l3.l3_dl_est == DL_DOWN) {
	
		isdn_l2_establish_req(bc->bc_sc);
		
		bc->bc_Q931_state = ST_OW;
	} else {
		isdn_l3_tx_setup(bc);
		bc->bc_Q931_state = ST_U1;
	}

	bc->bc_T303_first_to = 1;
	isdn_T303_start(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U0 event SETUP from L2
 *---------------------------------------------------------------------------*/
static void 
F_00H(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
/* 
 * tell l4 we have an incoming setup 
 */	
	i4b_l4_connect_ind(bc);	
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U0 event STATUS from L2
 *---------------------------------------------------------------------------*/
static void 
F_00I(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);

	if (bc->bc_call_state != 0) {
		bc->bc_cause_out = 101;
/* 
 * 1 = send cause 
 */		
		isdn_l3_tx_release_complete(bc, 1);	
	}
	bc->bc_Q931_state = ST_U0;
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U0 event RELEASE from L2
 *---------------------------------------------------------------------------*/
static void 
F_00J(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
/* 
 * 0 = don't send cause 
 */	
	isdn_l3_tx_release_complete(bc, 0);	
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U1 event disconnect req from L4
 *---------------------------------------------------------------------------*/
static void 
F_01B(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
/* 
 * cause from L4 
 */
	isdn_l3_tx_disconnect(bc);
	
	isdn_T303_stop(bc);
	isdn_T305_start(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U1 event RELEASE COMPLETE from L2
 *---------------------------------------------------------------------------*/
static void 
F_01K(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T303_stop(bc);
/* 
 * tell l4 we were rejected 
 */	
	i4b_l4_disconnect_ind(bc);	
	
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U1 event SETUP ACK from L2
 *---------------------------------------------------------------------------*/
static void 
F_01L(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T303_stop(bc);

	/*
	 * since this implementation does NOT support overlap sending,
	 * we react here as if we received a CALL PROCEEDING because
	 * several PBX's react with a SETUP ACK even if the called
	 * number is complete AND we sent a SENDING COMPLETE in the
	 * preceding SETUP message. (-hm)
	 */

	isdn_T310_start(bc);
	i4b_l4_proceeding_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U1 event CALL PROCEEDING from L2
 *---------------------------------------------------------------------------*/
static void 
F_01M(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T303_stop(bc);
	isdn_T310_start(bc);
	i4b_l4_proceeding_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U1 event ALERT from L2  (XXX !)
 *---------------------------------------------------------------------------*/
static void 
F_01N(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T303_stop(bc);
	i4b_l4_alert_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U1 event CONNECT from L2 (XXX !)
 *---------------------------------------------------------------------------*/
static void 
F_01O(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T303_stop(bc);
	isdn_l3_tx_connect_ack(bc);
	i4b_l4_connect_active_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U1 event T303 timeout
 *---------------------------------------------------------------------------*/
static void 
F_01U(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	
	if (bc->bc_T303_first_to == 1) {
		bc->bc_T303_first_to = 0;
		isdn_l3_tx_setup(bc);
		isdn_T303_start(bc);
		bc->bc_Q931_state = ST_U1;
	} else {
		i4b_l4_disconnect_ind(bc);
		isdn_bc_free(bc);
		bc->bc_Q931_state = ST_U0;
	}
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U3 event release req from L4
 *---------------------------------------------------------------------------*/
static void 
F_03C(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T310_stop(bc);
	bc->bc_cause_out = 6;
	isdn_l3_tx_release(bc, 1);	/* 0 = don't send cause */
	bc->bc_T308_first_to = 1;
	isdn_T308_start(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U3 event ALERT from L2
 *---------------------------------------------------------------------------*/
static void 
F_03N(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T310_stop(bc);
	i4b_l4_alert_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U3 event CONNECT from L2
 *---------------------------------------------------------------------------*/
static void 
F_03O(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T310_stop(bc);
	isdn_l3_tx_connect_ack(bc);	/* CONNECT ACK to network */
	i4b_l4_connect_active_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U3 event PROGESS IND from L2
 *---------------------------------------------------------------------------*/
static void 
F_03P(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T310_stop(bc);
#ifdef NOTDEF
	i4b_l4_progress_ind(bc);
#endif
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U3 event T310 timeout
 *---------------------------------------------------------------------------*/
static void 
F_03Y(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	bc->bc_cause_out = 102;	/* recovery on timer expiry */
	isdn_l3_tx_disconnect(bc);
	isdn_T305_start(bc);
	i4b_l4_disconnect_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U4 event CONNECT from L2
 *---------------------------------------------------------------------------*/
static void 
F_04O(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_connect_ack(bc);	/* CONNECT ACK to network */
	i4b_l4_connect_active_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U6 event alert req from L4
 *---------------------------------------------------------------------------*/
static void 
F_06D(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);

	if (bc->bc_sc->sc_l3.l3_dl_est == DL_DOWN) {
		isdn_l2_establish_req(bc->bc_sc);
		bc->bc_Q931_state = ST_IWL;
	} else {
		isdn_l3_tx_alert(bc);
		bc->bc_Q931_state = ST_U7;
	}
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U6 event incoming setup accept from L4
 *---------------------------------------------------------------------------*/
static void 
F_06E(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);

	if (bc->bc_sc->sc_l3.l3_dl_est == DL_DOWN) {
		isdn_l2_establish_req(bc->bc_sc);
		bc->bc_Q931_state = ST_IWA;
	} else {
		isdn_l3_tx_connect(bc);
		bc->bc_Q931_state = ST_U8;
	}
	isdn_313_start(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U6 event incoming setup reject from L4
 *---------------------------------------------------------------------------*/
static void 
F_06F(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);

	if (bc->bc_sc->sc_l3.l3_dl_est == DL_DOWN) {
		isdn_l2_establish_req(bc->bc_sc);
		bc->bc_Q931_state = ST_IWR;
	} else {
		isdn_l3_tx_release_complete(bc, 1);
		bc->bc_Q931_state = ST_U0;
		isdn_bc_free(bc);
	}
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U6 event incoming setup ignore from L4
 *---------------------------------------------------------------------------*/
static void 
F_06G(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U6 event RELEASE from L2
 *---------------------------------------------------------------------------*/
static void 
F_06J(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_release_complete(bc, 0);
	i4b_l4_disconnect_ind(bc);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U6 event DISCONNECT from L2
 *---------------------------------------------------------------------------*/
static void 
F_06Q(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	i4b_l4_disconnect_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U7 event setup response accept from L4
 *---------------------------------------------------------------------------*/
static void 
F_07E(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_connect(bc);
	isdn_313_start(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U7 event setup response reject from L4
 *---------------------------------------------------------------------------*/
static void 
F_07F(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_release_complete(bc, 1);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U7 event setup response ignore from L4
 *---------------------------------------------------------------------------*/
static void 
F_07G(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U8 event CONNECT ACK from L2
 *---------------------------------------------------------------------------*/
static void 
F_08R(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_313_stop(bc);
	i4b_l4_connect_active_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U8 event T313 timeout
 *---------------------------------------------------------------------------*/
static void 
F_08Z(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	bc->bc_cause_out = 102;	/* recovery on timer expiry */
	isdn_l3_tx_disconnect(bc);
	isdn_T305_start(bc);
	i4b_l4_disconnect_ind(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U9 event alert req from L4
 *---------------------------------------------------------------------------*/
static void 
F_09D(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_alert(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U9 event setup response accept from L4
 *---------------------------------------------------------------------------*/
static void 
F_09E(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_connect(bc);
	isdn_313_start(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U9 event setup response reject from L4
 *---------------------------------------------------------------------------*/
static void 
F_09F(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_release_complete(bc, 1);
	isdn_bc_free(bc);
}
/*---------------------------------------------------------------------------*
 *	L3 FSM state U9 event setup response ignore from L4
 *---------------------------------------------------------------------------*/
static void 
F_09G(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U11 event RELEASE from L2
 *---------------------------------------------------------------------------*/
static void 
F_11J(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	T305_stop(bc);
	isdn_l3_tx_release_complete(bc, 0);
	i4b_l4_disconnect_ind(bc);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U11 event DISCONNECT from L2
 *---------------------------------------------------------------------------*/
static void 
F_11Q(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	T305_stop(bc);
	isdn_l3_tx_release(bc, 0);
	bc->bc_T308_first_to = 1;
	isdn_T308_start(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U11 event T305 timeout
 *---------------------------------------------------------------------------*/
static void 
F_11V(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	bc->bc_cause_out = 102;
	isdn_l3_tx_release(bc, 1);
	bc->bc_T308_first_to = 1;
	isdn_T308_start(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U12 event release req from L4
 *---------------------------------------------------------------------------*/
static void 
F_12C(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_release(bc, 1);
	bc->bc_T308_first_to = 1;
	isdn_T308_start(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U12 event RELEASE from L2
 *---------------------------------------------------------------------------*/
static void 
F_12J(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_release_complete(bc, 0);
	i4b_l4_disconnect_ind(bc);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U19 event STATUS from L2
 *---------------------------------------------------------------------------*/
static void 
F_19I(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);

	if (bc->bc_call_state == 0) {
		i4b_l4_status_ind(bc);
		isdn_bc_free(bc);
		bc->bc_Q931_state = ST_U0;
	} else 
		bc->bc_Q931_state = ST_U19;
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U19 event RELEASE from L2
 *---------------------------------------------------------------------------*/
static void 
F_19J(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T308_stop(bc);
	i4b_l4_disconnect_ind(bc);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U19 event RELEASE COMPLETE from L2
 *---------------------------------------------------------------------------*/
static void 
F_19K(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_T308_stop(bc);
	i4b_l4_disconnect_ind(bc);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U19 event T308 timeout
 *---------------------------------------------------------------------------*/
static void 
F_19W(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	
	if (bc->bc_T308_first_to == 0) {
		bc->bc_T308_first_to = 1;
		isdn_l3_tx_release(bc, 0);
		isdn_T308_start(bc);
		bc->bc_Q931_state = ST_U19;
	} else {
		bc->bc_T308_first_to = 0;
		i4b_l4_disconnect_ind(bc);
		isdn_bc_free(bc);
		bc->bc_Q931_state = ST_U0;
	}
}

/*---------------------------------------------------------------------------*
 *	L3 FSM routine no change no action
 *---------------------------------------------------------------------------*/
static void 
F_NCNA(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM any state event STATUS ENQ from L2
 *---------------------------------------------------------------------------*/
static void 
F_STENQ(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_status(bc, CAUSE_Q850_STENQRSP); /* 30, resonse to stat enq */
}

/*---------------------------------------------------------------------------*
 *	L3 FSM any state except 0 & 19 event STATUS from L2
 *---------------------------------------------------------------------------*/
static void 
F_STAT(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	
	if (bc->bc_call_state == 0) {
		i4b_l4_status_ind(bc);
		bc->bc_Q931_state = ST_U0;
		isdn_bc_free(bc);
	} else {		
/* 
 * XXX !!!!!!!!!!!!!!!!!! 
 */
		i4b_l4_status_ind(bc);
		bc->bc_cause_out = 101;	/* message not compatible with call state */
		isdn_l3_tx_disconnect(bc);
		isdn_T305_start(bc);
		bc->bc_Q931_state = ST_U11;
	}
}

/*---------------------------------------------------------------------------*
 *	L3 FSM some states event INFORMATION from L2
 *---------------------------------------------------------------------------*/
static void 
F_INFO(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	i4b_l4_info_ind(bc);
	/* remain in current state */
}

/*---------------------------------------------------------------------------*
 *	L3 FSM some states event RELEASE COMPLETE from L2
 *---------------------------------------------------------------------------*/
static void 
F_RELCP(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_stop_all_timers(bc);
	i4b_l4_disconnect_ind(bc);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM some states event RELEASE from L2
 *---------------------------------------------------------------------------*/
static void 
F_REL(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_stop_all_timers(bc);
	isdn_l3_tx_release_complete(bc, 0);
	i4b_l4_disconnect_ind(bc);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM some states event DISCONNECT from L2
 *---------------------------------------------------------------------------*/
static void 
F_DISC(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_stop_all_timers(bc);

	/*
	 * no disconnect ind to L4, no jump to state U12
	 * instead we issue a RELEASE and jump to U19
	 */

	isdn_l3_tx_release(bc, 0);
	bc->bc_T308_first_to = 1;
	isdn_T308_start(bc);
	bc->bc_Q931_state = ST_U19;
}

/*---------------------------------------------------------------------------*
 *	L3 FSM some states event disconnect request from L4
 *---------------------------------------------------------------------------*/
static void 
F_DCRQ(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);

	/* stop T310 in case this is the result of an incoming call for a */
	/* calledback connection */

	if (bc->bc_T310 == TIMER_ACTIVE)
		isdn_T310_stop(bc);

	/* cause from L4 */
	isdn_l3_tx_disconnect(bc);
	isdn_T305_start(bc);
	bc->bc_Q931_state = ST_U11;
}

/*---------------------------------------------------------------------------*
 *	L3 FSM any state except 0 event unexpected message from L2
 *---------------------------------------------------------------------------*/
static void 
F_UEM(struct isdn_bc *bc)
{
	NDBGL3(L3_F_ERR, "%s executing, state = %s", 
		__func__, idsn_l3_print_state(bc));
/* 
 * 101, message not compatible with call state 
 */		
	isdn_l3_tx_status(bc, CAUSE_Q850_MSGNCWCS); 
}

/*---------------------------------------------------------------------------*
 *	L3 FSM any state except 0 event SETUP from L2
 *---------------------------------------------------------------------------*/
static void 
F_SIGN(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);

	(void)cd;
/* XXX */ /* isdn_bc_free(bc); ?????????? XXX */
}

/*---------------------------------------------------------------------------*
 *	L3 FSM relevant states event DL ESTABLISH IND from L2
 *---------------------------------------------------------------------------*/
static void 
F_DLEI(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);

/* XXX */

	/* remain in current state */
}

/*---------------------------------------------------------------------------*
 *	L3 FSM any state event illegal event occurred
 *---------------------------------------------------------------------------*/
static void 
F_ILL(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM any state event T309 timeout
 *---------------------------------------------------------------------------*/
static void 
F_309TO(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);

	i4b_l4_dl_fail_ind(bc);

	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM any state event FACILITY message received
 *---------------------------------------------------------------------------*/
static void 
F_FCTY(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	/* ST_SUSE, no change in state ! */
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state ST_OW event DL ESTABLISH CONF from L2
 *---------------------------------------------------------------------------*/
static void 
F_DECF1(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_setup(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state ST_IWA event DL ESTABLISH CONF from L2
 *---------------------------------------------------------------------------*/
static void 
F_DECF2(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_connect(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state ST_IWR event DL ESTABLISH CONF from L2
 *---------------------------------------------------------------------------*/
static void 
F_DECF3(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_release_complete(bc, 1);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state ST_IWL event DL ESTABLISH CONF from L2
 *---------------------------------------------------------------------------*/
static void 
F_DECF4(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_tx_alert(bc);
}


/*---------------------------------------------------------------------------*
 *	L3 FSM any state event DL ESTABLISH CONF from L2
 *---------------------------------------------------------------------------*/
static void 
F_DECF(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	T309_stop(bc);
	isdn_l3_tx_status(bc, CAUSE_Q850_NORMUNSP); /* 31, normal unspecified */
}

/*---------------------------------------------------------------------------*
 *	L3 FSM any state except U10 event DL RELEASE IND from L2
 *---------------------------------------------------------------------------*/
static void 
F_DLRI(struct isdn_bc *bc)
{
	NDBGL3(L3_F_MSG, "%s executing", __func__);
	isdn_l3_stop_all_timers(bc);
	i4b_l4_disconnect_ind(bc);
	isdn_bc_free(bc);
}

/*---------------------------------------------------------------------------*
 *	L3 FSM state U10 event DL RELEASE IND from L2
 *---------------------------------------------------------------------------*/
static void 
F_DLRIA(struct isdn_bc *bc)
{
	struct isdn_l2 * l2 = (l2_softc_t*)bc->bc_l3drv->l1_token;
	NDBGL3(L3_F_MSG, "%s executing", __func__);

	if (bc->bc_T309 == TIMER_IDLE)
		T309_start(bc);

	isdn_l2_establish_req(bc->bc_sc);
}
