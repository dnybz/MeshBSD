/*	$NetBSD: i4b_l4mgmt.c,v 1.18 2010/01/18 16:37:41 pooka Exp $	*/

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
 *	i4b_l4mgmt.c - layer 4 calldescriptor management utilites
 *	-----------------------------------------------------------
 *
 *	$Id: i4b_l4mgmt.c,v 1.18 2010/01/18 16:37:41 pooka Exp $
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

#include "isdn.h"

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <sys/random.h>

#include <netisdn/i4b_debug.h>
#include <netisdn/i4b_ioctl.h>

#include <netisdn/i4b_l3l4.h>
#include <netisdn/i4b_mbuf.h>
#include <netisdn/i4b_isdnq931.h>
#include <netisdn/i4b_global.h>

#include <netisdn/i4b_l2.h>
#include <netisdn/i4b_l1l2.h>
#include <netisdn/i4b_l4.h>

static unsigned int 	get_cd_id(void);

static void 	i4b_init_callout(struct isdn_call_desc *);
static void 	i4b_stop_callout(struct isdn_call_desc *);

struct isdn_call_desc call_desc[N_CALL_DESC];	/* call descriptor array */
int num_call_desc = 0;

/*---------------------------------------------------------------------------*
 *      return a new unique call descriptor id
 *	--------------------------------------
 *	returns a new calldescriptor id which is used to uniquely identyfy
 *	a single call in the communication between kernel and userland.
 *	this cdid is then used to associate a calldescriptor with an id.
 *---------------------------------------------------------------------------*/
static unsigned int
get_cd_id(void)
{
	static unsigned int cd_id_count = 0;
	int i;

	mtx_lock(&i4b_mtx);
/* 
 * get next id 
 */
	cd_id_count++;

again:
	if (cd_id_count == CDID_UNUSED)		/* zero is invalid */
		cd_id_count++;
	else if (cd_id_count > CDID_MAX)		/* wraparound ? */
		cd_id_count = 1;
/* 
 * check if id already in use 
 */
	for (i = 0; i < num_call_desc; i++) {
		if (call_desc[i].cd_id == cd_id_count) {
			cd_id_count++;
			goto again;
		}
	}

	mtx_unlock(&i4b_mtx);

	return (cd_id_count);
}

/*---------------------------------------------------------------------------*
 *      reserve a calldescriptor for later usage
 *      ----------------------------------------
 *      searches the calldescriptor array until an unused
 *      descriptor is found, gets a new calldescriptor id
 *      and reserves it by putting the id into the cdid field.
 *      returns pointer to the calldescriptor.
 *---------------------------------------------------------------------------*/
struct isdn_call_desc *
reserve_cd(void)
{
	struct isdn_call_desc *cd;
	int i;

	mtx_lock(&i4b_mtx);

	cd = NULL;

	for (i = 0; i < num_call_desc; i++) {
		if (call_desc[i].cd_id == CDID_UNUSED) {
/* 
 * get pointer to descriptor 
 */
			cd = &(call_desc[i]);	
			NDBGL4(L4_MSG, "found free cd - index=%d cdid=%u",
				 i, call_desc[i].cd_id);
			break;
		}
	}
	
	if ((cd == NULL) && (num_call_desc < N_CALL_DESC)) {
/* 
 * get pointer to descriptor 
 */
		i = num_call_desc++;
		cd = &(call_desc[i]);
		NDBGL4(L4_MSG, "found free cd - index=%d cdid=%u",
			 i, call_desc[i].cd_id);
	}
	
	if (cd != NULL) {
		(void)memset(cd, 0, sizeof(struct isdn_call_desc)); /* clear it */
		cd->cd_id = get_cd_id();	/* fill in new cdid */
	}

	mtx_unlock(&i4b_mtx);

	if (cd == NULL)
		panic("reserve_cd: no free call descriptor available!");

	i4b_init_callout(cd);

	return (cd);
}

/*---------------------------------------------------------------------------*
 *      free a calldescriptor
 *      ---------------------
 *      free a unused calldescriptor by giving address of calldescriptor
 *      and writing a 0 into the cdid field marking it as unused.
 *---------------------------------------------------------------------------*/
void
freecd_by_cd(struct isdn_call_desc *cd)
{
	int i;
	
	mtx_lock(&i4b_mtx);

	for (i = 0; i < num_call_desc; i++) {
		if ((call_desc[i].cd_id != CDID_UNUSED) &&
		    (&(call_desc[i]) == cd) ) {
			NDBGL4(L4_MSG, "releasing cd - index=%d cdid=%u cr=%d",
				i, call_desc[i].cd_id, cd->cd_cr);
			call_desc[i].cd_id = CDID_UNUSED;
			break;
		}
	}

	if (i == N_CALL_DESC)
		panic("freecd_by_cd: ERROR, cd not found, cr = %d", cd->cd_cr);

	mtx_unlock(&i4b_mtx);
}

