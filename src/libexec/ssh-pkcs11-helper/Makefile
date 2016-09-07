# $FreeBSD: head/secure/libexec/ssh-pkcs11-helper/Makefile 296633 2016-03-11 00:15:29Z des $

.include <src.opts.mk>

SSHDIR= 	${.CURDIR}/../../external/openssh

PROG=	ssh-pkcs11-helper
SRCS=	ssh-pkcs11.c ssh-pkcs11-helper.c
MAN=	ssh-pkcs11-helper.8
CFLAGS+=-I${SSHDIR} -include ssh_namespace.h
SRCS+=	ssh_namespace.h

LIBADD=	ssh

.if ${MK_LDNS} != "no"
CFLAGS+=	-DHAVE_LDNS=1
#DPADD+=	${LIBLDNS}
#LDADD+=	-lldns
.endif

LIBADD+=	crypto

WARNS?= 	0

.include <bsd.prog.mk>

.PATH:	${SSHDIR}
