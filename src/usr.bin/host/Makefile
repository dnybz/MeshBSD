# $FreeBSD: head/usr.bin/host/Makefile 275042 2014-11-25 14:29:10Z bapt $

LDNSDIR=	${.CURDIR}/../../external/ldns
LDNSHOSTDIR=	${.CURDIR}/../../external/ldns-host

.PATH:		${LDNSHOSTDIR}

PROG=		host
SRCS=		ldns-host.c
MAN=		host.1
CLEANFILES+=	host.1

host.1: ldns-host.1
	sed -e 's/ldns-//gI' <${.ALLSRC} >${.TARGET} || \
		(rm -rf ${.TARGET} ; false)

CFLAGS+=	-I${LDNSDIR}
LIBADD=		ldns

.include <bsd.prog.mk>
