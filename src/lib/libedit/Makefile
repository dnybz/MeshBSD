#	$NetBSD: Makefile,v 1.37 2009/01/18 12:17:49 lukem Exp $
#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: releng/11.0/lib/libedit/Makefile 298107 2016-04-16 07:45:30Z gjb $

PACKAGE=clibs
LIB=	edit
SHLIB_MAJOR=	7
SHLIBDIR?= /lib

OSRCS=	chared.c common.c el.c emacs.c fcns.c filecomplete.c help.c \
	hist.c keymacro.c map.c chartype.c \
	parse.c prompt.c read.c refresh.c search.c sig.c terminal.c tty.c vi.c

LIBADD=	ncursesw

# For speed and debugging
#SRCS=   ${OSRCS}
# For protection
SRCS=	editline.c
SRCS+=	tokenizer.c history.c readline.c
SRCS+=	common.h emacs.h fcns.h help.h vi.h
CLEANFILES+= common.h editline.c emacs.h fcns.c fcns.h help.c help.h vi.h

INCS=	histedit.h

OSRCS+=	eln.c
SRCS+=	tokenizern.c historyn.c
CLEANFILES+=	tokenizern.c historyn.c
CFLAGS+= -I. -I${.CURDIR} -I${.CURDIR}/edit -DWIDECHAR
CFLAGS+= #-DDEBUG_TTY -DDEBUG_KEY -DDEBUG_READ -DDEBUG -DDEBUG_REFRESH
CFLAGS+= #-DDEBUG_PASTE -DDEBUG_EDIT

WARNS?=	1

SUBDIR= edit/readline

AHDR=	vi.h emacs.h common.h
ASRC=	${.CURDIR}/vi.c ${.CURDIR}/emacs.c ${.CURDIR}/common.c

.for hdr in vi emacs common
${hdr}.h: ${hdr}.c makelist
	sh ${.CURDIR}/makelist -h ${.CURDIR}/${hdr}.c > ${.TARGET}
.endfor

fcns.h: ${AHDR} makelist
	sh ${.CURDIR}/makelist -fh ${AHDR} > ${.TARGET}

fcns.c: ${AHDR} fcns.h makelist
	sh ${.CURDIR}/makelist -fc ${AHDR} > ${.TARGET}

help.c: ${ASRC} makelist
	sh ${.CURDIR}/makelist -bc ${ASRC} > ${.TARGET}

help.h: ${ASRC} makelist
	sh ${.CURDIR}/makelist -bh ${ASRC} > ${.TARGET}

editline.c: ${OSRCS}
	sh ${.CURDIR}/makelist -e ${.ALLSRC:T} > ${.TARGET}

tokenizern.c: makelist Makefile
	sh ${.CURDIR}/makelist -n tokenizer.c > ${.TARGET}

historyn.c: makelist Makefile
	sh ${.CURDIR}/makelist -n history.c > ${.TARGET}

# minimal dependency to make "make depend" optional
editline.o editline.po editline.So editline.ln:	\
	common.h emacs.h fcns.c fcns.h help.c help.h vi.h

tc1.o: ${.CURDIR}/TEST/tc1.c

test: tc1.o libedit.a ${DPADD} ${LIBTERMCAP}
	${CC} ${CFLAGS} ${.ALLSRC} -o ${.TARGET} libedit.a ${LDADD}

.include <bsd.lib.mk>
