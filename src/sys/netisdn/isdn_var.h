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
 
#ifndef _NETISDN_ISDN_VAR_H_
#define _NETISDN_ISDN_VAR_H_

#ifdef _KERNEL

#include <sys/queue.h>

/*
 * Callout handling.
 */

#define TIMEOUT_FUNC_T	timeout_t *
#define SECOND		time_second
#define MICROTIME(x)	getmicrotime(&(x))

#define START_TIMER(XHANDLE, XF, XSC, XTIME) \
	XHANDLE = timeout((TIMEOUT_FUNC_T)XF, (void *)XSC, XTIME)
#define	STOP_TIMER(XHANDLE, XF, XSC)	\
	untimeout((TIMEOUT_FUNC_T)XF, (void*)XSC, XHANDLE)

#define TIMER_IDLE	1		/* a timer is running	*/
#define TIMER_ACTIVE	2		/* a timer is idle	*/

/*
 *	Call / connection on one B-channel and all its parameters.
 */
struct isdn_b_chan {
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
	struct callout_handle	bc_idle_timeout_handle;
	struct callout_handle	bc_T303_callout;
	struct callout_handle	bc_T305_callout;
	struct callout_handle	bc_T308_callout;
	struct callout_handle	bc_T309_callout;
	struct callout_handle	bc_T310_callout;
	struct callout_handle	bc_T313_callout;
	struct callout_handle	bc_T400_callout;
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

struct isdn_l2 {
	int	l2_Q921_state;	/* state according to Q.921 */

	u_char	l2_last_ril;	/* last reference number from TEI management */
	u_char	l2_last_rih;

	int	l2_tei_valid;	/* tei is valid flag */
#define TEI_INVALID	0
#define TEI_VALID	1
	int	l2_tei;		/* tei, if tei flag valid */

	int	l2_ph_active;	/* Layer 1 active flag */
#define PH_INACTIVE	0	/* layer 1 inactive */
#define PH_ACTIVEPEND	1	/* already tried to activate */
#define PH_ACTIVE	2	/* layer 1 active */

	int	l2_T200;		/* Multiframe timeout timer */
	int	l2_T201;		/* min time between TEI ID check */
	int	l2_T202;		/* min time between TEI ID Req messages */
	int	l2_N202;		/* TEI ID Req tx counter */
/* 
 * call-back function, when T202 expires 
 */	
	void 	(*l2_T202_fn)(void *);
	
	int	l2_T203;		/* max line idle time */
	
	struct callout_handle 	l2_T200_callout;
	struct callout_handle 	l2_T202_callout;
	struct callout_handle 	l2_T203_callout;
	struct callout_handle 	l2_IFQU_callout;
/*
 * isdn_l2_queue_i_frame: 
 *
 * 	value of IFQU_DLY some experimentation Gary did showed 
 * 	a minimal value of (hz/20) was possible to let this work, 
 * 	Gary suggested using (hz/10) but i settled down to using 
 * 	(hz/5) for now (-hm).
 */
#define IFQU_DLY (hz/5)		/* reschedule I-FRAME-QUEUED-UP 0.2 sec */

	int	l2_vr;		/* receive sequence frame counter */
	int	l2_vs;		/* transmit sequence frame counter */
	int	l2_va;		/* acknowledge sequence frame counter */

	int	l2_ack_pend;	/* acknowledge pending */
	int	l2_rej_excpt;	/* reject exception */
	int	l2_peer_busy;	/* peer receiver busy */
	int	l2_own_busy;	/* own receiver busy */
	int	l2_l3_init;	/* layer 3 initiated */
/*
 * XXX: might be merged in isdn_b_chan{}
 */
	int	l2_b_chan_state[2];

	struct ifqueue l2_i_queue;	/* queue of outgoing i frames */
#define IQUEUE_MAXLEN	20

/* 
 * XXX: this implementation only supports a k-value of 1 !!! 
 */
	struct mbuf *l2_ua_frame;	/* last unacked frame */
	
	int	l2_ua_num;		/* last unacked frame number */
#define UA_EMPTY (-1)		/* ua_frame is unused	*/

	int	l2_rxd_CR;		/* received Command Response bit */
	int	l2_rxd_PF;		/* received Poll/Final bit */
	int	l2_rxd_NR;		/* received N(R) field */
	int	l2_RC;		/* Retry Counter */

