# $NetBSD: Makefile,v 1.6 2011/05/30 14:41:27 joerg Exp $

.include <src.opts.mk>

RACOON_SRCTOP= 	${.CURDIR}/../..
.PATH: ${RACOON_SRCTOP}/external/ipsec-tools/src/racoon

PROG=	racoonctl
SRCS=	racoonctl.c str2val.c kmpstat.c vmbuf.c sockmisc.c misc.c
MAN=	racoonctl.8

#CFLAGS+=-g
CFLAGS+=	-I$. 
CFLAGS+=	-I${RACOON_SRCTOP}/lib/libipsec
CFLAGS+=	-DHAVE_UNISTD_H
CFLAGS+=	-DHAVE_STDARG_H
CFLAGS+=	-UIPSEC_DEBUG -DHAVE_CONFIG_H -DNOUSE_PRIVSEP	
CFLAGS+=	-DADMINPORTDIR=\"/var/run\"

LIBADD+=	ipsec

CLEANFILES+=	racoonctl.cat8

WARNS?=	0	# XXX third-party program, many issues
NOCLANGERROR=	# defined

.include <bsd.prog.mk>

