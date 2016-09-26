# $FreeBSD: head/usr.bin/bmake/Makefile.inc 288154 2015-09-23 21:35:58Z bdrewery $ 

.sinclude <src.opts.mk>

.if defined(.PARSEDIR)
# make sure this is available to unit-tests/Makefile
.export SRCTOP
.endif

.if exists(${.CURDIR}/tests)
PROG= make
.endif

.if !defined(MK_SHARED_TOOLCHAIN) || ${MK_SHARED_TOOLCHAIN} == "no"
NO_SHARED?=     YES
.endif

WARNS=3
CFLAGS+= -DNO_PWD_OVERRIDE
