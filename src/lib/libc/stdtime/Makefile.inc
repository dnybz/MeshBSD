#	Makefile.inc,v 1.2 1994/09/13 21:26:01 wollman Exp
# $FreeBSD: releng/11.0/lib/libc/stdtime/Makefile.inc 297283 2016-03-26 03:46:12Z bdrewery $

.PATH:	${LIBC_SRCTOP}/stdtime ${LIBC_SRCTOP}/../locale \
	${LIBC_SRCTOP}/../../contrib/tzcode/stdtime

SRCS+=	asctime.c difftime.c localtime.c strftime.c strptime.c timelocal.c \
	time32.c

SYM_MAPS+= ${LIBC_SRCTOP}/stdtime/Symbol.map

CFLAGS+= -I${LIBC_SRCTOP}/../../contrib/tzcode/stdtime -I${LIBC_SRCTOP}/stdtime

CFLAGS.localtime.c= -fwrapv

