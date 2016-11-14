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

#ifndef _NETISDN_ISDN_H_
#define _NETISDN_ISDN_H_

/*
 * Routing distinguisher, < channel, cr ,sapi, tei > maps to < lla >
 */
struct isdn_rd {
	uint8_t 	rd_chan;
	uint8_t 	rd_cr;
	uint8_t 	rd_sapi; 	
	uint8_t 	rd_tei;	 
} __packed;
#define ISDN_HDRLEN			(sizeof(struct isdn_rd))

/*
 * Denotes ISDN Channel.
 */
struct sockaddr_isdn {
	uint8_t 	sisdn_len; 	/* length */
	sa_family_t 	sisdn_family; 	/* AF_ISDN */
	struct isdn_rd 	sisdn_rd;	/* < channel, cr ,sapi, tei > */		
};
#define SISDN_LEN 	(sizeof(struct sockaddr_isdn))

/*
 * Denotes tel-no.
 */
struct sockaddr_e167 {
	uint8_t 	se167_len; 	/* length */
	sa_family_t 	se167_family; 	/* AF_E167 */
	uint8_t 	se167_telno[ISDN_TELNO_MAX];
	uint8_t 	se167_subaddr[ISDN_SUBADDR_MAX];
};
#endif /* _NETISDN_ISDN_H_ */
