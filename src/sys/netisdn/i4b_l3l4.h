/*	$NetBSD: i4b_l3l4.h,v 1.17 2005/12/10 23:51:50 elad Exp $	*/

/*
 * Copyright (c) 1997, 1999 Hellmuth Michaelis. All rights reserved.
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
 *	i4b_l3l4.h - layer 3 / layer 4 interface
 *	------------------------------------------
 *
 *	$Id: i4b_l3l4.h,v 1.17 2005/12/10 23:51:50 elad Exp $
 *
 * $FreeBSD$
 *
 *	last edit-date: [Fri Jun  2 14:29:35 2000]
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
 
#ifndef _NETISDN_I4B_L3L4_H_
#define _NETISDN_I4B_L3L4_H_

#define T303VAL	(hz*4)			/* 4 seconds timeout		*/
#define T305VAL	(hz*30)			/* 30 seconds timeout		*/
#define T308VAL	(hz*4)			/* 4 seconds timeout		*/
#define T309VAL	(hz*90)			/* 90 seconds timeout		*/
#define T310VAL	(hz*60)			/* 30-120 seconds timeout	*/
#define T313VAL	(hz*4)			/* 4 seconds timeout		*/
#define T400DEF	(hz*10)			/* 10 seconds timeout		*/

#define MAX_BCHAN	2
#define N_CALL_DESC	(20*(MAX_BCHAN)) /* XXX: make resizable */

struct isdn_bch_stats {
	int bch_obytes;
	int bch_ibytes;
};

/*---------------------------------------------------------------------------*
 * table of things the driver needs to know about the b channel
 * it is connected to for data transfer
 *---------------------------------------------------------------------------*/
struct isdn_bch_linktab {
	void *bch_l1token;
	int bch_channel;
	
	struct isdn_l4_bch *bch_sap;
/*
 * XXX: ... 
 */	
	struct ifqueue *bch_tx_queue;
	struct ifqueue *bch_rx_queue;	/* data xfer for NON-HDLC traffic   */
	struct mbuf **rx_mbuf;		/* data xfer for HDLC based traffic */
};

struct isdn_l4_sap;
struct isdn_l3;

/*---------------------------------------------------------------------------*
 *	this structure describes one call/connection on one B-channel
 *	and all its parameters
 *---------------------------------------------------------------------------*/
struct isdn_call_desc {
	u_int	cd_id;			/* call descriptor id		*/
	
	int 	cd_l3_id;
	struct isdn_l3 *cd_l3;
	
	int	cd_cr;			/* call reference value		*/

	int	cd_cr_flag;			/* call reference flag		*/
#define CRF_ORIG	0		/* originating side		*/
#define CRF_DEST	1		/* destinating side		*/

	int	cd_ch_id;		/* channel id value		*/
	int	cd_ch_excl;		/* channel exclusive		*/

	int	cd_bch_proto;			/* B channel protocol BPROT_XXX */

	int	cd_bch_sap_index;	/* sap to use for B channel	*/
	int	cd_bch_sap_unit;	/* unit for above sap number	*/

	cause_t	cd_cause_in;		/* cause value from NT	*/
	cause_t	cd_cause_out;		/* cause value to NT	*/

	int	cd_call_state;		/* from incoming SETUP	*/
/*
 * XXX; candidates for attributes on sockaddr_isdn{}
 */
	u_char	cd_dst_telno[TELNO_MAX];	/* destination number	*/
	u_char	cd_src_telno[TELNO_MAX];	/* source number	*/
	u_char	cd_src_subaddr[SUBADDR_MAX];
	u_char	cd_dest_subaddr[SUBADDR_MAX];

	int	cd_scr_ind;		/* screening ind for incoming call */
	int	cd_prs_ind;		/* presentation ind for incoming call */
	int	cd_type_plan;		/* type and plan for incoming number */

	int	cd_Q931state;		/* Q.931 state for call	*/
	int	cd_event;			/* event to be processed */

	int	cd_response;		/* setup response type	*/

	int	cd_T303;			/* SETUP sent response timeout	*/
	int	cd_T303_first_to;		/* first timeout flag		*/

	int	cd_T305;			/* DISC without PROG IND	*/

	int	cd_T308;			/* RELEASE sent response timeout*/
	int	cd_T308_first_to;		/* first timeout flag		*/

	int	cd_T309;			/* data link disconnect timeout	*/

	int	cd_T310;			/* CALL PROC received		*/

	int	cd_T313;			/* CONNECT sent timeout		*/

	int	cd_T400;			/* L4 timeout */

	isdn_link_t	*cd_ilt;		/* isdn B channel driver/state	*/
	struct isdn_l4_sap *cd_l4_sap; 	/* Interface L4 Service Primitves */
	void	*cd_l4_softc; 	/* Software COntext L4	*/

	int	cd_dir;			/* outgoing or incoming call	*/
#define DIR_OUTGOING	0
#define DIR_INCOMING	1

	int	cd_timeout_active;		/* idle timeout() active flag	*/
/*
 * XXX: I'll refactor it by rplacment of callout_handle(9).
 */
	struct	callout	cd_idle_timeout_handle;
	struct	callout	cd_T303_callout;
	struct	callout	cd_T305_callout;
	struct	callout	cd_T308_callout;
	struct	callout	cd_T309_callout;
	struct	callout	cd_T310_callout;
	struct	callout	cd_T313_callout;
	struct	callout	cd_T400_callout;
	int	cd_callouts_inited;		/* must init before use */

	int	cd_idletime_state;		/* wait for idle_time begin	*/
#define IST_IDLE	0	/* shorthold mode disabled 	*/
#define IST_NONCHK	1	/* in non-checked window	*/
#define IST_CHECK	2	/* in idle check window		*/
#define IST_SAFE	3	/* in safety zone		*/

