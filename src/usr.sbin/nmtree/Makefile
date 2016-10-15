#	$FreeBSD: head/usr.sbin/nmtree/Makefile 275054 2014-11-25 16:57:27Z bapt $

.include <src.opts.mk>

PROG=		mtree
MAN=		mtree.5 mtree.8
SRCS=		compare.c crc.c create.c excludes.c getid.c misc.c mtree.c \
		only.c spec.c specspec.c verify.c

CFLAGS+=	-I${.CURDIR}/../../sbin/mknod
.PATH:		${.CURDIR}/../../sbin/mknod
SRCS+=		pack_dev.c

CFLAGS+=	-I${.CURDIR}/../../lib/libmd
CFLAGS+=	-I${.CURDIR}/../../lib/libutil
LIBADD=		md util

LINKS=		${BINDIR}/mtree ${BINDIR}/nmtree
MLINKS=		mtree.8 nmtree.8

.include <bsd.prog.mk>
