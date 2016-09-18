# $FreeBSD: head/lib/libthr/support/Makefile.inc 212516 2010-09-13 01:43:10Z imp $

.PATH: ${.CURDIR}/support ${.CURDIR}/../libc/gen ${.CURDIR}/../libc/string

# libc must search machine_arch, then machine_cpuarch, but libthr has all its
# code implemented in machine_cpuarch.  Cope.
.if exists(${.CURDIR}/../libc/${TARGET}/sys)
.PATH: ${.CURDIR}/../libc/${TARGET}/sys
CFLAGS+= -I${.CURDIR}/../libc/${TARGET}
.else
.PATH: ${.CURDIR}/../libc/${TARGET_ARCH}/sys
CFLAGS+= -I${.CURDIR}/../libc/${TARGET_ARCH}
.endif

SYSCALLS= thr_new

SYSCALL_SRC=   ${SYSCALLS:S/$/.S/}
SYSCALL_OBJ=   ${SYSCALLS:S/$/.So/}

${SYSCALL_SRC}:
	printf '#include "SYS.h"\nRSYSCALL(${.PREFIX})\n' > ${.TARGET}

LIBC_OBJS=

SOBJS+=	thr_libc.So
CLEANFILES+= ${SYSCALL_SRC} ${SYSCALL_OBJ} ${LIBC_OBJS}

thr_libc.So: ${SYSCALL_OBJ} ${LIBC_OBJS}
	${CC} -fPIC -nostdlib -o ${.TARGET} -r ${.ALLSRC}
