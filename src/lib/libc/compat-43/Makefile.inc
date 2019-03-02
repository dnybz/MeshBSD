#	@(#)Makefile.inc	8.1 (Berkeley) 6/2/93
# $FreeBSD: releng/11.0/lib/libc/compat-43/Makefile.inc 262722 2014-03-04 02:19:39Z marcel $

# compat-43 sources
.PATH: ${LIBC_SRCTOP}/${LIBC_ARCH}/compat-43 ${LIBC_SRCTOP}/compat-43

SRCS+=	creat.c gethostid.c getwd.c killpg.c sethostid.c setpgrp.c \
	setrgid.c setruid.c sigcompat.c

SYM_MAPS+=${LIBC_SRCTOP}/compat-43/Symbol.map

