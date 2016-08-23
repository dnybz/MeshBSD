# $FreeBSD: head/sys/modules/bhnd/cores/bhnd_chipc/Makefile 299241 2016-05-08 19:14:05Z adrian $

.PATH: ${.CURDIR}/../../../../dev/bhnd/cores/chipc

KMOD=	bhnd_chipc
SRCS=	chipc.c
SRCS+=	device_if.h bus_if.h bhnd_bus_if.h \
	bhnd_chipc_if.h bhnd_nvram_if.h

.include <bsd.kmod.mk>
