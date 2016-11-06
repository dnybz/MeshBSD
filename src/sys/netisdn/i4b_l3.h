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
 *	i4b_l3.h - layer 3 header file
 *	------------------------------
 *
 *	$Id: i4b_l3.h,v 1.7 2005/12/10 23:51:50 elad Exp $
 *
 * $FreeBSD$
 *
 *      last edit-date: [Thu Apr 27 11:07:01 2000]
 *
 *---------------------------------------------------------------------------*/

#ifndef _NETISDN_I4B_L3_H_
#define _NETISDN_I4B_L3_H_

extern int utoc_tab[];
extern unsigned char cause_tab_q931[];

extern int 	i4b_aoc(unsigned char *, struct isdn_call_desc *);
extern void 	i4b_decode_q931(int, int, u_char *);
extern int 	i4b_decode_q931_cs0_ie(struct isdn_call_desc *, int, u_char *);
extern void 	i4b_decode_q931_message(struct isdn_call_desc *, u_char);
extern void 	i4b_l3_stop_all_timers(struct isdn_call_desc *);
extern void 	i4b_l3_tx_alert(struct isdn_call_desc *);
extern void 	i4b_l3_tx_connect(struct isdn_call_desc *);
extern void 	i4b_l3_tx_connect_ack(struct isdn_call_desc *);
extern void 	i4b_l3_tx_disconnect(struct isdn_call_desc *);
extern void 	i4b_l3_tx_release(struct isdn_call_desc *, int);
extern void 	i4b_l3_tx_release_complete(struct isdn_call_desc *, int);
extern void 	i4b_l3_tx_setup(struct isdn_call_desc *);
extern void 	i4b_l3_tx_status(struct isdn_call_desc *, u_char);
extern int 	i4b_get_dl_stat( struct isdn_call_desc *);
extern void 	i4b_print_frame(int, u_char *);
extern void 	next_l3state(struct isdn_call_desc *, int);
extern const char * 	print_l3state(struct isdn_call_desc *);
extern unsigned char 	setup_cr(struct isdn_call_desc *, unsigned char);
extern void 	T303_start(struct isdn_call_desc *);
extern void 	T303_stop(struct isdn_call_desc *);
extern void 	T305_start(struct isdn_call_desc *);
extern void 	T305_stop(struct isdn_call_desc *);
extern void 	T308_start(struct isdn_call_desc *);
extern void 	T308_stop(struct isdn_call_desc *);
extern void 	T309_start(struct isdn_call_desc *);
extern void 	T309_stop(struct isdn_call_desc *);
extern void 	T310_start(struct isdn_call_desc *);
extern void 	T310_stop(struct isdn_call_desc *);
extern void 	T313_start(struct isdn_call_desc *);
extern void 	T313_stop(struct isdn_call_desc *);

#endif /* !_NETISDN_I4B_L3_H_ */
