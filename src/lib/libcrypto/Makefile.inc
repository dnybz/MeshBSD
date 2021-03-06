# $MeshBSD$

.include <bsd.own.mk>

OPENSSL_VER=	2.5.0
OPENSSL_DATE=	

LIBRESSL_SRC=	${.CURDIR}/../../contrib/libressl
LCRYPTO_SRC=	${LIBRESSL_SRC}/crypto
LCRYPTO_DOC=	${LCRYPTO_SRC}/doc
LSSL_SRC=	${LIBRESSL_SRC}/ssl

CFLAGS+=	-DTERMIOS -DANSI_SOURCE
CFLAGS+=	-I${LCRYPTO_SRC} -I${LSSL_SRC} -I${.OBJDIR}
CFLAGS+=	-I${LIBRESSL_SRC}/include -I${LIBRESSL_SRC}/include/compat
CFLAGS+=	-DDSO_DLFCN -DHAVE_DLFCN_H
CFLAGS+=	-DLIBRESSL_INTERNAL

.if 2 == 1
.endif

MANDIR= 	${LIBRESSL_SRC}/man

.if defined(LIB)
_docs=	${LIB}
_skip=	SSLeay_version des_modes
_sec=	3
.else
_docs=	apps
_skip=	config
_sec=	1
.endif

