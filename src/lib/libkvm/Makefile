#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: releng/11.0/lib/libkvm/Makefile 298840 2016-04-30 09:21:13Z ngie $

PACKAGE=lib${LIB}
LIB=	kvm

SHLIBDIR?= /lib
SHLIB_MAJOR=	7
CFLAGS+=-DLIBC_SCCS -I${.CURDIR}

WARNS?=	3

SRCS=	kvm.c kvm_cptime.c kvm_getloadavg.c \
	kvm_getswapinfo.c kvm_pcpu.c kvm_proc.c kvm_vnet.c \
	kvm_minidump_mips.c 
INCS=	kvm.h

LIBADD=	elf


.include <bsd.lib.mk>
