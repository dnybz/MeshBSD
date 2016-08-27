# $FreeBSD: head/etc/devd/Makefile 298140 2016-04-17 03:45:45Z gjb $

.include <src.opts.mk>

FILEGROUPS=	FILES

.if ${MK_USB} != "no"
FILES+=	uath.conf ulpt.conf usb.conf
.endif

NO_OBJ=
FILESDIR=	/etc/devd
FILESMODE=	644

.include <bsd.prog.mk>
