#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: head/libexec/rlogind/Makefile 298107 2016-04-16 07:45:30Z gjb $

.include <src.opts.mk>

PACKAGE=rcmds
PROG=	rlogind
MAN=	rlogind.8
PACKAGE=rcmds
LIBADD=	util
WARNS?=	2

.if ${MK_INET6_SUPPORT} != "no"
CFLAGS+= -DINET6
.endif

.include <bsd.prog.mk>
