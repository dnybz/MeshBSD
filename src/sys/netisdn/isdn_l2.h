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
#ifndef _NETISDN_I4B_L2_H_
#define _NETISDN_I4B_L2_H_

struct isdn_l2 {
	struct isdn_sc *l2_sc;

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
	
	void(*l2_T202func)(void *);/* function to be called when T202 expires */
	int	l2_T203;		/* max line idle time */
	
	struct	callout l2_T200_callout;
	struct	callout l2_T202_callout;
	struct	callout l2_T203_callout;
	struct	callout l2_IFQU_callout;

/*
 * isdn_iframe.c, isdn_i_frame_queued_up(): value of IFQU_DLY
 * some experimentation Gary did showed a minimal value of (hz/20) was
 * possible to let this work, Gary suggested using (hz/10) but i settled
 * down to using (hz/5) for now (-hm).
 */
#define IFQU_DLY (hz/5)		/* reschedule I-FRAME-QUEUED-UP 0.2 sec */

	int	l2_vr;		/* receive sequence frame counter */
	int	l2_vs;		/* transmit sequence frame counter */
	int	l2_va;		/* acknowledge sequence frame counter */

	int	l2_ack_pend;	/* acknowledge pending */
	int	l2_rej_excpt;	/* reject exception */
	int	l2_peer_busy;	/* peer receiver busy */
	int	l2_own_busy;	/* own receiver busy */
	int	l2_l3initiated;	/* layer 3 initiated */

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

	int (*l2_postfsmfunc)(struct isdn_l3 *);/* function to be called at fsm exit */
	struct isdn_l3 *l2_postfsmarg;	/* argument for above function */

