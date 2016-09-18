# $FreeBSD: head/sys/modules/i2c/iicbb/Makefile 294461 2016-01-20 21:54:43Z brooks $

.PATH:		${.CURDIR}/../../../dev/iicbus
.if ${TARGET_ARCH} == "aarch64" || ${TARGET_ARCH} == "arm" || \
    ${TARGET_ARCH} == "mips"
ofw_bus_if=	ofw_bus_if.h
.endif
KMOD		= iicbb
SRCS		= device_if.h bus_if.h iicbus_if.h \
		  iicbb_if.h iicbb_if.c iicbb.c ${ofw_bus_if} opt_platform.h

.include <bsd.kmod.mk>
