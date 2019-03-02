#	@(#)Makefile.inc	8.3 (Berkeley) 10/24/94
# $FreeBSD: releng/11.0/lib/libc/sys/Makefile.inc 302092 2016-06-22 21:11:27Z brooks $

# sys sources
.PATH: ${LIBC_SRCTOP}/${LIBC_ARCH}/sys ${LIBC_SRCTOP}/sys

# Include the generated makefile containing the *complete* list
# of syscall names in MIASM.
.include "${LIBC_SRCTOP}/../../sys/sys/syscall.mk"

# Include machine dependent definitions.
#
# MDASM names override the default syscall names in MIASM.
# NOASM will prevent the default syscall code from being generated.
#
.sinclude "${LIBC_SRCTOP}/${LIBC_ARCH}/sys/Makefile.inc"

SRCS+= clock_gettime.c gettimeofday.c __vdso_gettimeofday.c
NOASM+=  clock_gettime.o gettimeofday.o
PSEUDO+= _clock_gettime.o _gettimeofday.o

# Sources common to both syscall interfaces:
SRCS+=	\
	__error.c \
	interposing_table.c

SRCS+= futimens.c utimensat.c
NOASM+= futimens.o utimensat.o
PSEUDO+= _futimens.o _utimensat.o

SRCS+= pipe.c

INTERPOSED = \
	accept \
	accept4 \
	aio_suspend \
	close \
	connect \
	fcntl \
	fsync \
	fork \
	kevent \
	msync \
	nanosleep \
	open \
	openat \
	poll \
	ppoll \
	pselect \
	read \
	readv \
	recvfrom \
	recvmsg \
	select \
	sendmsg \
	sendto \
	setcontext \
	sigprocmask \
	sigsuspend \
	sigtimedwait \
	sigwait \
	sigwaitinfo \
	swapcontext \
	wait4 \
	wait6 \
	write \
	writev

.if ${MACHINE_CPUARCH} == "sparc64"
SRCS+=	sigaction.c
NOASM+=	sigaction.o
.else
INTERPOSED+= sigaction
.endif

SRCS+=	${INTERPOSED:S/$/.c/}
NOASM+=	${INTERPOSED:S/$/.o/}
PSEUDO+=	${INTERPOSED:C/^.*$/_&.o/}

# Add machine dependent asm sources:
SRCS+=${MDASM}

# Look though the complete list of syscalls (MIASM) for names that are
# not defined with machine dependent implementations (MDASM) and are
# not declared for no generation of default code (NOASM).  Add each
# syscall that satisfies these conditions to the ASM list.
.for _asm in ${MIASM}
.if (${MDASM:R:M${_asm:R}} == "")
.if (${NOASM:R:M${_asm:R}} == "")
ASM+=$(_asm)
.endif
.endif
.endfor

SASM=	${ASM:S/.o/.S/}

SPSEUDO= ${PSEUDO:S/.o/.S/}

SRCS+=	${SASM} ${SPSEUDO}

SYM_MAPS+=	${LIBC_SRCTOP}/sys/Symbol.map

# Generated files
CLEANFILES+=	${SASM} ${SPSEUDO}

.if ${MACHINE_CPUARCH} == "amd64" || ${MACHINE_CPUARCH} == "i386" || \
    ${MACHINE_CPUARCH} == "powerpc" || ${MACHINE_ARCH:Marmv6*}
NOTE_GNU_STACK='\t.section .note.GNU-stack,"",%%progbits\n'
.else
NOTE_GNU_STACK=''
.endif

${SASM}:
	printf '#include "compat.h"\n' > ${.TARGET}
	printf '#include "SYS.h"\nRSYSCALL(${.PREFIX})\n' >> ${.TARGET}
	printf  ${NOTE_GNU_STACK} >>${.TARGET}

${SPSEUDO}:
	printf '#include "compat.h"\n' > ${.TARGET}
	printf '#include "SYS.h"\nPSEUDO(${.PREFIX:S/_//})\n' \
	    >> ${.TARGET}
	printf ${NOTE_GNU_STACK} >>${.TARGET}

