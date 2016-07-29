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
	${_tests} \
	${_tftp-proxy} \
	ulog-helper 


_atrun=		atrun
_casper=	

SUBDIR+=	ftpd

_dma=		dma
_pppoed=	pppoed
_tftp-proxy=	tftp-proxy

.if !defined(NO_PIC) && !defined(NO_RTLD)
_rtld-elf=	rtld-elf
.endif


SUBDIR+=	tftpd

.if ${MK_TESTS} != "no"
_atf=		atf
_tests=		tests
.endif

.include <bsd.arch.inc.mk>

.include <bsd.subdir.mk>
