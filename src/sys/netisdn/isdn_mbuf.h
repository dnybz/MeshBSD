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
 *	i4b - mbuf handling support routines
 *	--------------------------------------
 *
 *	$Id: i4b_mbuf.h,v 1.3 2005/12/10 23:51:50 elad Exp $
 *
 * $FreeBSD$
 *
 *	last edit-date: [Fri Mar  3 14:30:09 2000]
 *
 *---------------------------------------------------------------------------*/

#ifndef _NETISDN_ISDN_MBUF_H_
#define _NETISDN_ISDN_MBUF_H_

/* layer 1 / layer 2 comunication: 3rd ph_data_req parameter */
#define MBUF_DONTFREE	0
#define MBUF_FREE	1

#define MT_DCHAN        MT_DATA
#define MT_BCHAN        MT_DATA

#define MT_ISDN_D	MT_DCHAN
#define MT_ISDN_B	MT_BCHAN

struct mbuf * 	isdn_getmbuf(int, int, short);

#endif /* !_NETISDN_ISDN_MBUF_H_ */

/* EOF */