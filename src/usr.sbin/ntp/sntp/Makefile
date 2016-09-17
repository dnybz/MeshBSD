# $FreeBSD: head/usr.sbin/ntp/sntp/Makefile 283542 2015-05-25 20:27:46Z bapt $

.include <src.opts.mk>

.PATH:	${.CURDIR}/../../../external/ntp/sntp

PROG=	sntp
MK_MAN=	no
SRCS= 	crypto.c kod_management.c log.c main.c networking.c \
	sntp-opts.c sntp.c utilities.c

CFLAGS+= -I${.CURDIR}/../../../external/ntp/include \
	-I${.CURDIR}/../../../external/ntp/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/unix/include \
	-I${.CURDIR}/../../../external/ntp/lib/isc/pthreads/include \
	-I${.CURDIR}/../../../external/ntp/sntp \
	-I${.CURDIR}/../../../external/ntp/sntp/libopts \
	-I${.CURDIR}/../../../external/ntp/sntp/libevent/include \
	-I${.CURDIR}/../libntpevent \
	-I${.CURDIR}/../

LIBADD=	m opts ntp ntpevent pthread

.if ${MK_LIBRESSL} != "no"
LIBADD+=	crypto
.else
LIBADD+=	md
.endif

.include <bsd.prog.mk>
