# $FreeBSD: head/gnu/usr.bin/diff/Makefile 279520 2015-03-02 11:49:01Z bapt $

.include <src.opts.mk>

DIFFSRC=${.CURDIR}/../../../tools/diff/src
.PATH: ${DIFFSRC} \
       ${.CURDIR}/../../../tools/diff/lib \
       ${.CURDIR}/../../../tools/diff/man \
       ${.CURDIR}/../../../tools/diff/doc

PROG=	diff
SRCS=   analyze.c context.c diff.c dir.c ed.c ifdef.c io.c \
	normal.c side.c util.c \
	xmalloc.c strtoumax.c cmpbuf.c exitfail.c error.c quotesys.c \
	strftime.c c-stack.c basename.c exclude.c hard-locale.c \
	file-type.c posixver.c prepargs.c version-etc.c

# Important for ctype macros!
CFLAGS+=-funsigned-char

CFLAGS+=-DHAVE_CONFIG_H
CFLAGS+=-DPR_PROGRAM=\"/usr/bin/pr\"
CFLAGS+=-I${.CURDIR}/../../../tools/diff
CFLAGS+=-I${.CURDIR}/../../../tools/diff/src
CFLAGS+=-I${.CURDIR}/../../../tools/diff/lib
CFLAGS+=-I${DESTDIR}/usr/include/gnu

MAN=	diff.1 diff.7

LIBADD+=	gnuregex

.include <bsd.prog.mk>
