# $FreeBSD: releng/11.0/lib/libpe/Makefile 295581 2016-02-12 21:12:47Z emaste $
.include <bsd.own.mk>

INTERNALLIB=

ELFTCDIR=	${.CURDIR}/../../contrib/elftoolchain

.PATH:	${ELFTCDIR}/libpe

LIB=	pe

SRCS=	libpe_buffer.c \
	libpe_coff.c \
	libpe_dos.c \
	libpe_init.c \
	libpe_rich.c \
	libpe_section.c \
	libpe_utils.c \
	pe_buffer.c \
	pe_cntl.c \
	pe_coff.c \
	pe_dos.c \
	pe_flag.c \
	pe_init.c \
	pe_rich.c \
	pe_section.c \
	pe_symtab.c \
	pe_update.c

CFLAGS+=-I${ELFTCDIR}/libpe -I${ELFTCDIR}/common

MAN=

.include <bsd.lib.mk>
