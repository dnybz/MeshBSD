#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: head/libexec/Makefile 289087 2015-10-09 22:09:44Z bapt $

.include <src.opts.mk>

SUBDIR+= 	${_atf} 
SUBDIR+= 	${_atrun}
SUBDIR+=	${_ftpd}
SUBDIR+= 	getty
SUBDIR+= 	${_open_ssh}
SUBDIR+= 	${_pppoed}
SUBDIR+= 	$(_rlogind}
SUBDIR+= 	$(_rshd}
SUBDIR+= 	${_rtld-elf}
SUBDIR+=	tftpd
SUBDIR+= 	tftp-proxy
SUBDIR+= 	ulog-helper 

.if ${MK_AT} != "no"
_atrun= 	atrun
.endif 

.if ${MK_ATF} != "no"
_atf=		atf
.endif

.if ${MK_INET_SUPPORT} != "no" || \
    ${MK_INET6_SUPPORT} != "no"
.if ${MK_FTP} != "no"
_ftpd= 	ftpd
.endif
.if ${MK_OPENSSH} != "no"	
_open_ssh= 	sftp-server ssh-keysign ssh-pkcs11-helper
.endif
.if ${MK_RCMDS} != "no"
_rlogind= 	rlogind
_rshd= 	rshd
.endif
.endif

.if ${MK_NETGRAPH} != "no"
_pppoed= 	pppoed
.endif


.if !defined(NO_PIC) && !defined(NO_RTLD)
_rtld-elf=	rtld-elf
.endif



.include <bsd.arch.inc.mk>
.include <bsd.subdir.mk>
