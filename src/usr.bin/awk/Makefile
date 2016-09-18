# $FreeBSD: head/usr.bin/awk/Makefile 299289 2016-05-09 22:21:09Z bdrewery $

AWKSRC=	${.CURDIR}/../../tools/one-true-awk
.PATH: ${AWKSRC}

PROG=	awk
SRCS=	awkgram.y b.c lex.c lib.c main.c parse.c proctab.c run.c tran.c ytab.h

CFLAGS+= -DHAS_ISBLANK -I. -I${AWKSRC} -DFOPEN_MAX=64

WARNS?=	1

LIBADD=	m

LINKS=	${BINDIR}/awk ${BINDIR}/nawk
MLINKS=	awk.1 nawk.1

CLEANFILES= maketab proctab.c ytab.h

ytab.h: awkgram.h .NOMETA
	ln -sf ${.ALLSRC} ${.TARGET}

proctab.c: maketab
	${BTOOLSPATH:U.}/maketab > proctab.c

build-tools: maketab
maketab: ytab.h ${AWKSRC}/maketab.c ${BUILD_TOOLS_META}

.include <bsd.prog.mk>
