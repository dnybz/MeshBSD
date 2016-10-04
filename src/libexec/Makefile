#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: head/libexec/Makefile 289087 2015-10-09 22:09:44Z bapt $

.include <src.opts.mk>

SUBDIR+= 	${_atf} 
SUBDIR+= 	${_atrun}
SUBDIR+=	${_bootpd}
SUBDIR+=	${_ftpd}
SUBDIR+= 	getty
SUBDIR+= 	${_open_ssh}
SUBDIR+= 	${_pppoed}
SUBDIR+= 	$(_rlogind}
SUBDIR+= 	$(_rshd}
SUBDIR+= 	${_rtld_elf}
SUBDIR+=	${_tftpd}
SUBDIR+= 	${_tftp_proxy}
SUBDIR+= 	ulog-helper 

.if ${MK_AT} != "no"
_atrun= 	atrun
.endif 

.if ${MK_ATF} != "no"
_atf=		atf
.endif

.if ${MK_BOOTPD} != "no"
_bootpd= 	bootpd
.endif

.if ${MK_FTP} != "no"
_ftpd= 	ftpd
.endif

.if ${MK_NETGRAPH} != "no"
_pppoed= 	pppoed
.endif

.if ${MK_OPENSSH} != "no"	
_open_ssh= 	sftp-server ssh-keysign ssh-pkcs11-helper
.endif

.if ${MK_RCMDS} != "no"
_rlogind= 	rlogind
_rshd= 	rshd
.endif

.if ${MK_TFTP} != "no"
_tftpd= 	tftpd
_tftp_proxy= 	tftp-proxy
.endif

.if !defined(NO_PIC) && !defined(NO_RTLD)
_rtld_elf=	rtld-elf
.endif

.include <bsd.arch.inc.mk>
.include <bsd.subdir.mk>
