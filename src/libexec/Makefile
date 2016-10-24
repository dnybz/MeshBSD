#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: head/libexec/Makefile 289087 2015-10-09 22:09:44Z bapt $

.include <src.opts.mk>


SUBDIR+= 	getty
SUBDIR+= 	ulog-helper 

.if ${MK_AT} != "no"
SUBDIR+= 	atrun
.endif 

.if ${MK_ATF} != "no"
SUBDIR+= 	atf
.endif

.if ${MK_BOOTPD} != "no"
SUBDIR+= 	bootpd
.endif

.if ${MK_FTP} != "no"
SUBDIR+= 	ftpd
.endif

.if ${MK_NETGRAPH} != "no"
SUBDIR+= 	pppoed
.endif

.if ${MK_NIS} != "no"
SUBDIR+= 	mknetid
SUBDIR+= 	ypxfr
.endif

.if ${MK_OPENSSH} != "no"	
SUBDIR+= 	sftp-server
SUBDIR+= 	ssh-keysign
SUBDIR+= 	ssh-pkcs11-helper
.endif

.if ${MK_RCMDS} != "no"
SUBDIR+= 	rlogind
SUBDIR+= 	rshd
.endif

.if ${MK_TFTP} != "no"
SUBDIR+= 	tftpd
SUBDIR+= 	tftp-proxy
.endif

.if !defined(NO_PIC) && !defined(NO_RTLD)
SUBDIR+= rtld-elf
.endif

.include <bsd.arch.inc.mk>
.include <bsd.subdir.mk>
