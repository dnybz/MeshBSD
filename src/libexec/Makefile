#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: head/libexec/Makefile 289087 2015-10-09 22:09:44Z bapt $

.include <src.opts.mk>

SUBDIR+= 	${_atf} 
SUBDIR+= 	atrun
SUBDIR+=	ftpd
SUBDIR+= 	getty
SUBDIR+= 	pppoed
SUBDIR+= 	${_rtld-elf}
SUBDIR+= 	${_tests}
SUBDIR+=	tftpd
SUBDIR+= 	tftp-proxy
SUBDIR+= 	ulog-helper 

.if ${MK_OPENSSH} != "no"	
SUBDIR+=sftp-server ssh-keysign ssh-pkcs11-helper
.endif

.if !defined(NO_PIC) && !defined(NO_RTLD)
_rtld-elf=	rtld-elf
.endif

.if ${MK_TESTS} != "no"
_atf=		atf
_tests=		tests
.endif

.include <bsd.arch.inc.mk>
.include <bsd.subdir.mk>
