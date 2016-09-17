# $FreeBSD: head/usr.sbin/ntp/libopts/Makefile 280849 2015-03-30 13:30:15Z cy $

.PATH: ${.CURDIR}/../../../external/ntp/sntp/libopts 

LIB=	opts
INTERNALLIB=

SRCS=	libopts.c

CFLAGS+= -I${.CURDIR}/../../../external/ntp/include \
	-I${.CURDIR}/../../../external/ntp/sntp/libopts \
	-I${.CURDIR}/../

.include <bsd.lib.mk>
