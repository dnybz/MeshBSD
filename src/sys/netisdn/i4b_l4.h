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
 *	i4b_l4.h - kernel interface to userland header file
 *	---------------------------------------------------
 *
 *	$Id: i4b_l4.h,v 1.10 2005/12/10 23:51:50 elad Exp $
 *
 * $FreeBSD$
 *
 *      last edit-date: [Thu Apr 27 13:28:01 2000]
 *
 *---------------------------------------------------------------------------*/

#ifndef _NETISDN_I4B_L4_H_
#define _NETISDN_I4B_L4_H_

extern void 	i4bputqueue(struct mbuf *);
extern void 	i4bputqueue_hipri(struct mbuf *);
extern void 	i4b_l4_accounting(int, int, int, int, int, int, int, int);
extern void 	i4b_l4_alert_ind(struct call_desc *);
extern void 	i4b_l4_charging_ind( struct call_desc *);
extern void 	i4b_l4_connect_active_ind(struct call_desc *);
extern void 	i4b_l4_connect_ind(struct call_desc *);
extern void 	i4b_l4_daemon_attached(void);
extern void 	i4b_l4_daemon_detached(void);
extern void 	i4b_l4_dialout(int, int);
extern void 	i4b_l4_dialoutnumber(int, int, int, char *);
extern void 	i4b_l4_disconnect_ind(struct call_desc *);
extern void 	i4b_l4_drvrdisc(int);
extern void 	i4b_l4_negcomplete(struct call_desc *);
extern void 	i4b_l4_ifstate_changed(struct call_desc *, int);
extern void 	i4b_l4_idle_timeout_ind(struct call_desc *);
extern void 	i4b_l4_info_ind(struct call_desc *);
extern void 	i4b_l4_packet_ind(int, int, int, struct mbuf *);
extern void 	i4b_l4_l12stat(struct isdn_l3 *, int, int);
extern void 	i4b_l4_pdeact(struct isdn_l3 *, int);
extern void 	i4b_l4_teiasg(struct isdn_l3 *, int);
extern void 	i4b_l4_status_ind(struct call_desc *);
extern void 	i4b_l4_proceeding_ind(struct call_desc *);
extern void 	i4b_idle_check(struct call_desc *);
extern struct call_desc * 	cd_by_cdid(unsigned int);
extern void 	freecd_by_cd(struct call_desc *);
struct call_desc * 	_by_isdnifcr(int, int, int);
void 	free_all_cd_of_isdnif(int);
extern unsigned char 	get_rand_cr(int);
extern struct call_desc * 	reserve_cd(void);
extern void 	T400_start(struct call_desc *);
extern void 	T400_stop(struct call_desc *);
extern void 	update_controller_leds(struct isdn_l3 *);

#endif /* !_NETISDN_I4B_L4_H_ */
