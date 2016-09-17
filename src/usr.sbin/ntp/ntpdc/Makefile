# $FreeBSD: head/usr.sbin/ntp/ntpdc/Makefile 287982 2015-09-19 03:51:19Z bdrewery $

MAN=

.include <src.opts.mk>
.include <bsd.own.mk>

.PATH: ${.CURDIR}/../../../external/ntp/ntpdc

PROG=	ntpdc
SRCS= 	ntpdc.c ntpdc_ops.c ntpdc-opts.c version.c

CFLAGS+= -I${.CURDIR}/../../../external/ntp/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/unix/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/pthreads/include \
	-I${.CURDIR}/../../../external/ntp/sntp/libopts \
	-I${.CURDIR}/../../../lib/libc/${MACHINE_ARCH} \
	-I${.CURDIR}/../ -I${.CURDIR}

LIBADD=	edit ntp m opts pthread
CFLAGS+=	-DHAVE_LIBEDIT -DHAVE_READLINE_READLINE_H \
		-I${DESTDIR}/${INCLUDEDIR}/edit

.if ${MK_LIBRESSL} != "no"
LIBADD+=	crypto
.else
LIBADD+=	md
.endif

CLEANFILES+= .version version.c

version.c:
	sh -e ${.CURDIR}/../scripts/mkver ntpdc

.include <bsd.prog.mk>
