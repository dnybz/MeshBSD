# $FreeBSD: releng/11.0/sbin/geom/class/Makefile 265420 2014-05-06 04:22:01Z imp $

.include <src.opts.mk>

SUBDIR=	cache
.if ${MK_LIBRESSL} != "no"
SUBDIR+=eli
.endif
SUBDIR+=journal
SUBDIR+=label
SUBDIR+=mountver
SUBDIR+=multipath
SUBDIR+=nop
SUBDIR+=part
SUBDIR+=shsec

.include <bsd.subdir.mk>
