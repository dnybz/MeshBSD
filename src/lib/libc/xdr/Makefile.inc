#	@(#)Makefile	5.11 (Berkeley) 9/6/90
# $FreeBSD: releng/11.0/lib/libc/xdr/Makefile.inc 262722 2014-03-04 02:19:39Z marcel $

.PATH: ${LIBC_SRCTOP}/xdr ${LIBC_SRCTOP}/.
SRCS+=	xdr.c xdr_array.c xdr_float.c xdr_mem.c \
	xdr_rec.c xdr_reference.c xdr_sizeof.c \
	xdr_stdio.c

SYM_MAPS+=	${LIBC_SRCTOP}/xdr/Symbol.map

