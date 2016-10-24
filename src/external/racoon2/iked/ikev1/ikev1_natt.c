/*
 * Copyright (C) 2004 SuSE Linux AG, Nuernberg, Germany.
 * Contributed by: Michal Ludvig <mludvig@suse.cz>, SUSE Labs
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <config.h>

#include <sys/types.h>
#include <sys/param.h>

#include <netinet/in.h>
#ifdef __linux__
#include <linux/udp.h>
#endif
#if defined(__NetBSD__) || defined (__FreeBSD__)
#include <netinet/udp.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "racoon.h"

#include "var.h"
/* #include "misc.h" */
/* #include "vmbuf.h" */
#include "plog.h"
#include "debug.h"

/* #include "localconf.h" */
#include "remoteconf.h"
#include "sockmisc.h"
#include "isakmp.h"
#include "isakmp_var.h"
#include "isakmp_impl.h"
#include "ikev1_impl.h"
#include "oakley.h"
#include "ipsec_doi.h"
#include "vendorid.h"
#include "handler.h"
#include "crypto_impl.h"
#include "ikev1_natt.h"
/* #include "grabmyaddr.h" */

#include "ike_conf.h"

int ikev1_natt_ka_interval = IKEV1_DEFAULT_NATK_INTERVAL;

struct natt_ka_addrs {
	struct sockaddr *src;
	struct sockaddr *dst;
	unsigned in_use;

	TAILQ_ENTRY(natt_ka_addrs) chain;
};

static TAILQ_HEAD(_natt_ka_addrs, natt_ka_addrs) ka_tree;

/*
 * check if the given vid is NAT-T.
 */
int
natt_vendorid(int vid)
{
	return (
#ifdef ENABLE_NATT_00
		       vid == VENDORID_NATT_00 ||
#endif
#ifdef ENABLE_NATT_01
		       vid == VENDORID_NATT_01 ||
#endif
#ifdef ENABLE_NATT_02
		       vid == VENDORID_NATT_02 ||
		       vid == VENDORID_NATT_02_N ||
#endif
#ifdef ENABLE_NATT_03
		       vid == VENDORID_NATT_03 ||
#endif
#ifdef ENABLE_NATT_04
		       vid == VENDORID_NATT_04 ||
#endif
#ifdef ENABLE_NATT_05
		       vid == VENDORID_NATT_05 ||
#endif
#ifdef ENABLE_NATT_06
		       vid == VENDORID_NATT_06 ||
#endif
#ifdef ENABLE_NATT_07
		       vid == VENDORID_NATT_07 ||
#endif
#ifdef ENABLE_NATT_08
		       vid == VENDORID_NATT_08 ||
#endif
		       /* Always enable NATT RFC if ENABLE_NATT
		        */
		       vid == VENDORID_NATT_RFC);
}

rc_vchar_t *
ikev1_natt_hash_addr(struct ph1handle *iph1, struct sockaddr *addr)
{
	rc_vchar_t *natd;
	rc_vchar_t *buf;
	char *ptr;
	void *addr_ptr, *addr_port;
	size_t buf_size, addr_size;

	plog(PLOG_INFO, PLOGLOC, 0, "Hashing %s with algo #%d %s\n",
	     rcs_sa2str(addr), iph1->approval->hashtype,
	     ikev1_nat_traversal(iph1->rmconf) ==
	     NATT_FORCE ? "(NAT-T forced)" : "");

	if (addr->sa_family == AF_INET) {
		addr_size = sizeof(struct in_addr);	/* IPv4 address */
		addr_ptr = &((struct sockaddr_in *)addr)->sin_addr;
		addr_port = &((struct sockaddr_in *)addr)->sin_port;
	} else if (addr->sa_family == AF_INET6) {
		addr_size = sizeof(struct in6_addr);	/* IPv6 address */
		addr_ptr = &((struct sockaddr_in6 *)addr)->sin6_addr;
		addr_port = &((struct sockaddr_in6 *)addr)->sin6_port;
	} else {
		plog(PLOG_INTERR, PLOGLOC, 0,
		     "Unsupported address family #0x%x\n", addr->sa_family);
		return NULL;
	}

	buf_size = 2 * sizeof(isakmp_cookie_t);	/* CKY-I + CKY+R */
	buf_size += addr_size + 2;	/* Address + Port */

	if ((buf = rc_vmalloc(buf_size)) == NULL)
		return NULL;

	ptr = buf->v;

	/* Copy-in CKY-I */
	memcpy(ptr, iph1->index.i_ck, sizeof(isakmp_cookie_t));
	ptr += sizeof(isakmp_cookie_t);

	/* Copy-in CKY-I */
	memcpy(ptr, iph1->index.r_ck, sizeof(isakmp_cookie_t));
	ptr += sizeof(isakmp_cookie_t);

	/* Copy-in Address (or zeroes if NATT_FORCE) */
	if (ikev1_nat_traversal(iph1->rmconf) == NATT_FORCE)
		memset(ptr, 0, addr_size);
	else
		memcpy(ptr, addr_ptr, addr_size);

	ptr += addr_size;

	/* Copy-in Port number */
	memcpy(ptr, addr_port, 2);

	natd = oakley_hash(buf, iph1);
	rc_vfree(buf);

	return natd;
}

