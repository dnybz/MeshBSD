# $FreeBSD: head/usr.sbin/ntp/ntpdate/Makefile 283542 2015-05-25 20:27:46Z bapt $

.include <src.opts.mk>

.PATH: ${.CURDIR}/../../../external/ntp/ntpdate

PROG=	ntpdate
MAN=
SRCS=	ntpdate.c version.c

CFLAGS+= -I${.CURDIR}/../../../external/ntp/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/include/ \
	-I${.CURDIR}/../../../external/ntp/lib/isc/unix/include/ \
	-I${.CURDIR}/../../../external/ntp/lib/isc/pthreads/include \
	-I${.CURDIR}/../

LIBADD=	ntp m pthread

.if ${MK_LIBRESSL} != "no"
LIBADD+=	crypto
.else
LIBADD+=	md
.endif

CLEANFILES+= .version version.c

version.c: 
	sh -e ${.CURDIR}/../scripts/mkver ntpdate

.include <bsd.prog.mk>
