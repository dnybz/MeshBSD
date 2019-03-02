#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: releng/11.0/libexec/Makefile 301553 2016-06-07 16:31:03Z lidl $

.include <src.opts.mk>

SUBDIR=	${_atf} \
	${_atrun} \
	${_blacklistd-helper} \
	getty \
	${_pppoed} \
	${_rlogind} \
	${_rshd} \
	${_rtld-elf} \
	${_tests} \
	${_tftp-proxy} \
	ulog-helper 

.if ${MK_AT} != "no"
_atrun=		atrun
.endif

.if ${MK_BLACKLIST} != "no"
_blacklistd-helper+=	blacklistd-helper
.endif

.if ${MK_FTP} != "no"
SUBDIR+=	ftpd
.endif

.if ${MK_NETGRAPH} != "no"
_pppoed=	pppoed
.endif

.if !defined(NO_PIC) && !defined(NO_RTLD)
_rtld-elf=	rtld-elf
.endif

.if ${MK_RCMDS} != "no"
_rlogind=	rlogind
_rshd=		rshd
.endif

.if ${MK_MAN_UTILS} != "no"
_makewhatis.local=	makewhatis.local
.endif

.if ${MK_TCP_WRAPPERS} != "no"
SUBDIR+=	tcpd
.endif

.if ${MK_TFTP} != "no"
SUBDIR+=	tftpd
.endif

.if ${MK_PF} != "no"
_tftp-proxy=	tftp-proxy
.endif

.if ${MK_TESTS} != "no"
_atf=		atf
_tests=		tests
.endif

.include <bsd.arch.inc.mk>

.include <bsd.subdir.mk>
