# $FreeBSD: head/lib/libnv/Makefile 298107 2016-04-16 07:45:30Z gjb $

PACKAGE=lib${LIB}
SHLIBDIR?= /lib

.include <src.opts.mk>

LIB=	nv
SHLIB_MAJOR= 0

.PATH: ${.CURDIR}/../../sys/libnv ${.CURDIR}/../../sys/sys
CFLAGS+=-I${.CURDIR}/../../sys -I${.CURDIR}

SRCS=	dnvlist.c
SRCS+=	msgio.c
SRCS+=	nvlist.c
SRCS+=	nvpair.c

.include <bsd.lib.mk>
