# $FreeBSD: head/usr.sbin/ntp/ntpq/Makefile 283542 2015-05-25 20:27:46Z bapt $

MAN=

.include <src.opts.mk>
.include <bsd.own.mk>

.PATH: ${.CURDIR}/../../../external/ntp/ntpq

BINDIR=	/usr/bin

PROG=	ntpq
SRCS= 	ntpq.c ntpq-opts.c ntpq-subs.c version.c

CFLAGS+= -I${.CURDIR}/../../../external/ntp/include \
	-I${.CURDIR}/../../../external/ntp/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/unix/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/pthreads/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/${NTP_ATOMIC}/include \
	-I${.CURDIR}/../../../external/ntp/sntp/libopts \
	-I${.CURDIR}/../

LIBADD+=	edit ntp opts m pthread

.if ${MK_LIBRESSL} != "no"
LIBADD+=	crypto
.else
LIBADD+=	md
.endif

CFLAGS+=	-DHAVE_LIBEDIT -DHAVE_READLINE_READLINE_H \
		-I${DESTDIR}/${INCLUDEDIR}/edit

CLEANFILES+= .version version.c

version.c:
	sh -e ${.CURDIR}/../scripts/mkver ntpq

.include <bsd.prog.mk>
