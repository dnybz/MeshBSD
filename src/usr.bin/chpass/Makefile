#	@(#)Makefile	8.2 (Berkeley) 4/2/94
# $FreeBSD: releng/11.0/usr.bin/chpass/Makefile 298107 2016-04-16 07:45:30Z gjb $

.include <src.opts.mk>

.PATH: ${.CURDIR}/../../usr.sbin/pwd_mkdb ${.CURDIR}/../../lib/libc/gen

PROG=	chpass
SRCS=	chpass.c edit.c field.c pw_scan.c table.c util.c
BINOWN=	root
BINMODE=4555
PRECIOUSPROG=

#Some people need this, uncomment to activate
#CFLAGS+=-DRESTRICT_FULLNAME_CHANGE
CFLAGS+=-I${.CURDIR}/../../usr.sbin/pwd_mkdb -I${.CURDIR}/../../lib/libc/gen -I.

LIBADD=	crypt util

SYMLINKS=	${BINDIR}/chpass ${BINDIR}/chfn
SYMLINKS+=	${BINDIR}/chpass ${BINDIR}/chsh

MLINKS=	chpass.1 chfn.1 chpass.1 chsh.1

beforeinstall:
.for i in chpass chfn chsh ypchpass ypchfn ypchsh
.if exists(${DESTDIR}${BINDIR}/$i)
	-chflags noschg ${DESTDIR}${BINDIR}/$i
.endif
.endfor

.include <bsd.prog.mk>
