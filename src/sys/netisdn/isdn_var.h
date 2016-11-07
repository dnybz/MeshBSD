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

#ifndef _NETISDN_ISDN_VAR_H_
#define _NETISDN_ISDN_VAR_H_

#ifdef _KERNEL

#include <sys/queue.h>

struct isdn_softc;

/*
 *	Call / connection on one B-channel and all its parameters.
 */
struct isdn_b_chan {
	struct isdn_softc 	*bc_sc; 
	
	int	bc_cr;			/* call reference value		*/

	int	bc_cr_flag;			/* call reference flag		*/
#define CRF_ORIG	0		/* originating side		*/
#define CRF_DEST	1		/* destinating side		*/

	int	bc_id;		/* channel id value		*/
	int	bc_excl;		/* channel exclusive		*/

	int	bc_proto;			/* B channel protocol BPROT_XXX */

	cause_t	bc_cause_in;		/* cause value from NT	*/
	cause_t	bc_cause_out;		/* cause value to NT	*/

	int	bc_call_state;		/* from incoming SETUP	*/

	struct sockaddr_e167 	bc_dst; 	
	struct sockaddr_e167 	bc_src;
	
	int	bc_scr_ind;		/* screening ind for incoming call */
	int	bc_prs_ind;		/* presentation ind for incoming call */
	int	bc_type_plan;		/* type and plan for incoming number */

	int	bc_Q931state;		/* Q.931 state for call	*/
	int	bc_event;			/* event to be processed */

	int	bc_response;		/* setup response type	*/

	int	bc_T303;			/* SETUP sent response timeout	*/
	int	bc_T303_first_to;		/* first timeout flag		*/

	int	bc_T305;			/* DISC without PROG IND	*/

	int	bc_T308;			/* RELEASE sent response timeout*/
	int	bc_T308_first_to;		/* first timeout flag		*/

	int	bc_T309;			/* data link disconnect timeout	*/

	int	bc_T310;			/* CALL PROC received		*/

	int	bc_T313;			/* CONNECT sent timeout		*/

	int	bc_T400;			/* L4 timeout */

	int	bc_dir;			/* outgoing or incoming call	*/
#define DIR_OUTGOING	0
#define DIR_INCOMING	1

	int	bc_timeout_active;		/* idle timeout() active flag	*/
/*
 * XXX: I'll refactor it by rplacment of callout_handle(9).
 */
	struct	callout	bc_idle_timeout_handle;
	struct	callout	bc_T303_callout;
	struct	callout	bc_T305_callout;
	struct	callout	bc_T308_callout;
	struct	callout	bc_T309_callout;
	struct	callout	bc_T310_callout;
	struct	callout	bc_T313_callout;
	struct	callout	bc_T400_callout;
	int	bc_callouts_inited;		/* must init before use */

	int	bc_idletime_state;		/* wait for idle_time begin	*/
#define IST_IDLE	0	/* shorthold mode disabled 	*/
#define IST_NONCHK	1	/* in non-checked window	*/
#define IST_CHECK	2	/* in idle check window		*/
#define IST_SAFE	3	/* in safety zone		*/

	time_t	bc_idletimechk_start;	/* check idletime window start	*/
	time_t	bc_connect_time;		/* time connect was made	*/
	time_t	bc_last_active_time;	/* last time with activity	*/

					/* for incoming connections:	*/
	time_t	bc_max_idle_time;		/* max time without activity	*/

					/* for outgoing connections:	*/
	msg_shorthold_t bc_shorthold_data;	/* shorthold data to use */

	int	bc_aocd_flag;		/* AOCD used for unitlength calc*/
	time_t	bc_last_aocd_time;		/* last time AOCD received	*/
	
	int	bc_units;			/* number of AOCD charging units*/
	int	bc_units_type;		/* units type: AOCD, AOCE	*/
	int	bc_cunits;			/* calculated units		*/

	int	bc_isdntxdelay;		/* isdn tx delay after connect	*/

	uint8_t	bc_display[ISDN_DISPLAY_MAX];	/* display information element	*/
	char	bc_datetime[ISDN_DATETIME_MAX];	/* date/time information element*/
};

/*
 * Software context.
 */
struct isdn_softc {
	struct ifnet 	*sc_ifp; 	
	struct isdn_l2 	sc_l2;
	struct isdn_l3 	sc_l3;
};

/*
 * Denotes AF_ISDN partition on domain family..
 */ 
struct isdn_ifinfo {
	struct lltable		*iii_llt;	/* isdn_arp cache */
	struct isdn_softc 	*iii_sc;
};
#define ISDN_LLTABLE(ifp)	\
	(((struct isdn_ifinfo *)(ifp)->if_afdata[AF_ISDN])->iii_llt)
#define ISDN_SOFTC(ifp) \
	(((struct isdn_ifinfo *)(ifp)->if_afdata[AF_ISDN])->iii_sc)

/*
 * Describes an ISDN channel on IEEE802.{3,11} link-layer.
 */
struct isdn_ifaddr {
	struct ifaddr 	ii_ifa;		/* protocol-independent info */
#define ii_addr 	ii_ifa.ifa_addr
#define ii_netmask 	ii_ifa.ifa_netmask
#define ii_dstaddr 	ii_ifa.ifa_dstaddr
#define ii_ifp 	ii_ifa.ifa_ifp	
#define ii_flags 	ii_ifa.ifa_flags
#define ii_metric 	ii_ifa.ifa_metric
	TAILQ_ENTRY(isdn_ifaddr)	ii_link;
/* 
 * < channel, proto ,sapi, tei >  
 */	
	struct sockaddr_isdn 	ii_seg;	
/* 
 * < telno, subaddr > 
 */
	struct sockaddr_e167 	ii_no; 	 
};

#endif /* _KERNEL */
#endif /* _NETISDN_ISDN_VAR_H_ */
