# $FreeBSD: head/sys/boot/ficl/Makefile 296706 2016-03-11 23:45:51Z bdrewery $
#

FICLDIR?=	${.CURDIR}

.if ${MACHINE_ARCH} == "mips64" || ${MACHINE_ARCH} == "mips64el"
.PATH: ${FICLDIR}/mips64
.elif ${TARGET} == "mips64" || ${TARGET} == "mips64el"
.PATH: ${FICLDIR}/mips64
.elif exists(${FICLDIR}/${TARGET_ARCH})
.PATH: ${FICLDIR}/${TARGET_ARCH}
.else
.PATH: ${FICLDIR}/${MACHINE_CPUARCH}
.endif
BASE_SRCS=	dict.c ficl.c fileaccess.c float.c loader.c math64.c \
		prefix.c search.c stack.c tools.c vm.c words.c

SRCS=		${BASE_SRCS} sysdep.c softcore.c
CLEANFILES=	softcore.c testmain testmain.o

#
# XXX ...
#
.if HAVE_PNP 
CFLAGS+=	-DHAVE_PNP
.endif
.include <bsd.stand.mk>
.ifmake testmain
CFLAGS+=	-DTESTMAIN -D_TESTMAIN
SRCS+=		testmain.c
PROG=		testmain
.include <bsd.prog.mk>
.else
LIB=		ficl
INTERNALLIB=
.include <bsd.lib.mk>
.endif

# Standard softwords
.PATH: ${FICLDIR}/softwords
SOFTWORDS=	softcore.fr jhlocal.fr marker.fr freebsd.fr ficllocal.fr \
		ifbrack.fr
# Optional OO extension softwords
#SOFTWORDS+=	oo.fr classes.fr

.if ${MACHINE_ARCH} == "mips64" || ${MACHINE_ARCH} == "mips64el"
FICL_CPUARCH=	mips64
.else
FICL_CPUARCH=	${MACHINE_CPUARCH}
.endif

CFLAGS+=	-I${FICLDIR} -I${FICLDIR}/${FICL_CPUARCH} \
		-I${FICLDIR}/../common

softcore.c: ${SOFTWORDS} softcore.awk
	(cd ${FICLDIR}/softwords; cat ${SOFTWORDS} \
	    | awk -f softcore.awk -v datestamp="`LC_ALL=C date`") > ${.TARGET}
