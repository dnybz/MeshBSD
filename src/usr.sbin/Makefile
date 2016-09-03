#	From: @(#)Makefile	5.20 (Berkeley) 6/12/93
# $FreeBSD: head/usr.sbin/Makefile 292553 2015-12-21 17:41:08Z ian $

.include <src.opts.mk>

#
# XXX: powerd(8) will be replaced by powerd++(8)
#

SUBDIR+= 	ac
SUBDIR+= 	adduser
SUBDIR+= 	amd
SUBDIR+= 	ancontrol
SUBDIR+= 	arp
SUBDIR+= 	authpf
SUBDIR+= 	autofs
SUBDIR+= 	bluetooth
SUBDIR+= 	chown
SUBDIR+= 	chroot
SUBDIR+= 	config
SUBDIR+= 	cron
SUBDIR+= 	crunch
SUBDIR+= 	daemon
SUBDIR+= 	devctl
SUBDIR+= 	devinfo
SUBDIR+= 	dhcpd
SUBDIR+= 	eeprom
SUBDIR+= 	flowctl
SUBDIR+= 	ftp-proxy
SUBDIR+= 	fwcontrol
SUBDIR+= 	gpioctl
SUBDIR+= 	httpd
SUBDIR+= 	i2c
SUBDIR+= 	inetd
SUBDIR+= 	ip6addrctl
SUBDIR+= 	kbdcontrol
SUBDIR+= 	kldxref
SUBDIR+= 	lastlogin
SUBDIR+= 	lpr
SUBDIR+= 	lptcontrol
SUBDIR+= 	mailstats
SUBDIR+= 	mailwrapper
SUBDIR+= 	makefs
SUBDIR+= 	mixer
SUBDIR+= 	mld6query
SUBDIR+= 	mtest
SUBDIR+= 	ndp
SUBDIR+= 	newsyslog
SUBDIR+= 	ngctl
SUBDIR+= 	nghook
SUBDIR+= 	nmtree
SUBDIR+= 	nologin
SUBDIR+= 	nscd
SUBDIR+= 	ntp
SUBDIR+= 	pciconf
SUBDIR+= 	periodic
SUBDIR+= 	phttpget
SUBDIR+= 	powerd
SUBDIR+= 	ppp
SUBDIR+= 	pppctl
SUBDIR+= 	pstat
SUBDIR+= 	pw
SUBDIR+= 	pwd_mkdb
SUBDIR+= 	rarpd
SUBDIR+= 	rip6query
SUBDIR+= 	route6d
SUBDIR+= 	rpc.umntall
SUBDIR+= 	rrenumd
SUBDIR+= 	rtadvctl
SUBDIR+= 	rtadvd
SUBDIR+= 	rtprio
SUBDIR+= 	rtsold
SUBDIR+= 	sendmail
SUBDIR+= 	services_mkdb
SUBDIR+= 	setfib
SUBDIR+= 	setfmac
SUBDIR+= 	setpmac
SUBDIR+= 	snapinfo
SUBDIR+= 	syslogd
SUBDIR+= 	tcpdrop
SUBDIR+= 	tests
SUBDIR+= 	timed
SUBDIR+= 	traceroute
SUBDIR+= 	traceroute6
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

SUBDIR.${MK_TESTS}+=	tests

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