	time_t	cd_idletimechk_start;	/* check idletime window start	*/
	time_t	cd_connect_time;		/* time connect was made	*/
	time_t	cd_last_active_time;	/* last time with activity	*/

					/* for incoming connections:	*/
	time_t	cd_max_idle_time;		/* max time without activity	*/

					/* for outgoing connections:	*/
	msg_shorthold_t cd_shorthold_data;	/* shorthold data to use */

	int	cd_aocd_flag;		/* AOCD used for unitlength calc*/
	time_t	cd_last_aocd_time;		/* last time AOCD received	*/
	int	cd_units;			/* number of AOCD charging units*/
	int	cd_units_type;		/* units type: AOCD, AOCE	*/
	int	cd_cunits;			/* calculated units		*/

	int	cd_isdntxdelay;		/* isdn tx delay after connect	*/

	u_char	cd_display[DISPLAY_MAX];	/* display information element	*/
	char	cd_datetime[DATETIME_MAX];	/* date/time information element*/
};

extern struct isdn_call_desc call_desc[];
extern int num_call_desc;

/*
 * Set of funcs layer 4 drivers calls to manipulate the B channel
 * they are using.
 */
struct isdn_l4_bch {
	void (*l4_bch_config)(void *, int, int, int);
	void (*l4_bch_tx_start)(void *, int);
	void (*l4_bch_stat)(void *, int, bch_stats_t *);
};

/*
 * Functions a layer 4 application driver exports
 *
 * XXX: well... I'll map those into AF_ISDN on socket-layer 
 */
struct isdn_l4_sap {
	
/*
 * Functions for use by the B channel driver
 */
	void 	(*l4_bch_rx_data_ready)(void *);
	void 	(*l4_bch_tx_queue_empty)(void *);
	void 	(*l4_bch_activity)(void *, int);
#define ACT_RX 0
#define ACT_TX 1
	void 	(*l4_line_connected)(void *, void *);
	void 	(*l4_line_disconnected)(void *, void *);
	void 	(*l4_dial_response)(void *, int, cause_t);
	void 	(*l4_updown_ind)(void *, int);
	/*
	 * Functions used by the ISDN management system
	 */
	void * 	(*l4_get_softc)(int);
	void 	(*l4_set_linktab)(void *, isdn_link_t *);
	/*
	 * Optional accounting function
	 */
	time_t (*l4_get_idletime)(void *);
};

/* global registry of layer 4 drivers */
int 	isdn_l4_attach(const char *, int, struct isdn_l4_sap *);
int 	isdn_l4_detatch(const char *);
int 	isdn_l4_find_ifid(const char *);

struct isdn_l4_sap * 	isdn_l4_find_sap(const char *, int);
struct isdn_l4_sap * 	isdn_l4_get_sap(int, int);

/* forward decl. */
struct isdn_diagnostic_request;
struct isdn_dr_prot;

/*
 * funcs exported by a layer 3 driver to layer 4 
 */
struct isdn_l3_sap {
	
	isdn_link_t* (*get_linktab)(void *, int);
	
	void (*set_l4_sap)(void *, int, 
		struct isdn_l4_sap *, void *);

	void	(*N_CONNECT_REQUEST)	(struct call_desc *);
	void	(*N_CONNECT_RESPONSE)	(struct call_desc *, int, int);
	void	(*N_DISCONNECT_REQUEST)	(struct call_desc *, int);
	void	(*N_ALERT_REQUEST)	(struct call_desc *
	void	(*N_MGMT_COMMAND)	(struct isdn_l3 *, int, void *);
};

/*---------------------------------------------------------------------------*
 *	This structure "describes" one ISDN interface (typically identical
 * 	to one controller, but when one controller drives multiple ISDN's,
 *	this is just one of those ISDN's).
 *
 *	An ISDN can be either a Basic Rate Interface (BRI, 2 B channels)
 *	or a Primary Rate Interface (PRI, 30 B channels).
 *---------------------------------------------------------------------------*/
struct isdn_l3 {
	SLIST_ENTRY(isdn_l3) l3_q;
/*
 * XXX: I'll refactor this... to provide a homomorphisnen on ifnet(9).
 */	
	void 	*l3_l2;		/* l2_softc (!!) , e. g. struct isdn_iifinfo{} */
	int	l3_id;			/* ISDN id assigned to this */
	char l3_xname[IFNAMSIZ];			/* pointer to autoconf identifier */
					/* e.g. "re0" or "ath0", etc.pp */

	int	l3_dch_proto;		/* D-channel protocol type */

	int	l3_dl_est;			/* layer 2 established	*/
#define DL_DOWN	0
#define DL_UP	1

	int	l3_nbch;			/* number of B-channels */
	int	l3_bch_state[NBCH_BRI];		/* states of the nbch b channels */
#define BCH_ST_FREE	0		/* free to be used, idle */
#define BCH_ST_RSVD	1		/* reserved, may become free or used */
#define BCH_ST_USED	2		/* in use for data transfer */

	int	l3_tei;			/* current tei or -1 if invalid */
/* 
 * By L4 called Service Access Point 
 */
	struct isdn_l3_sap *l3_sap;
};

#define NBCH_BRI 2
#define NBCH_PRI 30

void 	i4b_l4_contr_ev_ind(int , int);
struct isdn_l3 * 	i4b_attach(const char *,
    const char *, void *,
    struct isdn_l3_sap *, int);
int 	i4b_detach(struct isdn_l3 *);
void 	isdn_isdnif_ready(int);
struct isdn_l3 * 	isdn_get_l3_by_id(int);
int 	isdn_count_isdnif(int *);

#endif /* !_NETISDN_I4B_Q931_H_ */
