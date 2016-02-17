# $FreeBSD: head/sbin/geom/class/Makefile 265420 2014-05-06 04:22:01Z imp $

.include <src.opts.mk>

SUBDIR=	cache
.if ${MK_OPENSSL} != "no"
SUBDIR+=eli
.endif
SUBDIR+=journal
SUBDIR+=label
SUBDIR+=mountver
SUBDIR+=nop
SUBDIR+=part
SUBDIR+=sched
SUBDIR+=shsec
SUBDIR+=virstor

.include <bsd.subdir.mk>
