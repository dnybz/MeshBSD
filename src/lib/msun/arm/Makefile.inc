# $FreeBSD: head/lib/msun/arm/Makefile.inc 297283 2016-03-26 03:46:12Z bdrewery $

LDBL_PREC = 53
SYM_MAPS += ${.CURDIR}/arm/Symbol.map

.if ${TARGET_ARCH} == "armv6"
ARCH_SRCS = fenv-softfp.c fenv-vfp.c
.endif

CFLAGS.fenv-vfp.c=	-mfpu=vfp -mfloat-abi=softfp
