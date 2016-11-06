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

#ifndef _NETISDN_I4B_H_
#define _NETISDN_I4B_H_

struct sockaddr_isdn {
	uint8_t 	sisdn_len; 	/* length */
	sa_family_t 	sisdn_family; 	/* AF_ISDN */
	uint16_t 	sisdn_ctlr; 	/* Index of Ethernet Controller */
	uint8_t 	sisdn_chan;
	uint8_t 	sisdn_proto;
	uint8_t 	sisdn_sapi; 	
	uint8_t 	sisdn_tei;				
};
#define SISDN_LEN 	(sizeof(struct sockaddr_isdn))

/*
 * Routing distinguisher, < channel, proto ,sapi, tei > maps to < lla >
 */
struct isdn_rd {
	uint8_t 	ir_chan;
	uint8_t 	ir_proto;
	uint8_t 	ir_sapi; 	
	uint8_t 	ir_tei;	 
} __packed;
#define ISDN_HDRLEN			(sizeof(struct isdn_rd))

#ifdef _KERNEL

struct i4b_ifinfo {
	struct lltable		*iii_llt;	/* isdn_arp cache */
	
	struct isdn_l2 	iii_l2;
	struct isdn_l3 	iii_l3;

};
#define ISDN_LLTABLE(ifp)	\
	(((struct isdn_ifinfo *)(ifp)->if_afdata[AF_ISDN])->mii_llt)
#define ISDN_IFINFO(ifp) \
	((struct isdn_ifinfo *)(ifp)->if_afdata[AF_ISDN])

#endif /* _KERNEL */
	
#endif /* _NETISDN_I4B_H_ */
