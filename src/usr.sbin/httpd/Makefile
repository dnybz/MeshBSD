#	$OpenBSD: Makefile,v 1.28 2015/06/23 15:23:14 reyk Exp $

PROG=		httpd
SRCS=		parse.y
SRCS+=		config.c control.c httpd.c log.c logger.c proc.c
SRCS+=		server.c server_http.c server_file.c server_fcgi.c
MAN=		httpd.8 httpd.conf.5

SRCS+=		patterns.c
MAN+=		patterns.7

CFLAGS+= -I${.CURDIR}/../../lib/libevent
CFLAGS+= -I${.CURDIR}/../../lib/libtls
CFLAGS+= -I${.CURDIR}/../../lib/libutil

LIBADD=		event tls ssl crypto util

#DEBUG=		-g -DDEBUG=3 -O0
CFLAGS+=	-Wall -I${.CURDIR}
CFLAGS+=	-Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=	-Wmissing-declarations
CFLAGS+=	-Wshadow -Wpointer-arith
CFLAGS+=	-Wsign-compare
CLEANFILES+=	y.tab.h

WARNS?=	2

.include <bsd.prog.mk>
