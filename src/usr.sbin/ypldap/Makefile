# $OpenBSD: Makefile,v 1.8 2015/09/09 15:33:18 deraadt Exp $
# $FreeBSD: head/usr.sbin/ypldap/Makefile 290932 2015-11-16 16:51:18Z rodrigc $

PROG=		ypldap
SRCS=		parse.y ypldap.c log.c	\
		ldapclient.c entries.c yp.c \
		aldap.c ber.c \
		ypldap_dns.c

MAN=		ypldap.8 ypldap.conf.5

LIBADD=		event util rpcsvc

CFLAGS+=-I${.CURDIR}
CFLAGS+=-I${.CURDIR}/../../lib/libevent
CFLAGS+=-I${.CURDIR}/../../lib/libutil

WARNS=	2

.include <bsd.prog.mk>
