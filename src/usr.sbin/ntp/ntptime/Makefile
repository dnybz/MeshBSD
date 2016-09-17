# $FreeBSD: head/usr.sbin/ntp/ntptime/Makefile 280849 2015-03-30 13:30:15Z cy $

.PATH: ${.CURDIR}/../../../external/ntp/util

PROG=	ntptime
MAN=

CFLAGS+= -I${.CURDIR}/../../../external/ntp/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/include/ \
	-I${.CURDIR}/../../../external/ntp/lib/isc/unix/include/ \
	-I${.CURDIR}/../../../external/ntp/lib/isc/pthreads/include \
	-I${.CURDIR}/../

LIBADD=	ntp pthread

.include <bsd.prog.mk>
