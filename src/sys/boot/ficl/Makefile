# $FreeBSD: head/sys/boot/ficl/Makefile 296706 2016-03-11 23:45:51Z bdrewery $
#

FICLDIR?=	${.CURDIR}

.if ${MACHINE_CPUARCH} == "mips64" || ${TARGET_ARCH} == "mips64el"
.PATH: ${FICLDIR}/mips64
.else
.PATH: ${FICLDIR}/${MACHINE_CPUARCH}
.endif
BASE_SRCS=	dict.c ficl.c fileaccess.c float.c loader.c math64.c \
		prefix.c search.c stack.c tools.c vm.c words.c

SRCS=		${BASE_SRCS} sysdep.c softcore.c
CLEANFILES=	softcore.c testmain testmain.o
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

.if ${TARGET_ARCH} == "mips64" || ${TARGET_ARCH} == "mips64el"
FICL_ARCH=	mips64
.else
FICL_ARCH=	${TARGET_ARCH}
.endif

CFLAGS+=	-I${FICLDIR} -I${FICLDIR}/${FICL_ARCH} \
		-I${FICLDIR}/../common

softcore.c: ${SOFTWORDS} softcore.awk
	(cd ${FICLDIR}/softwords; cat ${SOFTWORDS} \
	    | awk -f softcore.awk -v datestamp="`LC_ALL=C date`") > ${.TARGET}

.if ${TARGET_ARCH} == "amd64" && defined(FICL32)
.if !exists(machine)
${SRCS:M*.c:R:S/$/.o/g}: machine

beforedepend ${OBJS}: machine
.endif

machine: .NOMETA
	ln -sf ${.CURDIR}/../../i386/include machine

CLEANFILES+=	machine
.endif
