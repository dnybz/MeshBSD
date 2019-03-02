#	From: @(#)Makefile	5.20 (Berkeley) 6/12/93
# $FreeBSD: releng/11.0/usr.sbin/Makefile 303675 2016-08-02 20:18:43Z bdrewery $

.include <src.opts.mk>

SUBDIR=	adduser \
	arp \
	chkgrp \
	chown \
	chroot \
	ckdist \
	crashinfo \
	cron \
	daemon \
	devctl \
	devinfo \
	dhcpd \
	diskinfo \
	extattr \
	extattrctl \
	fifolog \
	fstyp \
	ftp-proxy \
	getfmac \
	getpmac \
	httpd \
	i2c \
	ifmcstat \
	iostat \
	kldxref \
	makefs \
	memcontrol \
	mtest \
	newsyslog \
	nmtree \
	nologin \
	ntpd \
	pciconf \
	periodic \
	phttpget \
	procctl \
	pstat \
	pw \
	pwd_mkdb \
	rarpd \
	rpcbind \
	rtprio \
	service \
	services_mkdb \
	setfib \
	setfmac \
	setpmac \
	smbmsg \
	syslogd \
	tcpdrop \
	tcpdump \
	traceroute \
	trpt \
	tzsetup \
	ugidfw \
	vigr \
	vipw \
	wake \
	watch \
	watchdogd \
	zic

# NB: keep these sorted by MK_* knobs

SUBDIR.${MK_AUTHPF}+=	authpf
.if ${MK_BLACKLIST} != "no"
SUBDIR.${MK_BLACKLIST}+=	blacklistctl
SUBDIR.${MK_BLACKLIST}+=	blacklistd
.endif
SUBDIR.${MK_FMTREE}+=	fmtree
SUBDIR.${MK_GPIO}+=	gpioctl
SUBDIR.${MK_INET6}+=	ip6addrctl
SUBDIR.${MK_INET6}+=	mld6query
SUBDIR.${MK_INET6}+=	ndp
SUBDIR.${MK_INET6}+=	rip6query
SUBDIR.${MK_INET6}+=	route6d
SUBDIR.${MK_INET6}+=	rrenumd
SUBDIR.${MK_INET6}+=	rtadvctl
SUBDIR.${MK_INET6}+=	rtadvd
SUBDIR.${MK_INET6}+=	rtsold
SUBDIR.${MK_INET6}+=	traceroute6
SUBDIR.${MK_INETD}+=	inetd


SUBDIR.${MK_JAIL}+=	jail
SUBDIR.${MK_JAIL}+=	jexec
SUBDIR.${MK_JAIL}+=	jls

.if ${MK_LIBTHR} != "no" || ${MK_LIBPTHREAD} != "no"
SUBDIR.${MK_PPP}+=	pppctl
SUBDIR.${MK_NS_CACHING}+=	nscd
.endif

SUBDIR.${MK_NAND}+=	nandsim
SUBDIR.${MK_NAND}+=	nandtool

SUBDIR.${MK_NETGRAPH}+=	ngctl
SUBDIR.${MK_NETGRAPH}+=	nghook

SUBDIR.${MK_LIBRESSL}+=	keyserv

# SUBDIR.${MK_PF}+=	ftp-proxy
SUBDIR.${MK_PMC}+=	pmcannotate
SUBDIR.${MK_PMC}+=	pmccontrol
SUBDIR.${MK_PMC}+=	pmcstat
SUBDIR.${MK_PMC}+=	pmcstudy
SUBDIR.${MK_PPP}+=	ppp

SUBDIR.${MK_RCMDS}+=	rwhod


SUBDIR.${MK_TCP_WRAPPERS}+=	tcpdchk
SUBDIR.${MK_TCP_WRAPPERS}+=	tcpdmatch

SUBDIR.${MK_TOOLCHAIN}+=	config
SUBDIR.${MK_TOOLCHAIN}+=	crunch

SUBDIR.${MK_UNBOUND}+=	unbound

SUBDIR.${MK_USB}+=	uathload
SUBDIR.${MK_USB}+=	uhsoctl
SUBDIR.${MK_USB}+=	usbconfig
SUBDIR.${MK_USB}+=	usbdump

SUBDIR.${MK_UTMPX}+=	ac
SUBDIR.${MK_UTMPX}+=	lastlogin
SUBDIR.${MK_UTMPX}+=	utx

SUBDIR.${MK_WIRELESS}+=	wlandebug
SUBDIR.${MK_WIRELESS}+=	wpa

SUBDIR.${MK_TESTS}+=	tests

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
