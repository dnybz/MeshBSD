# $FreeBSD: head/etc/mtree/Makefile 293068 2016-01-03 04:32:05Z imp $

.include <src.opts.mk>

FILES=	BSD.include.dist \
	BSD.root.dist \
	${_BSD.lib32.dist} \
	${_BSD.libsoft.dist} \
	BSD.usr.dist \
	BSD.var.dist

.if ${MK_GROFF} != "no"
_BSD.groff.dist=	BSD.groff.dist
.endif
.if ${MK_LIB32} != "no"
_BSD.lib32.dist=	BSD.lib32.dist
.endif
.if ${MK_LIBSOFT} != "no"
_BSD.libsoft.dist=	BSD.libsoft.dist
.endif

NO_OBJ=
FILESDIR= /etc/mtree

.include <bsd.prog.mk>