/*
 * ISDN is gone, get rid of all CDs for it
 */
void 
free_all_cd_of_isdnif(int l3_id)
{
	int i;
	
	mtx_lock(&i4b_mtx);

	for (i = 0; i < num_call_desc; i++) {
		if ((call_desc[i].cd_id != CDID_UNUSED) &&
		    call_desc[i].cd_l3_id == l3_id) {
			NDBGL4(L4_MSG, "releasing cd - index=%d cdid=%u cr=%d",
				i, call_desc[i].cd_id, call_desc[i].cd_cr);
			if (call_desc[i].callouts_inited)
				i4b_stop_callout(&call_desc[i]);
			call_desc[i].cd_id = CDID_UNUSED;
			call_desc[i].cd_l3_id = -1;
			call_desc[i].cd_l3 = NULL;
		}
	}
	mtx_unlock(&i4b_mtx);
}

/*---------------------------------------------------------------------------*
 *      return pointer to calldescriptor by giving the calldescriptor id
 *      ----------------------------------------------------------------
 *      lookup a calldescriptor in the calldescriptor array by looking
 *      at the cdid field. return pointer to calldescriptor if found,
 *      else return NULL if not found.
 *---------------------------------------------------------------------------*/
struct isdn_call_desc *
cd_by_cdid(unsigned int cdid)
{
	int i;

	for (i = 0; i < num_call_desc; i++) {
		if (call_desc[i].cd_id == cdid) {
			NDBGL4(L4_MSG, "found cdid - index=%d cdid=%u cr=%d",
					i, call_desc[i].cd_id, call_desc[i].cd_cr);
			i4b_init_callout(&call_desc[i]);
			return (&(call_desc[i]));
		}
	}
	return (NULL);
}

/*---------------------------------------------------------------------------*
 *      search calldescriptor
 *      ---------------------
 *      This routine searches for the calldescriptor for a passive controller
 *      given by unit number, callreference and callreference flag.
 *	It returns a pointer to the calldescriptor if found, else a NULL.
 *---------------------------------------------------------------------------*/
struct isdn_call_desc *
cd_by_isdnifcr(int isdnif, int cr, int crf)
{
	int i;

	for (i = 0; i < num_call_desc; i++) {
		if (call_desc[i].cd_id != CDID_UNUSED
		    && call_desc[i].cd_l3_id == isdnif
		    && call_desc[i].cd_cr == cr
		    && call_desc[i].cd_crflag == crf) {
			NDBGL4(L4_MSG, "found cd, index=%d cdid=%u cr=%d",
			    i, call_desc[i].cd_id, call_desc[i].cd_cr);
			i4b_init_callout(&call_desc[i]);
			return (&(call_desc[i]));
		}
	}
	return (NULL);
}

/*---------------------------------------------------------------------------*
 *	generate 7 bit "random" number used for outgoing Call Reference
 *---------------------------------------------------------------------------*/
unsigned char
get_rand_cr(int unit)
{
	register int i, j;
	static u_char val, retval;
	static int called = 42;
	struct timeval t;

	val += ++called;

	for (i = 0; i < 50 ; i++, val++) {
		int found = 1;

		(void)read_random((char *)&val, sizeof(val));

		retval = val & 0x7f;

		if (retval == 0 || retval == 0x7f)
			continue;

		for (j = 0; j < num_call_desc; j++) {
			if ((call_desc[j].cd_id != CDID_UNUSED) &&
			    (call_desc[j].cd_cr == retval)) {
				found = 0;
				break;
			}
		}

		if (found)
			return (retval);
	}
	return (0);	/* XXX */
}

static void
i4b_stop_callout(struct isdn_call_desc *cd)
{
	if (cd->cd_callouts_inited) {
		callout_stop(&cd->cd_idle_timeout_handle);
		callout_stop(&cd->cd_T303_callout);
		callout_stop(&cd->cd_T305_callout);
		callout_stop(&cd->cd_T308_callout);
		callout_stop(&cd->cd_T309_callout);
		callout_stop(&cd->cd_T310_callout);
		callout_stop(&cd->cd_T313_callout);
		callout_stop(&cd->cd_T400_callout);
	}
}

/*---------------------------------------------------------------------------*
 *	initialize the callout handles for FreeBSD
 *---------------------------------------------------------------------------*/
void
i4b_init_callout(struct isdn_call_desc *cd)
{
	if (cd->cd_callouts_inited == 0) {
		callout_init(&cd->cd_idle_timeout_handle, 0);
		callout_init(&cd->cd_T303_callout, 0);
		callout_init(&cd->cd_T305_callout, 0);
		callout_init(&cd->cd_T308_callout, 0);
		callout_init(&cd->cd_T309_callout, 0);
		callout_init(&cd->cd_T310_callout, 0);
		callout_init(&cd->cd_T313_callout, 0);
		callout_init(&cd->cd_T400_callout, 0);
		cd->cd_callouts_inited = 1;
	}
}

