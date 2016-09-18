# $FreeBSD: head/sys/boot/libstand32/Makefile 296706 2016-03-11 23:45:51Z bdrewery $

.include <src.opts.mk>

LIBSTAND_SRC=	${.CURDIR}/../../../lib/libstand

LIBSTAND_CPUARCH=${TARGET_ARCH}

LIBC_SRC=	${LIBSTAND_SRC}/../libc
INTERNALLIB=
INCS=
MAN=
.PATH:	${LIBSTAND_SRC}

.include "${LIBSTAND_SRC}/Makefile"

