#	from @(#)Makefile.inc	8.1 (Berkeley) 6/4/93
# $FreeBSD: releng/11.0/lib/libc/regex/Makefile.inc 262722 2014-03-04 02:19:39Z marcel $

# regex sources
.PATH: ${LIBC_SRCTOP}/regex

CFLAGS+=-DPOSIX_MISTAKE

SRCS+=	regcomp.c regerror.c regexec.c regfree.c

SYM_MAPS+=${LIBC_SRCTOP}/regex/Symbol.map

