#	@(#)Makefile	8.1 (Berkeley) 6/6/93
# $FreeBSD: head/usr.bin/xinstall/Makefile 299244 2016-05-08 21:11:24Z jilles $

.include <src.opts.mk>

PROG=		xinstall
PROGNAME=	install
MAN=		install.1

SRCS=		xinstall.c getid.c

CFLAGS+=	-I${.CURDIR}/../../usr.sbin/nmtree
.PATH:		${.CURDIR}/../../usr.sbin/nmtree
CFLAGS+=	-I${.CURDIR}/../../lib/libutil

LIBADD=	md 

.include <bsd.prog.mk>