int
ikev1_natt_compare_addr_hash(struct ph1handle *iph1, rc_vchar_t *natd_received,
			     int natd_seq)
{
	rc_vchar_t *natd_computed;
	uint32_t flag;
	int verified = 0;

	if (ikev1_nat_traversal(iph1->rmconf) == NATT_FORCE)
		return verified;

	if (natd_seq == 0) {
		natd_computed = ikev1_natt_hash_addr(iph1, iph1->local);
		flag = NAT_DETECTED_ME;
	} else {
		natd_computed = ikev1_natt_hash_addr(iph1, iph1->remote);
		flag = NAT_DETECTED_PEER;
	}

	if (natd_received->l == natd_computed->l &&
	    memcmp(natd_received->v, natd_computed->v, natd_received->l) == 0) {
		iph1->natt_flags &= ~flag;
		verified = 1;
	}

	rc_vfree(natd_computed);

	return verified;
}

int
ikev1_natt_udp_encap(int encmode)
{
	return (encmode == IPSECDOI_ATTR_ENC_MODE_UDPTUNNEL_RFC ||
		encmode == IPSECDOI_ATTR_ENC_MODE_UDPTRNS_RFC ||
		encmode == IPSECDOI_ATTR_ENC_MODE_UDPTUNNEL_DRAFT ||
		encmode == IPSECDOI_ATTR_ENC_MODE_UDPTRNS_DRAFT);
}

static int
natt_fill_options(struct ph1natt_options *opts, int version)
{
	uint16_t port_isakmp_natt = PORT_ISAKMP_NATT;

	if (!opts)
		return -1;

	opts->version = version;

	switch (version) {
	case VENDORID_NATT_00:
	case VENDORID_NATT_01:
		opts->float_port = 0;	/* No port floating for those drafts */
		opts->payload_nat_d = ISAKMP_NPTYPE_NATD_DRAFT;
		opts->payload_nat_oa = ISAKMP_NPTYPE_NATOA_DRAFT;
		opts->mode_udp_tunnel = IPSECDOI_ATTR_ENC_MODE_UDPTUNNEL_DRAFT;
		opts->mode_udp_transport = IPSECDOI_ATTR_ENC_MODE_UDPTRNS_DRAFT;
		opts->encaps_type = UDP_ENCAP_ESPINUDP_NON_IKE;
		break;

	case VENDORID_NATT_02:
	case VENDORID_NATT_02_N:
	case VENDORID_NATT_03:
		opts->float_port = port_isakmp_natt;
		opts->payload_nat_d = ISAKMP_NPTYPE_NATD_DRAFT;
		opts->payload_nat_oa = ISAKMP_NPTYPE_NATOA_DRAFT;
		opts->mode_udp_tunnel = IPSECDOI_ATTR_ENC_MODE_UDPTUNNEL_DRAFT;
		opts->mode_udp_transport = IPSECDOI_ATTR_ENC_MODE_UDPTRNS_DRAFT;
		opts->encaps_type = UDP_ENCAP_ESPINUDP;
		break;

	case VENDORID_NATT_04:
	case VENDORID_NATT_05:
	case VENDORID_NATT_06:
	case VENDORID_NATT_07:
	case VENDORID_NATT_08:
		opts->float_port = port_isakmp_natt;
		opts->payload_nat_d = ISAKMP_NPTYPE_NATD_BADDRAFT;
		opts->payload_nat_oa = ISAKMP_NPTYPE_NATOA_BADDRAFT;
		opts->mode_udp_tunnel = IPSECDOI_ATTR_ENC_MODE_UDPTUNNEL_RFC;
		opts->mode_udp_transport = IPSECDOI_ATTR_ENC_MODE_UDPTRNS_RFC;
		opts->encaps_type = UDP_ENCAP_ESPINUDP;
		break;

	case VENDORID_NATT_RFC:
		opts->float_port = port_isakmp_natt;
		opts->payload_nat_d = ISAKMP_NPTYPE_NATD_RFC;
		opts->payload_nat_oa = ISAKMP_NPTYPE_NATOA_RFC;
		opts->mode_udp_tunnel = IPSECDOI_ATTR_ENC_MODE_UDPTUNNEL_RFC;
		opts->mode_udp_transport = IPSECDOI_ATTR_ENC_MODE_UDPTRNS_RFC;
		opts->encaps_type = UDP_ENCAP_ESPINUDP;
		break;

	default:
		plog(PLOG_INTERR, PLOGLOC, NULL,
		     "unsupported NAT-T version: %s\n",
		     vid_string_by_id(version));
		return -1;
	}

	opts->mode_udp_diff =
		opts->mode_udp_tunnel - IPSECDOI_ATTR_ENC_MODE_TUNNEL;

	return 0;
}

