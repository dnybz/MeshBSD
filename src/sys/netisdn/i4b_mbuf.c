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
 *	i4b - mbuf handling support routines
 *	------------------------------------
 *
 *	$Id: i4b_mbuf.c,v 1.6 2010/01/18 16:29:51 pooka Exp $
 *
 * $FreeBSD$
 *
 *      last edit-date: [Fri Jan  5 11:33:47 2001]
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
 
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/tty.h>
#include <sys/proc.h>
#include <sys/uio.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/i4b_mbuf.h>
#include <netisdn/i4b_global.h>

/*---------------------------------------------------------------------------*
 *	allocate D-channel mbuf space
 *---------------------------------------------------------------------------*/
struct mbuf*
i4b_getmbuf(int len, int how, short type)
{
	struct mbuf *m;

	if (len > MCLBYTES)	{
/* 
 * if length > max extension size 
 */
#ifdef I4B_DEBUG
		(void)printf("i4b_getmbuf: error - len(%d) > MCLBYTES(%d)\n",
					len, MCLBYTES);
#endif

		m = NULL;
		goto out;
	}
/* 
 * get mbuf with pkthdr 
 */
	MGETHDR(m, how, type);	
/* 
 * did we actually get the mbuf? 
 */
	if (m == NULL) {

#ifdef I4B_DEBUG
		(void)printf("i4b_getbuf: error - MGETHDR failed!\n");
#endif

		goto out;
	}

	if (len >= MHLEN) {
		MCLGET(m, how);

		if (!(m->m_flags & M_EXT)) {
			m_freem(m);

#ifdef I4B_DEBUG
			(void)printf("i4b_getbuf: error - MCLGET failed, len(%d)\n", len);
#endif

			m = NULL;
			goto out;
		}
	}

	m->m_len = len;
out:
	return (m);
}


