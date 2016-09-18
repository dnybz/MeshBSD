#	@(#)Makefile	8.2 (Berkeley) 3/17/94
# $FreeBSD: head/sbin/bsdlabel/Makefile 298107 2016-04-16 07:45:30Z gjb $

.PATH: ${.CURDIR}/../../sys/geom

PACKAGE=runtime
PROG=	bsdlabel
SRCS=	bsdlabel.c geom_bsd_enc.c
MAN+=	bsdlabel.8

LIBADD=	geom

.include <bsd.prog.mk>
