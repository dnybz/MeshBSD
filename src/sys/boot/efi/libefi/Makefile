# $FreeBSD: head/sys/boot/efi/libefi/Makefile 300081 2016-05-17 21:25:20Z imp $

LIB=	efi
INTERNALLIB=
WARNS?=	2

SRCS=	delay.c efi_console.c efinet.c efipart.c env.c errno.c handles.c \
	libefi.c time.c

.if ${MACHINE_CPUARCH} == "aarch64"
CFLAGS+=	-msoft-float -mgeneral-regs-only
.endif

CFLAGS+= -I${.CURDIR}/../include
CFLAGS+= -I${.CURDIR}/../include/${TARGET}
CFLAGS+= -I${.CURDIR}/../../../../lib/libstand

# Pick up the bootstrap header for some interface items
CFLAGS+= -I${.CURDIR}/../../common

# Handle FreeBSD specific %b and %D printf format specifiers
CFLAGS+= ${FORMAT_EXTENSIONS}
CFLAGS+= -DTERM_EMU

.include <bsd.lib.mk>
