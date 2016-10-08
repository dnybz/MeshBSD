# $FreeBSD: head/lib/csu/Makefile 283125 2015-05-19 21:00:53Z emaste $

LIBCSU_SRCTOP?= ${.CURDIR}

.if exists(${LIBCSU_SRCTOP}/${MACHINE_ARCH})
SUBDIR+= ${MACHINE_ARCH}
.elif exists(${LIBCSU_SRCTOP}/${MACHINE_CPUARCH})
SUBDIR+= ${MACHINE_CPUARCH}
.else
SUBDIR+= ${TARGET_ARCH}
.endif

.include <bsd.subdir.mk>






