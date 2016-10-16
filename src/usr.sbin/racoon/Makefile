# $NetBSD: Makefile,v 1.27 2013/11/09 00:33:48 christos Exp $

.include <src.opts.mk>
 
RACOON_SRCTOP= 	${.CURDIR}/../..
.PATH: ${RACOON_SRCTOP}/external/ipsec-tools/src/racoon

USE_FORT?= yes	# network client and server
PROG=	racoon

SRCS=	main.c session.c isakmp.c handler.c 			\
	isakmp_ident.c isakmp_agg.c isakmp_base.c 		\
	isakmp_quick.c isakmp_inf.c isakmp_newg.c 		\
	gssapi.c dnssec.c getcertsbyname.c privsep.c		\
	pfkey.c admin.c evt.c ipsec_doi.c oakley.c 		\
	grabmyaddr.c vendorid.c policy.c localconf.c 		\
	remoteconf.c crypto_openssl.c algorithm.c		\
	proposal.c sainfo.c strnames.c plog.c logger.c 		\
	schedule.c str2val.c safefile.c backupsa.c 		\
	genlist.c rsalist.c cftoken.l cfparse.y 		\
	prsa_tok.l prsa_par.y isakmp_xauth.c isakmp_cfg.c 	\
	isakmp_unity.c throttle.c isakmp_frag.c 		\
	nattraversal.c vmbuf.c sockmisc.c misc.c
MAN=	racoon.8 racoon.conf.5

CFLAGS+= 	-I$. -I$./missing
CFLAGS+= 	-I${RACOON_SRCTOP}/lib/libipsec
CFLAGS+=	-DHAVE_UNISTD_H
CFLAGS+=	-DHAVE_STDARG_H
CFLAGS+= 	-DIPSEC_DEBUG -DHAVE_CONFIG_H -DENABLE_WILDCARD_MATCH
CFLAGS+= 	-DADMINPORTDIR=\"/var/run\"
CFLAGS+= 	-DSYSCONFDIR=\"/etc/c/racoon\"

LIBADD+= l y ipsec util

YHEADER=cfparse.h

CFLAGS+=-DHAVE_LIBRADIUS

libadd+= 	radius

.if ${MK_PAM_SUPPORT} != "no"
CFLAGS+=-DHAVE_LIBPAM
LIBADD+=	pam util
.endif
.if ${MK_KERBEROS_SUPPORT} != "no"
CFLAGS+=-DHAVE_GSSAPI
LIBADD+= gssapi krb5 hx509 heimntlm com_err \
	roken asn1
.endif
CFLAGS+=-DHAVE_OPENSSL_IDEA_H
.if ${MK_INET6_SUPPORT} != "no"
CFLAGS+=-DINET6
.endif

LIBADD+= crypto crypt

.if defined(HAVE_GCC) && ${HAVE_GCC} == 48
.if ${MACHINE_ARCH} == "sh3el"
COPTS.ipsec_doi.c=	-O0
.endif
.endif

WARNS?=	0	
NOCLANGERROR=	# defined


.include <bsd.prog.mk>