	int	l2_iframe_sent;	/* check if i frame acked by another i frame */
/* 
 * function to be called at fsm exit 
 */
	int 	(*l2_post_fsm_fn)(struct isdn_softc *);	
/* 
 * statistics 
 */
	lapdstat_t	l2_stat;	/* lapd protocol statistics */
};

/* Q.912 system parameters (Q.921 03/93 pp 43) */

#define MAX_K_VALUE	1	/* BRI - # of outstanding frames 	*/

#define N200	3		/* max no of retransmissions */
#define N201DEF	260		/* max no of octetts in information field */
#define N202DEF	3		/* max no of TEI ID Request message transmissions */

#define T200DEF	(hz*1)		/* default T200 timer value = 1 second	*/
#define T201DEF	T200DEF		/* default T201 timer value = T200DEF	*/
#define T202DEF (hz*2)		/* default T202 timer value = 2 seconds */
#define T203DEF (hz*10)		/* default T203 timer value = 10 seconds*/

/* modulo 128 operations */

#define M128INC(v) 	(v)++;		\
			if((v)>127)	\
			{		\
				v = 0;	\
			}

#define M128DEC(v) 	(v)--;		\
			if((v)<0)	\
			{		\
				v = 127;\
			}

/* P-bit values */

typedef enum {
	P0,
	P1
} pbit_t;

/* F-bit values */

typedef enum {
	F0,
	F1
} fbit_t;

/* CR-bit values to NT */

typedef enum {
	CR_CMD_TO_NT,
	CR_RSP_TO_NT
} crbit_to_nt_t;

/* CR-bit values from NT */

typedef enum {
	CR_RSP_FROM_NT,
	CR_CMD_FROM_NT
} crbit_from_nt_t;

/* address field - octett 2 */

#define OFF_SAPI	0	/* SAPI offset, HDLC flag is eaten by L1 */
#define SAPI_CCP	0	/* SAPI = 0 - call control procedures */
#define SAPI_X25	16	/* SAPI = 16 - X.25 packet procedures */
#define SAPI_L2M	63	/* SAPI = 63 - Layer 2 management procedures */

/* extract and insert macros for SAPI octett */

#define GETSAPI(octett)		(((octett) >> 2) & 0x3f)
#define PUTSAPI(sapi,cr,octett)	\
	((octett) = (((sapi << 2) & 0xfc) | ((cr & 0x01) << 1)))
#define GETCR(octett)		(((octett) >> 1) & 0x01)
#define GETEA(octett)		((octett) & 0x01)

/* address field - octett 3 */

#define OFF_TEI		1	/* TEI offset */
#define GETTEI(octett) (((octett) >> 1) & 0x7f)
#define PUTTEI(tei, octett) \
	((octett) = ((((tei) << 1) & 0xfe)) | 0x01)

#define GROUP_TEI	127	/* broadcast TEI for LME */

/* control field - octett 4 */

#define OFF_CNTL	2	/* 1st byte of control field */

/* S frames */

#define S_FRAME_LEN	4	/* length of a U-frame */
#define OFF_SRCR	2	/* 1st byte of control field,	*/
				/* R-commands and R-responses	*/
#define OFF_SNR		3	/* 2nd byte of control field, N(R) and PF */
#define SPFBIT		0x01	/* poll/final bit mask */
#define SPBITSET	SPFBIT
#define SFBITSET	SPFBIT
#define GETSNR(octett) (((octett) >> 1) & 0x7f)
#define GETSPF(octett) ((octett) & SPFBIT)
#define RR		0x01	/* RR and bit 0 set */
#define RNR		0x05	/* RNR and bit 0 set */
#define REJ		0x09	/* REJ and bit 0 set */

/* U frames */

#define UI_HDR_LEN	3	/* length of UI header in front of L3 frame */
#define U_FRAME_LEN	3	/* length of a U-frame */
#define UPFBIT		0x10	/* poll/final bit mask */
#define UPBITSET	UPFBIT
#define UFBITSET	UPFBIT
#define GETUPF(octett) (((octett) >> 4) & 0x01)

/* commands/responses with pf bit set to 0 */

#define SABME		0x6f
#define	DM		0x0f
#define UI		0x03
#define DISC		0x43
#define UA		0x63
#define FRMR		0x87
#define XID		0xaf

/* control field - octett 3 */

#define OFF_MEI		3	/* 2nd byte of control field */

/* control field - octett 4,5 */

#define OFF_RIL		4	/* Ri low byte */
#define OFF_RIH		5	/* Ri high byte */

/* control field - octett 6 */

#define OFF_MT		6	/* Message Type */
#define OFF_AI		7	/* Action Indicator  */
#define GET_TEIFROMAI(octett) (((octett) >> 1) & 0x7f)

/* I frame */

#define I_HDR_LEN	4	/* length of I header in front of L3 frame */
#define OFF_INS		2	/* transmit sequence number */
#define OFF_INR		3	/* receive sequence number */
#define IPFBIT		0x01	/* poll/final bit mask */
#define IPBITSET	0x01
#define GETINR(octett)	(((octett) >> 1) & 0x7f)
#define GETINS(octett)	(((octett) >> 1) & 0x7f)
#define GETIP(octett)	((octett) & IPFBIT)

/* structure of a TEI management frame */

#define TEI_MGMT_FRM_LEN   8		/* frame length */
#define TEIM_SAPIO	0x00		/* SAPI, CR, EA */
#define TEIM_TEIO	0x01		/* TEI, EA */
#define TEIM_UIO	0x02		/* frame type = UI = 0x03 */
#define TEIM_MEIO	0x03		/* management entity id = 0x0f */
#define 	MEI	0x0f
#define TEIM_RILO	0x04		/* reference number, low  */
#define TEIM_RIHO	0x05		/* reference number, high */
#define TEIM_MTO	0x06		/* message type */
#define 	MT_ID_REQEST	0x01
#define 	MT_ID_ASSIGN	0x02
#define 	MT_ID_DENY	0x03
#define 	MT_ID_CHK_REQ	0x04
#define 	MT_ID_CHK_RSP	0x05
#define 	MT_ID_REMOVE	0x06
#define 	MT_ID_VERIFY	0x07
#define TEIM_AIO	0x07		/* action indicator */

/* isdn_mdl_error_ind codes */

enum MDL_ERROR_CODES {
	MDL_ERR_A,
	MDL_ERR_B,
	MDL_ERR_C,
	MDL_ERR_D,
	MDL_ERR_E,
	MDL_ERR_F,
	MDL_ERR_G,
	MDL_ERR_H,
	MDL_ERR_I,
	MDL_ERR_J,
	MDL_ERR_K,
	MDL_ERR_L,
	MDL_ERR_M,
	MDL_ERR_N,
	MDL_ERR_O,
	MDL_ERR_MAX
};

/*
 * Software context.
 */
struct isdn_softc {
	struct ifnet 	*sc_ifp; 	
	struct isdn_l2 	sc_l2;
	struct isdn_l3 	sc_l3;
	struct rwlock 	sc_lock;
};

#define SC_LOCK_INIT(sc, d, t) \
	rw_init_flags(&(sc)->sc_lock, (t), RW_RECURSE |  RW_DUPOK)
#define SC_LOCK_DESTROY(sc) 	rw_destroy(&(sc)->sc_lock)
#define SC_RLOCK(sc) 		rw_rlock(&(sc)->sc_lock)
#define SC_WLOCK(sc) 		rw_wlock(&(sc)->sc_lock)
#define SC_TRY_RLOCK(sc) 	rw_try_rlock(&(sc)->sc_lock)
#define SC_TRY_WLOCK(sc) 	rw_try_wlock(&(sc)->sc_lock)
#define SC_RUNLOCK(sc) 		rw_runlock(&(sc)->sc_lock)
#define SC_WUNLOCK(sc) 		rw_wunlock(&(sc)->sc_lock)
#define	SC_TRY_UPGRADE(sc) 	rw_try_upgrade(&(sc)->sc_lock)
#define	SC_DOWNGRADE(sc) 	rw_downgrade(&(sc)->sc_lock)
#define	SC_WLOCKED(sc) 		rw_wowned(&(sc)->sc_lock)
#define	SC_LOCK_ASSERT(sc) 	rw_assert(&(sc)->sc_lock, RA_LOCKED)
#define	SC_RLOCK_ASSERT(sc) 	rw_assert(&(sc)->sc_lock, RA_RLOCKED)
#define	SC_WLOCK_ASSERT(sc) 	rw_assert(&(sc)->sc_lock, RA_WLOCKED)
#define	SC_UNLOCK_ASSERT(sc) 	rw_assert(&(sc)->sc_lock, RA_UNLOCKED)

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
