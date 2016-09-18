# $FreeBSD: head/sys/boot/efi/Makefile.inc 287227 2015-08-27 23:46:42Z imp $

BINDIR?=	/boot

# Options used when building app-specific efi components
# See conf/kern.mk for the correct set of these
CFLAGS+=	-ffreestanding -Wformat -msoft-float ${CFLAGS_NO_SIMD}
LDFLAGS+=	-nostdlib

.include "../Makefile.inc"
