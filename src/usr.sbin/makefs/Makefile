#	$FreeBSD: head/usr.sbin/makefs/Makefile 289739 2015-10-22 05:32:45Z ngie $

.include <src.opts.mk>

PROG=	makefs

CFLAGS+=-I${.CURDIR}

SRCS=	ffs.c \
	makefs.c \
	mtree.c \
	walk.c
MAN=	makefs.8

WARNS?=	2

.include "${.CURDIR}/ffs/Makefile.inc"

CFLAGS+=-DHAVE_STRUCT_STAT_ST_FLAGS=1
CFLAGS+=-DHAVE_STRUCT_STAT_ST_GEN=1

.PATH: ${.CURDIR}/../../tools/mtree
CFLAGS+=-I${.CURDIR}/../../tools/mtree
SRCS+=	getid.c misc.c spec.c

.PATH: ${.CURDIR}/../../tools/mknod
CFLAGS+=-I${.CURDIR}/../../tools/mknod
SRCS+=	pack_dev.c

.PATH:	${.CURDIR}/../../sys/ufs/ffs
SRCS+=	ffs_tables.c

CFLAGS+=	-I${.CURDIR}/../../lib/libnetbsd
LIBADD=		netbsd util sbuf

.include <bsd.prog.mk>
