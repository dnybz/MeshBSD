#	$OpenBSD: Makefile,v 1.22 2015/10/07 23:33:38 beck Exp $

CFLAGS+= -Wall -Werror -Wimplicit
CFLAGS+= -DLIBRESSL_INTERNAL

LIB=	tls
SHLIB_MAJOR=	11

LIBADD= ssl crypto 

INCS=	tls.h

SRCS=	tls.c \
	tls_client.c \
	tls_config.c \
	tls_conninfo.c \
	tls_peer.c \
	tls_server.c \
	tls_util.c \
	tls_verify.c

MAN=	tls_init.3

WARNS?= 	0

.include <bsd.lib.mk>
