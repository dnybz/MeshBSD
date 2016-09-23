#
# $FreeBSD: head/sys/modules/hwpmc/Makefile 289853 2015-10-23 21:09:28Z emaste $
#

.PATH: ${.CURDIR}/../../dev/hwpmc

KMOD=	hwpmc

SRCS=	hwpmc_mod.c hwpmc_logging.c hwpmc_soft.c vnode_if.h

.if ${MACHINE_CPUARCH} == "aarch64"
SRCS+=  hwpmc_arm64.c hwpmc_arm64_md.c
.endif

.if ${MACHINE_CPUARCH} == "arm"
SRCS+=	hwpmc_arm.c
.endif

.include <bsd.kmod.mk>
