#	@(#)Makefile	8.1 (Berkeley) 6/6/93
# $FreeBSD: head/usr.bin/xinstall/Makefile 299244 2016-05-08 21:11:24Z jilles $

.include <src.opts.mk>

PROG=		xinstall
PROGNAME=	install
SRCS=		xinstall.c getid.c
MAN=		install.1

.PATH:		${.CURDIR}/../../tools/mtree
CFLAGS+=	-I${.CURDIR}/../../tools/mtree
CFLAGS+=	-I${.CURDIR}/../../lib/libnetbsd

LIBADD=	md

.include <bsd.prog.mk>
