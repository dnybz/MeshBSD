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
 *	i4b_l4.c - kernel interface to userland
 *	-----------------------------------------
 *
 *	$Id: i4b_l4.c,v 1.32 2006/11/16 01:33:49 christos Exp $
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
 
#include "opt_isdn.h"

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/mbuf.h>
#include <sys/proc.h>
#include <sys/fcntl.h>
#include <sys/socket.h>

#include <net/if.h>
#include <net/if_var.h>

#include <netisdn/i4b_debug.h>
#include <netisdn/i4b_ioctl.h>
#include <netisdn/i4b_cause.h>

#include <netisdn/i4b_global.h>
#include <netisdn/i4b_l3l4.h>
#include <netisdn/i4b_mbuf.h>
#include <netisdn/i4b_l2.h>
#include <netisdn/i4b_l3.h>
#include <netisdn/i4b_l4.h>

unsigned int i4b_l4_debug = L4_DEBUG_DEFAULT;

/*
 * ISDNs, in userland sometimes called "controllers", but one controller
 * may have multiple BRIs, for example daic QUAD cards attach four BRIs.
 * An ISDN may also be a PRI (30 B channels).
 */
static SLIST_HEAD(, isdn_l3) isdnif_list = 
	SLIST_HEAD_INITIALIZER(isdnif_list);
static int next_isdnif = 0;

/*
 * Attach a new ISDN-L3 interface instance and return its ISDN identifier
 *
 * XXX: I-ll change the signature of this fn
 * XXX: and refactor it providing an morphism
 * XXX: on IEEE802.{3,11} linklayer.
 */
struct isdn_l3 *
i4b_attach(struct ifnet *ifp, void *l2, 
		struct isdn_l3_sap *sap, int nbch)
{
	struct isdn_l3 *l3;

	(void)strlcpy(l3->l3_xname, ifp->if_xname, IFNAMSIZ);
	
	l3->l3_sap = sap;
	l3->l3_l2 = l2;
	l3->l3_id = next_isdnif++;
	l3->l3_tei = -1;
	l3->l3_dl_est = DL_DOWN;
	l3->l3_nbch = nbch;

	for (i = 0; i < nbch; i++)
		l3->l3_bch_state[i] = BCH_ST_FREE;

	mtx_lock(&i4b_mtx);
	SLIST_INSERT_HEAD(&isdnif_list, l3, l3_q);
	mtx_unlock(&i4b_mtx);

	return (l3);
}

/*
 * Detach a L3 driver instance
 */
int
i4b_detach(struct isdn_l3 *l3)
{
	struct isdn_l3 *sc;
	int isdnif;
	int maxidx;
	int i;

	mtx_lock(&i4b_mtx);

	isdnif = l3->l3_id;

	i4b_l4_contr_ev_ind(isdnif, 0);
	SLIST_REMOVE(&isdnif_list, l3, isdn_l3, l3_q);

	maxidx = -1;
	SLIST_FOREACH(sc, &isdnif_list, l3_q) 
		if (sc->l3_id > maxidx)
			maxidx = sc->l3_id;
	next_isdnif = maxidx+1;

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
	
	(void)printf("ISDN %d detached\n", isdnif);

	return (1);
}

struct isdn_l3 *
i4b_get_l3(int isdnif)
{
	struct isdn_l3 *l3;

	SLIST_FOREACH(l3, &isdnif_list, l3_q) {
		if (l3->l3_id == isdnif)
			break;
	}
/*
 * If not found, ptr denotes cursor maps to NULL implecitely.
 */			
	return (l3);
}

int 
isdn_count_isdnif(int *misdnif)
{
	struct isdn_l3 *l3;
	int count = 0;
	int max_isdnif = -1;

	SLIST_FOREACH(l3, &isdnif_list, l3_q) {
		count++;
		if (l3->l3_id > max_isdnif)
			max_isdnif = l3->l3_id;
	}

	if (misdnif)
		*misdnif = max_isdnif;

	return (count);
}

void *
i4b_get_l2(int isdnif)
{
	struct isdn_l3 *l3 = i4b_get_l3(isdnif);
	if (l3 == NULL)
		return (NULL);
		
	return (l3->l3_l2);
}

/*---------------------------------------------------------------------------*
 *      daemon is attached
 *---------------------------------------------------------------------------*/
void
i4b_l4_daemon_attached(void)
{
	struct isdn_l3 *d;

	mtx_lock(&i4b_mtx);
	
	SLIST_FOREACH(d, &isdnif_list, l3_q) {
		d->l3_sap->N_MGMT_COMMAND(d, CMR_DOPEN, 0);
	}
	mtx_unlock(&i4b_mtx);
}

/*---------------------------------------------------------------------------*
 *      daemon is detached
 *---------------------------------------------------------------------------*/
