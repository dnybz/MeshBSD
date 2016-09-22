# $FreeBSD: head/sys/modules/dtrace/dtrace/Makefile 278529 2015-02-10 19:41:30Z gnn $

SYSDIR?=	${.CURDIR}/../../..

ARCHDIR=	${MACHINE_CPUARCH}

.PATH: ${SYSDIR}/cddl/contrib/opensolaris/uts/common/dtrace
.PATH: ${SYSDIR}/cddl/compat/opensolaris/kern
.PATH: ${SYSDIR}/cddl/kern
.PATH: ${SYSDIR}/cddl/dev/dtrace
.PATH: ${SYSDIR}/cddl/dev/dtrace/${ARCHDIR}

KMOD=		dtrace
SRCS=		dtrace.c \
		dtrace_asm.S \
		dtrace_subr.c \
		bus_if.h \
		device_if.h \
		vnode_if.h

# Needed for dtrace_asm.S
SRCS+=		assym.s

# These are needed for assym.s
SRCS+=		opt_compat.h opt_kstack_pages.h opt_nfs.h opt_hwpmc_hooks.h

#This is needed for dtrace.c
SRCS += 	opensolaris_taskq.c

CFLAGS+=	-I${SYSDIR}/cddl/compat/opensolaris \
		-I${SYSDIR}/cddl/dev/dtrace \
		-I${SYSDIR}/cddl/dev/dtrace/${ARCHDIR} \
		-I${SYSDIR}/cddl/contrib/opensolaris/uts/common \
		-I${SYSDIR}/cddl/contrib/opensolaris/common/util \
		-I${SYSDIR} -DDIS_MEM

CFLAGS+=	-DSMP

EXPORT_SYMS=	dtrace_register \
		dtrace_unregister \
		dtrace_probe_lookup

dtrace_asm.o:  assym.s

.if ${TARGET_ARCH} == "arm"
assym.o: assym.s
	${AS} -meabi=5 -o assym.o assym.s
.endif

.include <bsd.kmod.mk>

CFLAGS+=	-include ${SYSDIR}/cddl/compat/opensolaris/sys/debug_compat.h

CWARNFLAGS+=	-Wno-parentheses
CWARNFLAGS+=	-Wno-uninitialized
CWARNFLAGS+=	-Wno-cast-qual
CWARNFLAGS+=	-Wno-unused
