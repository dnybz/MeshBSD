# $FreeBSD: releng/11.0/lib/libdevstat/Makefile 298107 2016-04-16 07:45:30Z gjb $

PACKAGE=lib${LIB}
LIB=	devstat
SHLIBDIR?= /lib
# Bump DEVSTAT_USER_API_VER in devstat.h every time this is incremented.
SHLIB_MAJOR= 7
SRCS=	devstat.c
INCS=	devstat.h

LIBADD=	kvm

CFLAGS+=-I${.CURDIR}

WARNS?=	3

.include <bsd.lib.mk>