void
i4b_l4_daemon_detached(void)
{
	struct isdn_l3 *d;

	mtx_lock(&i4b_mtx);

	SLIST_FOREACH(d, &isdnif_list, l3_q) {
		d->l3_sap->N_MGMT_COMMAND(d, CMR_DCLOSE, 0);
	}
	mtx_unlock(&i4b_mtx);
}

/*
 * B-channel layer 4 drivers and their registry.
 * (Application drivers connecting to a B-channel)
 */
static int 	i4b_link_bchandrvr(struct isdn_call_desc *);
static void 	i4b_unlink_bchandrvr(struct isdn_call_desc *);
static void 	i4b_l4_setup_timeout(struct isdn_call_desc *);
static void 	i4b_idle_check_fix_unit(struct isdn_call_desc *);
static void 	i4b_idle_check_var_unit(struct isdn_call_desc *);
static void 	i4b_l4_setup_timeout_fix_unit(struct isdn_call_desc *);
static void 	i4b_l4_setup_timeout_var_unit(struct isdn_call_desc *);
static time_t 	i4b_get_idletime(struct isdn_call_desc *);

static int next_l4_id = 0;

/*
 * A L4-softc denotes an interface for 
 * set of callback-sap's implementing on
 * e. g. socket-layer. 
 */
struct isdn_l4 {
	SLIST_ENTRY(isdn_l4) l4_q;
	char l4_name[L4IF_NAME_SIZ];
	int l4_id;
	struct isdn_l4_sap *l4_sap;
	int l4_units;
};
static SLIST_HEAD(, isdn_l4) l4_iflist = SLIST_HEAD_INITIALIZER(l4_iflist);

/*
 * Attach interface on L4 as morphism in e. g. socket-layer.
 */
int 
i4b_l4_attach(const char *name, int units, 
	struct isdn_l4_sap *sap)
{
	struct isdn_l4 *l4;
/*
 * Fall asleep until necessary resources are ready for allocation.
 */
	l4 = malloc(sizeof(struct isdn_l4), M_DEVBUF, M_WAITOK|M_ZERO);
	
	mtx_lock(&i4b_mtx);
	
	(void)strncpy(l4->l4_name, name, L4IF_NAME_SIZ);
	
	l4->l4_name[L4IF_NAME_SIZ-1] = 0;
	l4->l4_id =	next_l4_id++;
	l4->l4_sap = sap;
	l4->units = units;
	
	SLIST_INSERT_HEAD(&l4_iflist, l4, l4_q);
	
	mtx_unlock(&i4b_mtx);
	
	return (l4->l4_id);
}

/*
 * Release by interface bound resources.
 */
int 
i4b_l4_detach(const char *name)
{
	struct isdn_l4 *d;
	
	SLIST_FOREACH(d, &l4_iflist, l4_q) {
		if (strcmp(d->l4_name, name) == 0) {
			break;	
		}
	}	
/*
 * If found, zero out and free(9).
 */
	if (d != NULL) {
		
		mtx_lock(&i4b_mtx);
		
		next_l4_id--;
		SLIST_REMOVE(&l4_iflist, d, isdn_l4, l4_q);
		bzero(d, sizeof(*d));
		free(d, M_DEVBUF);	
		
		mtx_unlock(&i4b_mtx);
	}
	return (0);
}

/*
 * Get interface (set containing callback sap's) by its name.
 */
struct isdn_l4_sap *
i4b_l4_get_sap(const char *name, int unit)
{
	struct isdn_l4 *d;
	
	SLIST_FOREACH(d, &l4_iflist, l4_q) {
		if (strcmp(d->l4_name, name) == 0) {
			return (d->l4_sap);
		}
	}	
		
	return (NULL);
}

/*
 * Get id of interface.
 */
int 
i4b_l4_get_id(const char *name)
{
	struct isdn_l4 *l4;
	
	SLIST_FOREACH(l4, &l4_iflist, l4_q) {
		if (strcmp(l4->l4_name, name) == 0) {
			return (l4->l4_id);
		}
	}
	return (-1);
}

/*
 * Get interface (callback sap) by its id.
 */
struct isdn_l4_sap *
i4b_l4_get_sap(int l4_id, int unit)
{
	struct isdn_l4 * d;
	
	SLIST_FOREACH(d, &l4_iflist, l4_q) {
		if (d->l4_id == l4_id) {
			return (d->l4_sap);
		}
	}
	return (NULL);
}

