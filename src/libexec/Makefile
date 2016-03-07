#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: head/libexec/Makefile 289087 2015-10-09 22:09:44Z bapt $

.include <src.opts.mk>

SUBDIR=	${_atf} \
	${_atrun} \
	${_casper} \
	${_dma} \
	getty \
	${_mail.local} \
	${_pppoed} \
	${_rtld-elf} \
	${_telnetd} \
	${_tests} \
	${_tftp-proxy} \
	ulog-helper 

.if ${MK_AT} != "no"
_atrun=		atrun
.endif

.if ${MK_CASPER} != "no"
_casper=	casper
.endif

.if ${MK_FTP} != "no"
SUBDIR+=	ftpd
.endif

.if ${MK_DMAGENT} != "no"
_dma=		dma
.endif

.if ${MK_NETGRAPH} != "no"
_pppoed=	pppoed
.endif

.if ${MK_PF} != "no"
_tftp-proxy=	tftp-proxy
.endif

.if !defined(NO_PIC) && !defined(NO_RTLD)
_rtld-elf=	rtld-elf
.endif

.if ${MK_TELNET} != "no"
_telnetd=	telnetd
.endif

.if ${MK_TFTP} != "no"
SUBDIR+=	tftpd
.endif

.if ${MK_TESTS} != "no"
_atf=		atf
_tests=		tests
.endif

.include <bsd.arch.inc.mk>

.include <bsd.subdir.mk>