void
ikev1_natt_float_ports(struct ph1handle *iph1)
{
	if (!(iph1->natt_flags && NAT_DETECTED))
		return;
	if (!iph1->natt_options->float_port) {
		/* Drafts 00 / 01, just schedule keepalive */
		natt_keepalive_add_ph1(iph1);
		return;
	}

	set_port(iph1->local, iph1->natt_options->float_port);
	set_port(iph1->remote, iph1->natt_options->float_port);
	iph1->natt_flags |= NAT_PORTS_CHANGED | NAT_ADD_NON_ESP_MARKER;

	natt_keepalive_add_ph1(iph1);
}

void
ikev1_natt_handle_vendorid(struct ph1handle *iph1, int vid_numeric)
{
	if (!iph1->natt_options)
		iph1->natt_options =
			racoon_calloc(1, sizeof(*iph1->natt_options));

	if (!iph1->natt_options) {
		plog(PLOG_INTERR, PLOGLOC, NULL,
		     "Allocating memory for natt_options failed!\n");
		return;
	}

	if (iph1->natt_options->version < vid_numeric)
		if (natt_fill_options(iph1->natt_options, vid_numeric) == 0)
			iph1->natt_flags |= NAT_ANNOUNCED;
}

/* NAT keepalive functions */
static void
natt_keepalive_send(void *param)
{
	struct natt_ka_addrs *ka, *next = NULL;
	char keepalive_packet[] = { 0xff };
	int len;
	int s;

	for (ka = TAILQ_FIRST(&ka_tree); ka; ka = next) {
		next = TAILQ_NEXT(ka, chain);

		s = getsockmyaddr(ka->src);
		if (s == -1) {
			TAILQ_REMOVE(&ka_tree, ka, chain);
			racoon_free(ka);
			continue;
		}
		plog(PLOG_DEBUG, PLOGLOC, NULL, "KA: %s->%s\n",
		     rcs_sa2str(ka->src), rcs_sa2str(ka->dst));
		len = sendfromto(s, keepalive_packet, sizeof(keepalive_packet),
				 ka->src, ka->dst, 1);
		if (len == -1)
			plog(PLOG_INTERR, PLOGLOC, NULL,
			     "KA: sendfromto failed: %s\n", strerror(errno));
	}

	sched_new(ikev1_natt_ka_interval, natt_keepalive_send, NULL);
}

void
natt_keepalive_init(void)
{
	TAILQ_INIT(&ka_tree);

	/* To disable sending KAs set natt_ka_interval=0 */
	if (ikev1_natt_ka_interval > 0)
		sched_new(ikev1_natt_ka_interval, natt_keepalive_send, NULL);
}

int
natt_keepalive_add(struct sockaddr *src, struct sockaddr *dst)
{
	struct natt_ka_addrs *ka = NULL, *new_addr;

	TAILQ_FOREACH(ka, &ka_tree, chain) {
		if (rcs_cmpsa(ka->src, src) == 0 &&
		    rcs_cmpsa(ka->dst, dst) == 0) {
			ka->in_use++;
			plog(PLOG_INFO, PLOGLOC, NULL,
			     "KA found: %s->%s (in_use=%u)\n", rcs_sa2str(src),
			     rcs_sa2str(dst), ka->in_use);
			return 0;
		}
	}

	plog(PLOG_INFO, PLOGLOC, NULL, "KA list add: %s->%s\n",
	     rcs_sa2str(src), rcs_sa2str(dst));

	new_addr = (struct natt_ka_addrs *)racoon_malloc(sizeof(*new_addr));
	if (!new_addr) {
		plog(PLOG_INTERR, PLOGLOC, NULL,
		     "Can't allocate new KA list item\n");
		return -1;
	}

	new_addr->src = rcs_sadup(src);
	new_addr->dst = rcs_sadup(dst);
	new_addr->in_use = 1;
	TAILQ_INSERT_TAIL(&ka_tree, new_addr, chain);

	return 0;
}