	/* statistics */

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
#define PUTSAPI(sapi,cr,octett)	((octett) = (((sapi << 2) & 0xfc) | ((cr & 0x01) << 1)))
#define GETCR(octett)		(((octett) >> 1) & 0x01)
#define GETEA(octett)		((octett) & 0x01)

/* address field - octett 3 */

#define OFF_TEI		1	/* TEI offset */
#define GETTEI(octett) (((octett) >> 1) & 0x7f)
#define PUTTEI(tei, octett) ((octett) = ((((tei) << 1) & 0xfe)) | 0x01)
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

/* forward decl */
struct isdn_l3;
extern void isdn_acknowledge_pending ( struct isdn_l2 *l2sc );
extern struct mbuf * isdn_build_s_frame ( struct isdn_l2 *l2sc, crbit_to_nt_t crbit, pbit_t pbit, u_char type );
extern struct mbuf * isdn_build_u_frame ( struct isdn_l2 *l2sc, crbit_to_nt_t crbit, pbit_t pbit, u_char type );
extern void isdn_clear_exception_conditions ( struct isdn_l2 *l2sc );
extern int isdn_dl_data_req ( struct isdn_l2*, struct isdn_l3 *drv, struct mbuf *m );
extern int isdn_dl_establish_req ( struct isdn_l2*, struct isdn_l3 *drv );
extern int isdn_dl_release_req ( struct isdn_l2*, struct isdn_l3 *drv );
extern int isdn_dl_unit_data_req ( struct isdn_l2*, struct isdn_l3 *drv, struct mbuf *m );
extern void isdn_enquiry_response ( struct isdn_l2 *l2sc );
extern void isdn_establish_data_link ( struct isdn_l2 *l2sc );
extern void isdn_invoke_retransmission ( struct isdn_l2 *l2sc, int nr );
extern void isdn_i_frame_queued_up ( struct isdn_l2 *l2sc );
extern void isdn_l1_activate ( struct isdn_l2 *l2sc );
extern int isdn_l2_nr_ok ( int nr, int va, int vs );
extern void isdn_make_rand_ri ( struct isdn_l2 *l2sc );
extern void isdn_mdl_assign_ind ( struct isdn_l2 *l2sc );
extern void isdn_mdl_error_ind ( struct isdn_l2 *l2sc, const char *where, int errorcode );
extern void isdn_next_l2state ( struct isdn_l2 *l2sc, struct isdn_l3 *drv, int event );
extern void isdn_nr_error_recovery ( struct isdn_l2 *l2sc );
extern int isdn_ph_activate_ind ( struct isdn_l2* );
extern int isdn_ph_deactivate_ind ( struct isdn_l2* );
extern void isdn_print_frame ( int len, u_char *buf );
extern const char *isdn_print_l2state ( struct isdn_l2 *l2sc );
extern void isdn_print_l2var ( struct isdn_l2 *l2sc );
extern void isdn_rxd_ack(struct isdn_l2 *l2sc, struct isdn_l3 *drv, int nr);
extern void isdn_rxd_i_frame ( struct isdn_l2 *, struct isdn_l3 *drv, struct mbuf *m );
extern void isdn_rxd_s_frame ( struct isdn_l2 *, struct isdn_l3 *drv, struct mbuf *m );
extern void isdn_rxd_u_frame ( struct isdn_l2 *, struct isdn_l3 *drv, struct mbuf *m );
extern void isdn_T200_restart ( struct isdn_l2 *l2sc );
extern void isdn_T200_start ( struct isdn_l2 *l2sc );
extern void isdn_T200_stop ( struct isdn_l2 *l2sc );
extern void isdn_T202_start ( struct isdn_l2 *l2sc );
extern void isdn_T202_stop ( struct isdn_l2 *l2sc );
extern void isdn_T203_restart ( struct isdn_l2 *l2sc );
extern void isdn_T203_start ( struct isdn_l2 *l2sc );
extern void isdn_T203_stop ( struct isdn_l2 *l2sc );
extern void isdn_tei_assign ( struct isdn_l2 *l2sc );
extern void isdn_tei_chkresp ( struct isdn_l2 *l2sc );
extern void isdn_tei_rxframe ( struct isdn_l2 *, struct isdn_l3 *, struct mbuf *m );
extern void isdn_tei_verify ( struct isdn_l2 *l2sc );
extern void isdn_transmit_enquire ( struct isdn_l2 *l2sc );
extern void isdn_tx_disc ( struct isdn_l2 *l2sc, pbit_t pbit );
extern void isdn_tx_dm ( struct isdn_l2 *l2sc, fbit_t fbit );
extern void isdn_tx_frmr ( struct isdn_l2 *l2sc, fbit_t fbit );
extern void isdn_tx_rej_response ( struct isdn_l2 *l2sc, fbit_t fbit );
extern void isdn_tx_rnr_command ( struct isdn_l2 *l2sc, pbit_t pbit );
extern void isdn_tx_rnr_response ( struct isdn_l2 *l2sc, fbit_t fbit );
extern void isdn_tx_rr_command ( struct isdn_l2 *l2sc, pbit_t pbit );
extern void isdn_tx_rr_response ( struct isdn_l2 *l2sc, fbit_t fbit );
extern void isdn_tx_sabme ( struct isdn_l2 *l2sc, pbit_t pbit );
extern void isdn_tx_ua ( struct isdn_l2 *l2sc, fbit_t fbit );

struct isdn_l3;
extern int isdn_l2_channel_get_state(struct isdn_l3 *drv, int b_chanid);
extern void isdn_l2_channel_set_state(struct isdn_l3 *drv, int b_chanid, int state);
extern int isdn_mdl_status_ind ( struct isdn_l3 *drv, int status, int parm);
extern int isdn_dl_release_ind ( struct isdn_l3 *drv );
extern int isdn_dl_establish_ind ( struct isdn_l3 *drv );
extern int isdn_dl_release_cnf ( struct isdn_l3 *drv );
extern int isdn_dl_establish_cnf ( struct isdn_l3 *drv );
extern int isdn_dl_unit_data_ind ( struct isdn_l3 *drv, struct mbuf *m );
extern int isdn_dl_data_ind ( struct isdn_l3 *drv, struct mbuf *m );
int isdn_mdl_command_req(struct isdn_l3 *drv, int, void *);
void *isdn_get_l2(int isdnif);
extern int isdn_b_chan_silence( unsigned char *data, int len );

#endif /* !_NETISDN_I4B_L2_H_ */
