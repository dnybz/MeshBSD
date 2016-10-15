# $NetBSD: Makefile,v 1.27 2013/11/09 00:33:48 christos Exp $

WARNS?=	0	# XXX third-party program, many issues
NOCLANGERROR=	# defined

.include <bsd.own.mk>

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

RACOON_SRCTOP= ${.CURDIR}/../..
DIST=	${RACOON_SRCTOP}/external/ipsec-tools
CPPFLAGS+= -I${DIST}/src/racoon -I${DIST}/src/racoon/missing
CPPFLAGS+= -I${DIST}/src/libipsec
CPPFLAGS+= -I${RACOON_SRCTOP}/lib/libipsec -I.
CPPFLAGS+= -DIPSEC_DEBUG -DHAVE_CONFIG_H -DENABLE_WILDCARD_MATCH
CPPFLAGS+= -DADMINPORTDIR=\"/var/run\"
CPPFLAGS+= -DSYSCONFDIR=\"/etc/racoon\"

LIBADD+= l y ipsec util

YHEADER=cfparse.h

CPPFLAGS+=-DHAVE_LIBRADIUS

libadd+= 	radius

.if ${MK_PAM_SUPPORT} != "no"
CPPFLAGS+=-DHAVE_LIBPAM
LIBADD+=	pam util
.endif
.if ${MK_KERBEROS_SUPPORT} != "no"
CPPFLAGS+=-DHAVE_GSSAPI
LIBADD+= gssapi krb5 hx509 heimntlm com_err \
	roken asn1
.endif
CPPFLAGS+=-DHAVE_OPENSSL_IDEA_H
.if (${MKCRYPTO_RC5} != "no")
CPPFLAGS+=-DHAVE_OPENSSL_RC5_H
LDADD+= -lcrypto_rc5
LDADD+= ${LIBCRYPTO_RC5}
.endif
.if ${MK_INET6_SUPPORT} != "no"
CPPFLAGS+=-DINET6
.endif

LIBADD+= crypto crypt

.PATH:  ${RACOON_SRCTOP}/lib/libipsec ${DIST}/src/racoon

prsa_tok.c: ${DIST}/src/racoon/prsa_tok.l
	${LEX} -Pprsa -o${.TARGET} ${.ALLSRC}

prsa_par.c: ${DIST}/src/racoon/prsa_par.y
	${YACC} -pprsa -d -o ${.TARGET} ${.ALLSRC}

.if defined(HAVE_GCC) && ${HAVE_GCC} == 48
.if ${MACHINE_ARCH} == "sh3el"
COPTS.ipsec_doi.c=	-O0
.endif
.endif
.include <bsd.prog.mk>
