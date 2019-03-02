# $FreeBSD: releng/11.0/lib/libcasper/libcasper/Makefile 298107 2016-04-16 07:45:30Z gjb $

PACKAGE=${LIB}
LIB=	casper

SHLIB_MAJOR=	0
SHLIBDIR?=	/lib

SRCS=	libcasper.c
SRCS+=	libcasper_impl.c
SRCS+=	libcasper_service.c
SRCS+=	service.c
SRCS+=	zygote.c

INCS=	libcasper.h
INCS+=	libcasper_service.h

LIBADD=	nv

CFLAGS+=-I${.CURDIR}

.include <bsd.lib.mk>
