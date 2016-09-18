#	From: @(#)Makefile	5.20 (Berkeley) 6/12/93
# $FreeBSD: head/usr.sbin/Makefile 292553 2015-12-21 17:41:08Z ian $

.include <src.opts.mk>

#
# XXX: powerd(8) will be replaced by powerd++(8)
#

SUBDIR= 	ac
SUBDIR+= 	adduser
SUBDIR+= 	ancontrol
SUBDIR+= 	btxld
SUBDIR+= 	chkgrp
SUBDIR+= 	chown
SUBDIR+= 	chroot
SUBDIR+= 	config
SUBDIR+= 	cron
SUBDIR+= 	crunch
SUBDIR+= 	daemon
SUBDIR+= 	devctl
SUBDIR+= 	devinfo
SUBDIR+= 	eeprom
SUBDIR+= 	gpioctl
#
# XXX: LibreSSL and tls(3) are missing dependencies, but
# XXX: integration of those components are present task.
#
#SUBDIR+= 	httpd
#
SUBDIR+= 	i2c
SUBDIR+= 	kbdcontrol
SUBDIR+= 	kldxref
SUBDIR+= 	lastlogin
SUBDIR+= 	lpr
SUBDIR+= 	makefs
SUBDIR+= 	mixer
SUBDIR+= 	mtest
SUBDIR+= 	newsyslog
SUBDIR+= 	nmtree
SUBDIR+= 	nologin
SUBDIR+= 	pciconf
SUBDIR+= 	periodic
SUBDIR+= 	powerd
SUBDIR+= 	pstat
SUBDIR+= 	pw
SUBDIR+= 	pwd_mkdb
SUBDIR+= 	setfmac
SUBDIR+= 	setpmac
SUBDIR+= 	snapinfo
SUBDIR+= 	syslogd
SUBDIR+= 	timed
SUBDIR+= 	tzsetup
SUBDIR+= 	uathload
SUBDIR+= 	ugidfw
SUBDIR+= 	uhsoctl
SUBDIR+= 	usbconfig
SUBDIR+= 	usbdump
SUBDIR+= 	utx
SUBDIR+= 	vidcontrol
SUBDIR+= 	watch
SUBDIR+= 	watchdogd
SUBDIR+= 	wlandebug
SUBDIR+= 	wpa
SUBDIR+= 	zic
SUBDIR+= 	zzz

.if ${MK_AMD} != "no"
SUBDIR+= 	amd
SUBDIR+= 	autofs
.endif

.if ${MK_AUDIT} != "no" 
SUBDIR+= 	audit
SUBDIR+= 	auditd
SUBDIR+= 	auditdistd
SUBDIR+= 	auditreduce
.endif

.if ${MK_BLUETOOTH} != "no"
SUBDIR+= 	bluetooth
.endif

.if ${MK_BSNMP} != "no"
SUBDIR+= 	bsnmpd
.endif

.if ${MK_INET_SUPPORT} != "no"
SUBDIR+= 	arp
SUBDIR+= 	rarpd
SUBDIR+= 	traceroute
.endif

.if ${MK_INET6_SUPPORT} != "no"
SUBDIR+= 	ip6addrctl
SUBDIR+= 	mld6query
SUBDIR+= 	ndp
SUBDIR+= 	rip6query
SUBDIR+= 	route6d
SUBDIR+= 	traceroute6
.endif

.if ${MK_INET_SUPPORT} != "no" || \
    ${MK_INET6_SUPPORT} != "no"
SUBDIR+= 	dhcpd
SUBDIR+= 	flowctl
SUBDIR+= 	ftp-proxy
SUBDIR+= 	fwcontrol
SUBDIR+= 	inetd
SUBDIR+= 	mailstats
SUBDIR+= 	nscd
SUBDIR+= 	ntp
SUBDIR+= 	phttpget
SUBDIR+= 	ppp
SUBDIR+= 	pppctl
SUBDIR+= 	rpc.lockd
SUBDIR+= 	rpc.statd
SUBDIR+= 	rpc.umntall
SUBDIR+= 	rpc.umntall
SUBDIR+= 	rpcbind
SUBDIR+= 	rrenumd
SUBDIR+= 	rtadvctl
SUBDIR+= 	rtadvd
SUBDIR+= 	rtprio
SUBDIR+= 	rtsold
SUBDIR+= 	setfib
SUBDIR+= 	services_mkdb
SUBDIR+= 	tcpdrop
.if ${MK_NIS} != "no"
SUBDIR+= 	rpc.yppasswdd
SUBDIR+= 	rpc.ypupdated
SUBDIR+= 	rpc.ypxfrd
SUBDIR+= 	yp_mkdb
SUBDIR+= 	ypbind
SUBDIR+= 	ypldap
SUBDIR+= 	yppoll
SUBDIR+= 	yppush
SUBDIR+= 	ypserv
SUBDIR+= 	ypset
.endif
.if ${MK_OPENSSH} != "no"
SUBDIR+=sshd
.endif
.endif

.if ${MK_NAND} != "no"
SUBDIR+= 	nandsim
SUBDIR+= 	nandtool
.endif

.if ${MK_NETGRAPH} != "no"
SUBDIR+= 	ngctl
SUBDIR+= 	nghook
.endif

.if ${MK_PF} != "no"
SUBDIR+= 	authpf
.endif

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
