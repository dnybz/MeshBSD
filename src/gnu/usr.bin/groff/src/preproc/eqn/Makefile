# $FreeBSD: releng/11.0/gnu/usr.bin/groff/src/preproc/eqn/Makefile 270168 2014-08-19 15:04:32Z bdrewery $

PROG_CXX=	eqn
SRCS=		eqn.cpp eqn_tab.h main.cpp lex.cpp \
		box.cpp limit.cpp list.cpp over.cpp text.cpp \
		script.cpp mark.cpp other.cpp delim.cpp sqrt.cpp pile.cpp special.cpp
CFLAGS+=	-I${DIST_DIR} -I.
DPADD=		${LIBGROFF}
LDADD=		${LIBGROFF}
SCRIPTS=	neqn
MAN=		eqn.1 neqn.1
CLEANFILES=	eqn.cpp eqn_tab.h ${SCRIPTS} ${MAN} y.tab.c y.tab.h

eqn_tab.h: eqn.cpp

.include <bsd.prog.mk>
