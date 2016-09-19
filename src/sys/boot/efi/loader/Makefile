# $FreeBSD: head/sys/boot/efi/loader/Makefile 296769 2016-03-12 21:44:33Z emaste $

MAN=

.include <src.opts.mk>

MK_SSP=		no

PROG=		loader.sym
INTERNALPROG=
WARNS?=		3

# architecture-specific loader code
SRCS=	autoload.c \
	bootinfo.c \
	conf.c \
	copy.c \
	devicename.c \
	main.c \
	self_reloc.c \
	smbios.c \
	vers.c

.PATH: ${.CURDIR}/arch/${TARGET}
# For smbios.c
.include "${.CURDIR}/arch/${TARGET}/Makefile.inc"

CFLAGS+=	-I${.CURDIR}
CFLAGS+=	-I${.CURDIR}/arch/${TARGET}
CFLAGS+=	-I${.CURDIR}/../include
CFLAGS+=	-I${.CURDIR}/../include/${TARGET}
CFLAGS+=	-I${.CURDIR}/../../..

CFLAGS+=	-DNO_PCI -DEFI

# make buildenv doesn't set DESTDIR, this means LIBSTAND
# will be wrong when crossbuilding.
.if exists(${.OBJDIR}/../../../../lib/libstand/libstand.a)
LIBSTAND=	${.OBJDIR}/../../../../lib/libstand/libstand.a
.endif

.if ${MK_FORTH} != "no"
BOOT_FORTH=	yes
CFLAGS+=	-DBOOT_FORTH
CFLAGS+=	-I${.CURDIR}/../../ficl
CFLAGS+=	-I${.CURDIR}/../../ficl/${TARGET_ARCH}
LIBFICL=	${.OBJDIR}/../../ficl/libficl.a
.endif

LOADER_FDT_SUPPORT?=	no
.if ${MK_FDT} != "no" && ${LOADER_FDT_SUPPORT} != "no"
CFLAGS+=	-I${.CURDIR}/../../fdt
CFLAGS+=	-I${.OBJDIR}/../../fdt
CFLAGS+=	-DLOADER_FDT_SUPPORT
LIBEFI_FDT=	${.OBJDIR}/../../efi/fdt/libefi_fdt.a
LIBFDT=		${.OBJDIR}/../../fdt/libfdt.a
.endif

# Include bcache code.
HAVE_BCACHE=    yes

.if defined(EFI_STAGING_SIZE)
CFLAGS+=	-DEFI_STAGING_SIZE=${EFI_STAGING_SIZE}
.endif

# Always add MI sources
.PATH:		${.CURDIR}/../../common
.include	"${.CURDIR}/../../common/Makefile.inc"
CFLAGS+=	-I${.CURDIR}/../../common

FILES+=	loader.efi
FILESMODE_loader.efi=	${BINMODE}

LDSCRIPT=	${.CURDIR}/arch/${TARGET}/ldscript.${TARGET}
LDFLAGS+=	-Wl,-T${LDSCRIPT} -Wl,-Bsymbolic -shared

CLEANFILES+=	vers.c loader.efi

NEWVERSWHAT=	"EFI loader" ${TARGET}

vers.c:	${.CURDIR}/../../common/newvers.sh ${.CURDIR}/../../efi/loader/version
	sh ${.CURDIR}/../../common/newvers.sh ${.CURDIR}/version ${NEWVERSWHAT}

NM?=		nm
OBJCOPY?=	objcopy

EFI_TARGET=	binary

loader.efi: ${PROG}
	if ${NM} ${.ALLSRC} | grep ' U '; then \
		echo "Undefined symbols in ${.ALLSRC}"; \
		exit 1; \
	fi
	${OBJCOPY} -j .peheader -j .text -j .sdata -j .data \
		-j .dynamic -j .dynsym -j .rel.dyn \
		-j .rela.dyn -j .reloc -j .eh_frame -j set_Xcommand_set \
		--output-target=${EFI_TARGET} ${.ALLSRC} ${.TARGET}

LIBEFI=		${.OBJDIR}/../libefi/libefi.a

DPADD=		${LIBFICL} ${LIBEFI} ${LIBFDT} ${LIBEFI_FDT} ${LIBSTAND} \
		${LDSCRIPT}
LDADD=		${LIBFICL} ${LIBEFI} ${LIBFDT} ${LIBEFI_FDT} ${LIBSTAND}

.include <bsd.prog.mk>

beforedepend ${OBJS}: machine

CLEANFILES+=   machine

machine: .NOMETA
	ln -sf ${.CURDIR}/../../../${TARGET}/include machine