/*---------------------------------------------------------------------------*
 *	send MSG_PDEACT_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_pdeact(struct isdn_l3 *l3, int numactive)
{
	struct mbuf *m;
	int i;
	struct isdn_call_desc *cd;

	for (i = 0; i < num_call_desc; i++) {
		if ((call_desc[i].cd_id != CDID_UNUSED) 
			&& (call_desc[i].l3 == l3)) {
			cd = &call_desc[i];

			if (cd->timeout_active) 
				STOP_TIMER(cd->idle_timeout_handle, 
					i4b_idle_check, cd);

			if ((cd->l4_sap != NULL) 
				&& (cd->l4_softc != NULL)) {
				(*cd->l4_sap->line_disconnected)
					(cd->l4_softc, (void *)cd);
				i4b_unlink_bchandrvr(cd);
			}

			if ((cd->channelid >= 0)
			     && (cd->channelid < d->l3_nbch))
				d->l3_bch_state[cd->channelid] = BCH_ST_FREE;

			cd->cd_id = CDID_UNUSED;
		}
	}

	
	if ((m = i4b_Dgetmbuf(sizeof(msg_pdeact_ind_t))) != NULL) {
		msg_pdeact_ind_t *md = (msg_pdeact_ind_t *)m->m_data;

		md->header.type = MSG_PDEACT_IND;
		md->header.cd_id = -1;

		md->controller = d->l3_id;
		md->numactive = numactive;

		i4bputqueue_hipri(m);		/* URGENT !!! */
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_L12STAT_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_l12stat(struct isdn_l3 *d, int layer, int state)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_l12stat_ind_t))) != NULL) {
		msg_l12stat_ind_t *md = (msg_l12stat_ind_t *)m->m_data;

		md->header.type = MSG_L12STAT_IND;
		md->header.cd_id = -1;

		md->controller = d->l3_id;
		md->layer = layer;
		md->state = state;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_TEIASG_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_teiasg(struct isdn_l3 *d, int tei)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_teiasg_ind_t))) != NULL) {
		msg_teiasg_ind_t *md = (msg_teiasg_ind_t *)m->m_data;

		md->header.type = MSG_TEIASG_IND;
		md->header.cd_id = -1;

		md->controller = d->l3_id;
		md->l3_tei = d->l3_tei;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_DIALOUT_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_dialout(int sap, int driver_unit)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_dialout_ind_t))) != NULL) {
		msg_dialout_ind_t *md = (msg_dialout_ind_t *)m->m_data;

		md->header.type = MSG_DIALOUT_IND;
		md->header.cd_id = -1;

		md->l4_sap = driver;
		md->l4_sap_unit = driver_unit;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_DIALOUTNUMBER_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_dialoutnumber(int driver, int driver_unit, int cmdlen, char *cmd)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_dialoutnumber_ind_t))) != NULL) {
		msg_dialoutnumber_ind_t *md = (msg_dialoutnumber_ind_t *)m->m_data;

		md->header.type = MSG_DIALOUTNUMBER_IND;
		md->header.cd_id = -1;

		md->l4_sap = driver;
		md->l4_sap_unit = driver_unit;

		if (cmdlen > TELNO_MAX)
			cmdlen = TELNO_MAX;

		md->cmdlen = cmdlen;
		memcpy(md->cmd, cmd, cmdlen);
		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_NEGOTIATION_COMPL message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_negcomplete(struct isdn_call_desc *cd)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_negcomplete_ind_t))) != NULL) {
		msg_negcomplete_ind_t *md = (msg_negcomplete_ind_t *)m->m_data;

		md->header.type = MSG_NEGCOMP_IND;
		md->header.cd_id = cd->cd_id;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_IFSTATE_CHANGED_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_ifstate_changed(struct isdn_call_desc *cd, int new_state)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_ifstatechg_ind_t))) != NULL) {
		msg_ifstatechg_ind_t *md = (msg_ifstatechg_ind_t *)m->m_data;

		md->header.type = MSG_IFSTATE_CHANGED_IND;
		md->header.cd_id = cd->cd_id;
		md->state = new_state;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_DRVRDISC_REQ message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_drvrdisc(int cd_id)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_drvrdisc_req_t))) != NULL) {
		msg_drvrdisc_req_t *md = (msg_drvrdisc_req_t *)m->m_data;

		md->header.type = MSG_DRVRDISC_REQ;
		md->header.cd_id = cd_id;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_ACCT_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_accounting(int cd_id, int accttype, int ioutbytes,
		int iinbytes, int ro, int ri, int outbytes, int inbytes)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_accounting_ind_t))) != NULL) {
		msg_accounting_ind_t *md = (msg_accounting_ind_t *)m->m_data;

		md->header.type = MSG_ACCT_IND;
		md->header.cd_id = cd_id;

		md->accttype = accttype;
		md->ioutbytes = ioutbytes;
		md->iinbytes = iinbytes;
		md->outbps = ro;
		md->inbps = ri;
		md->outbytes = outbytes;
		md->inbytes = inbytes;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_CONNECT_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_connect_ind(struct isdn_call_desc *cd)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_connect_ind_t))) != NULL) {
		msg_connect_ind_t *mp = (msg_connect_ind_t *)m->m_data;

		mp->header.type = MSG_CONNECT_IND;
		mp->header.cd_id = cd->cd_id;

		mp->controller = cd->l3_id;
		mp->channel = cd->channelid;
		mp->bprot = cd->bprot;

		cd->dir = DIR_INCOMING;

		if (strlen(cd->dst_telno) > 0)
			strlcpy(mp->dst_telno, cd->dst_telno,
			    sizeof(mp->dst_telno));
		else
			strlcpy(mp->dst_telno, TELNO_EMPTY,
			    sizeof(mp->dst_telno));

		if (strlen(cd->src_telno) > 0)
			strlcpy(mp->src_telno, cd->src_telno,
			    sizeof(mp->src_telno));
		else
			strlcpy(mp->src_telno, TELNO_EMPTY,
			    sizeof(mp->src_telno));
		mp->type_plan = cd->type_plan;
		memcpy(mp->src_subaddr, cd->src_subaddr, sizeof(mp->src_subaddr));
		memcpy(mp->dest_subaddr, cd->dest_subaddr, sizeof(mp->dest_subaddr));

		strlcpy(mp->display, cd->display, sizeof(mp->src_telno));

		mp->scr_ind = cd->scr_ind;
		mp->prs_ind = cd->prs_ind;

		T400_start(cd);

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_CONNECT_ACTIVE_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_connect_active_ind(struct isdn_call_desc *cd)
{
	struct mbuf *m;

	mtx_lock(&i4b_mtx);

	cd->last_active_time = cd->connect_time = SECOND;

	NDBGL4(L4_TIMO, "last_active/connect_time=%ld", (long)cd->connect_time);

	i4b_link_bchandrvr(cd);

	if (cd->l4_sap != NULL && cd->l4_softc != NULL)
		(*cd->l4_sap->line_connected)(cd->l4_softc, cd);

	i4b_l4_setup_timeout(cd);

	mtx_unlock(&i4b_mtx);

	if ((m = i4b_Dgetmbuf(sizeof(msg_connect_active_ind_t))) != NULL) {
		msg_connect_active_ind_t *mp = 
			(msg_connect_active_ind_t *)m->m_data;

		mp->header.type = MSG_CONNECT_ACTIVE_IND;
		mp->header.cd_id = cd->cd_id;
		mp->controller = cd->l3_id;
		mp->channel = cd->channelid;
		if (cd->datetime[0] != '\0')
			strlcpy(mp->datetime, cd->datetime,
			    sizeof(mp->datetime));
		else
			mp->datetime[0] = '\0';
		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_DISCONNECT_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_disconnect_ind(struct isdn_call_desc *cd)
{
	struct isdn_l3 *d;
	struct mbuf *m;

	if (cd->timeout_active)
		STOP_TIMER(cd->idle_timeout_handle, i4b_idle_check, cd);

	if (cd->l4_sap != NULL && cd->l4_softc != NULL) {
		(*cd->l4_sap->line_disconnected)
			(cd->l4_softc, (void *)cd);
		i4b_unlink_bchandrvr(cd);
	}

	d = cd->l3;

	if ((cd->channelid >= 0) && (cd->channelid < d->l3_nbch)) {
		d->l3_bch_state[cd->channelid] = BCH_ST_FREE;
		/*
		 * XXX: don't call l2 function for active cards.
		 */
		if (d->l3_sap->N_DOWNLOAD == NULL)
			i4b_l2_channel_set_state(d, cd->channelid, BCH_ST_FREE);
	
	} else {
/* 
 * no error, might be hunting call for callback 
 */
		NDBGL4(L4_MSG, "invalid channel %d for ISDN!", cd->channelid);
	}

	if ((m = i4b_Dgetmbuf(sizeof(msg_disconnect_ind_t))) != NULL) {
		msg_disconnect_ind_t *mp = (msg_disconnect_ind_t *)m->m_data;

		mp->header.type = MSG_DISCONNECT_IND;
		mp->header.cd_id = cd->cd_id;
		mp->cause = cd->cause_in;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_IDLE_TIMEOUT_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_idle_timeout_ind(struct isdn_call_desc *cd)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_idle_timeout_ind_t))) != NULL) {
		msg_idle_timeout_ind_t *mp = (msg_idle_timeout_ind_t *)m->m_data;

		mp->header.type = MSG_IDLE_TIMEOUT_IND;
		mp->header.cd_id = cd->cd_id;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_CHARGING_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_charging_ind(struct isdn_call_desc *cd)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_charging_ind_t))) != NULL) {
		msg_charging_ind_t *mp = (msg_charging_ind_t *)m->m_data;

		mp->header.type = MSG_CHARGING_IND;
		mp->header.cd_id = cd->cd_id;
		mp->units_type = cd->units_type;
/*
 * XXX
 */		
		if (mp->units_type == CHARGE_CALC)
			mp->units = cd->cunits;
		else
			mp->units = cd->units;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_STATUS_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_status_ind(struct isdn_call_desc *cd)
{

}

/*---------------------------------------------------------------------------*
 *	send MSG_ALERT_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_alert_ind(struct isdn_call_desc *cd)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_alert_ind_t))) != NULL) {
		msg_alert_ind_t *mp = (msg_alert_ind_t *)m->m_data;

		mp->header.type = MSG_ALERT_IND;
		mp->header.cd_id = cd->cd_id;

		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	send MSG_INFO_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_info_ind(struct isdn_call_desc *cd)
{

}

/*---------------------------------------------------------------------------*
 *	send MSG_INFO_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_proceeding_ind(struct isdn_call_desc *cd)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_proceeding_ind_t))) != NULL) {
		msg_proceeding_ind_t *mp = (msg_proceeding_ind_t *)m->m_data;

		mp->header.type = MSG_PROCEEDING_IND;
		mp->header.cd_id = cd->cd_id;
		mp->controller = cd->l3_id;
		mp->channel = cd->channelid;
		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *    send MSG_PACKET_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_packet_ind(int driver, int driver_unit, int dir, struct mbuf *pkt)
{
	struct mbuf *m;
	int len = pkt->m_pkthdr.len;
	unsigned char *ip = pkt->m_data;

	if ((m = i4b_Dgetmbuf(sizeof(msg_packet_ind_t))) != NULL) {
		msg_packet_ind_t *mp = (msg_packet_ind_t *)m->m_data;

		mp->header.type = MSG_PACKET_IND;
		mp->header.cd_id = -1;
		mp->l4_sap = driver;
		mp->l4_sap_unit = driver_unit;
		mp->direction = dir;
		memcpy(mp->pktdata, ip,
			len <MAX_PACKET_LOG ? len : MAX_PACKET_LOG);
		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *    send MSG_CONTR_EV_IND message to userland
 *---------------------------------------------------------------------------*/
void
i4b_l4_contr_ev_ind(int controller, int attach)
{
	struct mbuf *m;

	if ((m = i4b_Dgetmbuf(sizeof(msg_ctrl_ev_ind_t))) != NULL) {
		msg_ctrl_ev_ind_t *ev = (msg_ctrl_ev_ind_t *)m->m_data;

		ev->header.type = MSG_CONTR_EV_IND;
		ev->header.cd_id = -1;
		ev->controller = controller;
		ev->event = attach;
		i4bputqueue(m);
	}
}

/*---------------------------------------------------------------------------*
 *	link a driver(unit) to a B-channel(controller,unit,channel)
 *---------------------------------------------------------------------------*/
static int
i4b_link_bchandrvr(struct isdn_call_desc *cd)
{
	struct isdn_l3 *d = cd->l3;

	if ((d == NULL) || (d->l3_sap == NULL) 
		|| (d->l3_sap->get_linktab == NULL)) {
			cd->ilt = NULL;
			return (1);
	}

	cd->ilt = d->l3_sap->get_linktab(d->l3_l2, cd->channelid);

	cd->l4_sap = i4b_l4_get_sap(cd->bchan_sap_index, 
		cd->bchan_sap_unit);
	
	if (cd->l4_sap != NULL) {
		cd->l4_softc = 
			cd->l4_sap->get_softc(cd->bchan_sap_unit);
	} else
		cd->l4_softc = NULL;

	if ((cd->l4_sap == NULL) 
		|| (cd->l4_softc == NULL) 
		|| (cd->ilt == NULL))
		return(-1);

	if (d->l3_sap->set_l4_sap != NULL) {
		d->l3_sap->set_l4_sap(d->l3_l2,
		    cd->channelid, cd->l4_sap, cd->l4_softc);
	}

	cd->l4_sap->set_linktab(cd->l4_softc, cd->ilt);
/* 
 * activate B channel 
 */
	(*cd->ilt->bchannel_if->bch_config)
		(cd->ilt->l1token, cd->ilt->channel, cd->bprot, 1);

	return(0);
}

/*---------------------------------------------------------------------------*
 *	unlink a driver(unit) from a B-channel(controller,unit,channel)
 *---------------------------------------------------------------------------*/
static void
i4b_unlink_bchandrvr(struct isdn_call_desc *cd)
{
	struct isdn_l3 *d = cd->l3;

	/*
	 * XXX - what's this *cd manipulation for? Shouldn't we
	 * close the bchannel driver first and then just set ilt to NULL
	 * in *cd?
	 */
	if ((d == NULL) || (d->l3_sap == NULL) 
		|| ((d->l3_sap->get_linktab == NULL)) {
		cd->ilt = NULL;
		return;
	} else {
		cd->ilt = d->l3_sap->get_linktab(
		    d->l3_l2, cd->channelid);
	}
/* 
 * deactivate B channel 
 */
	(*cd->ilt->bchannel_if->bch_config)
		(cd->ilt->l1token, cd->ilt->channel, cd->bprot, 0);
}

/*---------------------------------------------------------------------------

	How shorthold mode works for OUTGOING connections
	=================================================

	|<---- unchecked-window ------->|<-checkwindow->|<-safetywindow>|

idletime_state:      IST_NONCHK             IST_CHECK       IST_SAFE

	|				|		|		|
  time>>+-------------------------------+---------------+---------------+-...
	|				|		|		|
	|				|<--idle_time-->|<--earlyhup--->|
	|<-----------------------unitlen------------------------------->|


	  unitlen - specifies the time a charging unit lasts
	idle_time - specifies the thime the line must be idle at the
		    end of the unit to be elected for hangup
	 earlyhup - is the beginning of a timing safety zone before the
		    next charging unit starts

	The algorithm works as follows: lets assume the unitlen is 100
	secons, idle_time is 40 seconds and earlyhup is 10 seconds.
	The line then must be idle 50 seconds after the begin of the
	current unit and it must then be quiet for 40 seconds. if it
	has been quiet for this 40 seconds, the line is closed 10
	seconds before the next charging unit starts. In case there was
	any traffic within the idle_time, the line is not closed.
	It does not matter whether there was any traffic between second
	0 and second 50 or not.


	How shorthold mode works for INCOMING connections
	=================================================

	it is just possible to specify a maximum idle time for incoming
	connections, after this time of no activity on the line the line
	is closed.

---------------------------------------------------------------------------*/

static time_t
i4b_get_idletime(struct isdn_call_desc *cd)
{
	if (cd->l4_sap != NULL && cd->l4_softc != NULL
	    && cd->l4_sap->get_idletime)
		return (cd->l4_sap->get_idletime(cd->l4_softc));
		
	return (cd->last_active_time);
}

/*---------------------------------------------------------------------------*
 *	B channel idle check timeout setup
 *---------------------------------------------------------------------------*/
static void
i4b_l4_setup_timeout(struct isdn_call_desc *cd)
{
	NDBGL4(L4_TIMO, "%ld: direction %d, shorthold algorithm %d",
		(long)SECOND, cd->dir, cd->shorthold_data.shorthold_algorithm);

	cd->timeout_active = 0;
	cd->idletime_state = IST_IDLE;

	if ((cd->dir == DIR_INCOMING) && (cd->max_idle_time > 0)) {
/* 
 * incoming call: simple max idletime check 
 */
		START_TIMER(cd->idle_timeout_handle, i4b_idle_check, cd, hz/2);
		cd->timeout_active = 1;
		NDBGL4(L4_TIMO, 
			"%ld: incoming-call, setup max_idle_time to %ld", 
				(long)SECOND, (long)cd->max_idle_time);
	} else if ((cd->dir == DIR_OUTGOING) 
		&& (cd->shorthold_data.idle_time > 0)) {
		
		switch (cd->shorthold_data.shorthold_algorithm) {
		default:	/* fall into the old fix algorithm */
		case SHA_FIXU:
			i4b_l4_setup_timeout_fix_unit( cd );
			break;

		case SHA_VARU:
			i4b_l4_setup_timeout_var_unit( cd );
			break;
		}
	} else 
		NDBGL4(L4_TIMO, "no idle_timeout configured");
}

/*---------------------------------------------------------------------------*
 *	fixed unit algorithm B channel idle check timeout setup
 *---------------------------------------------------------------------------*/
static void
i4b_l4_setup_timeout_fix_unit(struct isdn_call_desc *cd)
{
	/* outgoing call */

	if ((cd->shorthold_data.idle_time > 0) && 
		(cd->shorthold_data.unitlen_time == 0)) {
		/* outgoing call: simple max idletime check */

		START_TIMER(cd->idle_timeout_handle, i4b_idle_check, cd, hz/2);
		cd->timeout_active = 1;
		NDBGL4(L4_TIMO, "%ld: outgoing-call, setup idle_time to %ld",
			(long)SECOND, (long)cd->shorthold_data.idle_time);
	} else if ((cd->shorthold_data.unitlen_time > 0) && 
		(cd->shorthold_data.unitlen_time > 
		(cd->shorthold_data.idle_time + 
			cd->shorthold_data.earlyhup_time))) {
/* 
 * outgoing call: full shorthold mode check 
 */
		START_TIMER(cd->idle_timeout_handle, 
			i4b_idle_check, cd, 
			hz*(cd->shorthold_data.unitlen_time - 
			(cd->shorthold_data.idle_time + 
			cd->shorthold_data.earlyhup_time)));
		
		cd->timeout_active = 1;
		cd->idletime_state = IST_NONCHK;
		
		NDBGL4(L4_TIMO, 
			"%ld: outgoing-call, start %ld sec nocheck window",
			(long)SECOND, (long)(cd->shorthold_data.unitlen_time - 
			(cd->shorthold_data.idle_time + 
			cd->shorthold_data.earlyhup_time)));

		if (cd->aocd_flag == 0) {
			cd->units_type = CHARGE_CALC;
			cd->cunits++;
			i4b_l4_charging_ind(cd);
		}
	} else {
/* 
 * parms somehow got wrong .. 
 */
		NDBGL4(L4_ERR, 
			"%ld: ERROR: idletime[%ld]+earlyhup[%ld] > unitlength[%ld]!",
			(long)SECOND, (long)cd->shorthold_data.idle_time, 
			(long)cd->shorthold_data.earlyhup_time, 
			(long)cd->shorthold_data.unitlen_time);
	}
}

/*---------------------------------------------------------------------------*
 *	variable unit algorithm B channel idle check timeout setup
 *---------------------------------------------------------------------------*/
static void
i4b_l4_setup_timeout_var_unit(struct isdn_call_desc *cd)
{
/* outgoing call: variable unit idletime check */ 

/*
 * start checking for an idle connect one second before the end of the unit.
 * The one second takes into account of rounding due to the driver only
 * using the seconds and not the uSeconds of the current time
 */
	cd->idletime_state = IST_CHECK;	/* move directly to the checking state */

	START_TIMER(cd->idle_timeout_handle, 
		i4b_idle_check, cd, 
		hz * (cd->shorthold_data.unitlen_time - 1));
	
	cd->timeout_active = 1;
	
	NDBGL4(L4_TIMO, "%ld: outgoing-call, var idle time - setup to %ld",
		(long)SECOND, (long)cd->shorthold_data.unitlen_time);
}


/*---------------------------------------------------------------------------*
 *	B channel idle check timeout function
 *---------------------------------------------------------------------------*/
void
i4b_idle_check(struct isdn_call_desc *cd)
{
	struct isdn_l3 *d;
	
	if (cd->cd_id == CDID_UNUSED)
		return;

	mtx_lock(&i4b_mtx);
/* 
 * failsafe 
 */	
	d = cd->l3;

	if (cd->timeout_active == 0) 
		NDBGL4(L4_ERR, "ERROR: timeout_active == 0 !!!"); 
	else 
		cd->timeout_active = 0;
/* 
 * incoming connections, simple idletime check 
 */
	if (cd->dir == DIR_INCOMING) {
		if ((i4b_get_idletime(cd) + cd->max_idle_time) <= SECOND) {
		
			NDBGL4(L4_TIMO, "%ld: incoming-call, "
				"line idle timeout, disconnecting!", (long)SECOND);
				d->l3_sap->N_DISCONNECT_REQUEST(cd,
					(CAUSET_I4B << 8) | CAUSE_I4B_NORMAL);
		
			i4b_l4_idle_timeout_ind(cd);
		} else {
			NDBGL4(L4_TIMO, "%ld: incoming-call, activity, "
				"last_active=%ld, max_idle=%ld", 
				(long)SECOND, 
				(long)i4b_get_idletime(cd), 
				(long)cd->max_idle_time);

			START_TIMER(cd->idle_timeout_handle, i4b_idle_check, cd, hz/2);
			cd->timeout_active = 1;
		}
	} else if (cd->dir == DIR_OUTGOING) {
/* 
 * outgoing connections 
 */				
		switch (cd->shorthold_data.shorthold_algorithm) {
		case SHA_FIXU:
			i4b_idle_check_fix_unit( cd );
			break;
		case SHA_VARU:
			i4b_idle_check_var_unit( cd );
			break;
		default:
			NDBGL4(L4_TIMO, "%ld: bad value for "
				"shorthold_algorithm of %d",
				(long)SECOND, cd->shorthold_data.shorthold_algorithm);
				i4b_idle_check_fix_unit(cd);
			break;
		}
	}
	mtx_unlock(&i4b_mtx);
}

/*---------------------------------------------------------------------------*
 *	fixed unit algorithm B channel idle check timeout function
 *---------------------------------------------------------------------------*/
static void
i4b_idle_check_fix_unit(struct isdn_call_desc *cd)
{
	struct isdn_l3 *d = cd->l3;

	/* simple idletime calculation */

	if ((cd->shorthold_data.idle_time > 0) && 
		(cd->shorthold_data.unitlen_time == 0)) {
		
		if ((i4b_get_idletime(cd) + 
			cd->shorthold_data.idle_time) <= SECOND) {
			NDBGL4(L4_TIMO, "%ld: outgoing-call-st, "
				"idle timeout, disconnecting!", (long)SECOND);
			d->l3_sap->N_DISCONNECT_REQUEST(cd, 
				(CAUSET_I4B << 8) | CAUSE_I4B_NORMAL);
			i4b_l4_idle_timeout_ind(cd);
		} else {
			NDBGL4(L4_TIMO, "%ld: outgoing-call-st, "
				"activity, last_active=%ld, max_idle=%ld",
					(long)SECOND, (long)i4b_get_idletime(cd), 
						(long)cd->shorthold_data.idle_time);
			START_TIMER(cd->idle_timeout_handle, 
				i4b_idle_check, cd, hz/2);
			cd->timeout_active = 1;
		}
	} else if ((cd->shorthold_data.unitlen_time > 0) && 
		(cd->shorthold_data.unitlen_time > 
			(cd->shorthold_data.idle_time + 
			cd->shorthold_data.earlyhup_time))) {
/* 
 * full shorthold mode calculation 
 */						
		switch (cd->idletime_state) {
		case IST_NONCHK:	
/* 
 * end of non-check time 
 */
			START_TIMER(cd->idle_timeout_handle, 
				i4b_idle_check, cd, 
				hz*(cd->shorthold_data.idle_time));
			
			cd->idletimechk_start = SECOND;
			cd->idletime_state = IST_CHECK;
			cd->timeout_active = 1;
			
			NDBGL4(L4_TIMO, "%ld: outgoing-call, "
				"idletime check window reached!", (long)SECOND);
			break;
		case IST_CHECK:		
/* 
 * end of idletime chk 
 */
			if ((i4b_get_idletime(cd) > cd->idletimechk_start) &&
			   (i4b_get_idletime(cd) <= SECOND)) {	
/* 
 * activity detected 
 */
				START_TIMER(cd->idle_timeout_handle, 
					i4b_idle_check, cd, 
						hz*(cd->shorthold_data.earlyhup_time));
				
				cd->timeout_active = 1;
				cd->idletime_state = IST_SAFE;
				
				NDBGL4(L4_TIMO, "%ld: outgoing-call, activity "
					"at %ld, wait earlyhup-end", (long)SECOND, 
					(long)i4b_get_idletime(cd));
			} else {	
/* 
 * no activity, hangup 
 */
				NDBGL4(L4_TIMO, "%ld: outgoing-call, idle "
					"timeout, last activity at %ld", (long)SECOND, 
					(long)i4b_get_idletime(cd));
					d->l3_sap->N_DISCONNECT_REQUEST(cd, 
						(CAUSET_I4B << 8) | CAUSE_I4B_NORMAL);
				
				i4b_l4_idle_timeout_ind(cd);
				cd->idletime_state = IST_IDLE;
			}
			break;
		case IST_SAFE:	
/* 
 * end of earlyhup time 
 */
			START_TIMER(cd->idle_timeout_handle, 
				i4b_idle_check, cd, 
				hz*(cd->shorthold_data.unitlen_time - 
					(cd->shorthold_data.idle_time + 
					cd->shorthold_data.earlyhup_time)));
			
			cd->timeout_active = 1;
			cd->idletime_state = IST_NONCHK;

			if (cd->aocd_flag == 0) {
				cd->units_type = CHARGE_CALC;
				cd->cunits++;
				i4b_l4_charging_ind(cd);
			}

			NDBGL4(L4_TIMO, "%ld: outgoing-call, earlyhup end, "
				"wait for idletime start", (long)SECOND);
			break;
		default:
			NDBGL4(L4_ERR, "outgoing-call: invalid "
				"idletime_state value!");
			
			cd->idletime_state = IST_IDLE;
			break;
		}
	}
}

/*---------------------------------------------------------------------------*
 *	variable unit algorithm B channel idle check timeout function
 *---------------------------------------------------------------------------*/
static void
i4b_idle_check_var_unit(struct isdn_call_desc *cd)
{
	struct isdn_l3 *d = cd->l3;
/* 
 * see if there has been any activity 
 * within the last idle_time seconds 
 */		
	switch (cd->idletime_state) {
	case IST_CHECK:
		if (i4b_get_idletime(cd) > 
			(SECOND - cd->shorthold_data.idle_time)) {	
/* 
 * activity detected 
 */
			cd->idle_timeout_handle =
/* 
 * check again in one second 
 */
			START_TIMER(cd->idle_timeout_handle, 
				i4b_idle_check, cd, hz);
			
			cd->timeout_active = 1;
			cd->idletime_state = IST_CHECK;
			
			NDBGL4(L4_TIMO, "%ld: outgoing-call, var "
				"idle timeout - activity at %ld, continuing", 
					(long)SECOND, (long)i4b_get_idletime(cd));
		} else {	
/* 
 * no activity, hangup 
 */
			NDBGL4(L4_TIMO, "%ld: outgoing-call, var "
				"idle timeout - last activity at %ld", 
					(long)SECOND, (long)i4b_get_idletime(cd));
			
			d->l3_sap->N_DISCONNECT_REQUEST(cd, 
				(CAUSET_I4B << 8) | CAUSE_I4B_NORMAL);
			
			i4b_l4_idle_timeout_ind(cd);
			cd->idletime_state = IST_IDLE;
		}
		break;
	default:
		NDBGL4(L4_ERR, "outgoing-call: var idle "
			"timeout invalid idletime_state value!");
		cd->idletime_state = IST_IDLE;
		break;
	}
}

