# $FreeBSD: head/sys/modules/dtrace/fbt/Makefile 270067 2014-08-16 21:42:55Z markj $

SYSDIR?=	${.CURDIR}/../../..

.PATH:		${SYSDIR}/cddl/dev/fbt

KMOD=		fbt
SRCS=		fbt.c fbt_isa.c
SRCS+=		vnode_if.h


CFLAGS+=	-I${SYSDIR}/cddl/dev/fbt/${TARGET_ARCH}
.PATH:		${SYSDIR}/cddl/dev/fbt/${TARGET_ARCH}

CFLAGS+=	-I${SYSDIR}/cddl/dev/fbt \
		-I${SYSDIR}/cddl/compat/opensolaris \
		-I${SYSDIR}/cddl/contrib/opensolaris/uts/common \
		-I${SYSDIR}

.include <bsd.kmod.mk>

CFLAGS+=	-include ${SYSDIR}/cddl/compat/opensolaris/sys/debug_compat.h
