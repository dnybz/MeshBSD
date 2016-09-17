# $FreeBSD: head/usr.sbin/ntp/ntp-keygen/Makefile 283542 2015-05-25 20:27:46Z bapt $

MAN=

.include <src.opts.mk>

.PATH:	${.CURDIR}/../../../external/ntp/util \
	${.CURDIR}/../../../external/ntp/ntpd

PROG=	ntp-keygen
SRCS=	ntp-keygen.c ntp-keygen-opts.c

CFLAGS+= -I${.CURDIR}/../../../external/ntp/include \
	-I${.CURDIR}/../../../external/ntp/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/unix/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/pthreads/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/${NTP_ATOMIC}/include \
	-I${.CURDIR}/../../../external/ntp/sntp/libopts \
	-I${.CURDIR}/../../../lib/libc/${MACHINE_ARCH} \
	-I${.CURDIR}/../

LIBADD+=	ntp opts pthread

.if ${MK_LIBRESSL} != "no"
LIBADD+=	crypto
.endif

.include <bsd.prog.mk>