int
natt_keepalive_add_ph1(struct ph1handle *iph1)
{
	int ret = 0;

	/* Should only the NATed host send keepalives?
	 * If yes, add '(iph1->natt_flags & NAT_DETECTED_ME)'
	 * to the following condition. */
	if (iph1->natt_flags & NAT_DETECTED &&
	    !(iph1->natt_flags & NAT_KA_QUEUED)) {
		ret = natt_keepalive_add(iph1->local, iph1->remote);
		if (ret == 0)
			iph1->natt_flags |= NAT_KA_QUEUED;
	}

	return ret;
}

void
natt_keepalive_remove(struct sockaddr *src, struct sockaddr *dst)
{
	struct natt_ka_addrs *ka, *next = NULL;

	plog(PLOG_INFO, PLOGLOC, NULL, "KA remove: %s->%s\n",
	     rcs_sa2str(src), rcs_sa2str(dst));

	for (ka = TAILQ_FIRST(&ka_tree); ka; ka = next) {
		next = TAILQ_NEXT(ka, chain);

		plog(PLOG_DEBUG, PLOGLOC, NULL,
		     "KA tree dump: %s->%s (in_use=%u)\n", rcs_sa2str(src),
		     rcs_sa2str(dst), ka->in_use);

		if (rcs_cmpsa(ka->src, src) == 0 &&
		    rcs_cmpsa(ka->dst, dst) == 0 && --ka->in_use <= 0) {

			plog(PLOG_DEBUG, PLOGLOC, NULL,
			     "KA removing this one...\n");

			TAILQ_REMOVE(&ka_tree, ka, chain);
			racoon_free(ka);
			/* Should we break here? Every pair of addresses should 
			 * be inserted only once, but who knows :-) Lets traverse 
			 * the whole list... */
		}
	}
}

#ifdef notyet
static struct remoteconf *
natt_enabled_in_rmconf_stub(struct remoteconf *rmconf, void *data)
{
	return (ikev1_nat_traversal(rmconf) == NATT_OFF ? NULL : rmconf);
}

int
natt_enabled_in_rmconf()
{
	return foreachrmconf(natt_enabled_in_rmconf_stub, NULL) != NULL;
}
#endif

struct payload_list *
isakmp_plist_append_natt_vids(struct payload_list *plist,
			      rc_vchar_t *vid_natt[MAX_NATT_VID_COUNT])
{
	int i, vid_natt_i = 0;

	if (vid_natt == NULL)
		return NULL;

	for (i = 0; i < MAX_NATT_VID_COUNT; i++)
		vid_natt[i] = NULL;

	/*
	 * Puts the olders VIDs last, as some implementations may choose
	 * the first NATT VID given.
	 */

	/* Always set RFC VID
	 */
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_RFC)) != NULL)
		vid_natt_i++;
#ifdef ENABLE_NATT_08
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_08)) != NULL)
		vid_natt_i++;
#endif
#ifdef ENABLE_NATT_07
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_07)) != NULL)
		vid_natt_i++;
#endif
#ifdef ENABLE_NATT_06
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_06)) != NULL)
		vid_natt_i++;
#endif
#ifdef ENABLE_NATT_05
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_05)) != NULL)
		vid_natt_i++;
#endif
#ifdef ENABLE_NATT_04
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_04)) != NULL)
		vid_natt_i++;
#endif
#ifdef ENABLE_NATT_03
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_03)) != NULL)
		vid_natt_i++;
#endif
#ifdef ENABLE_NATT_02
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_02)) != NULL)
		vid_natt_i++;
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_02_N)) != NULL)
		vid_natt_i++;
#endif
#ifdef ENABLE_NATT_01
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_01)) != NULL)
		vid_natt_i++;
#endif
#ifdef ENABLE_NATT_00
	if ((vid_natt[vid_natt_i] = set_vendorid(VENDORID_NATT_00)) != NULL)
		vid_natt_i++;
#endif
	/* set VID payload for NAT-T */
	for (i = 0; i < vid_natt_i; i++)
		plist = isakmp_plist_append(plist, vid_natt[i],
					    ISAKMP_NPTYPE_VID);

	return plist;
}
