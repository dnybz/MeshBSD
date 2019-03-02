# $FreeBSD: releng/11.0/lib/libexecinfo/Makefile 298107 2016-04-16 07:45:30Z gjb $

PACKAGE=lib${LIB}
LIBEXECINFO=	${.CURDIR}/../../contrib/libexecinfo

LIB=		execinfo
SHLIB_MAJOR=	1

.PATH:		${LIBEXECINFO}

INCS=		execinfo.h
SRCS=		backtrace.c symtab.c unwind.c

LIBADD=		elf

.include <bsd.lib.mk>
