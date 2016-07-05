#	From: @(#)Makefile	5.20 (Berkeley) 6/12/93
# $FreeBSD: head/usr.sbin/Makefile 292553 2015-12-21 17:41:08Z ian $

.include <src.opts.mk>

#
# XXX: powerd(8) will be replaced by powerd++(8)
#

SUBDIR=	ac \
	adduser \
	ancontrol \
	arp \
	authpf \
	autofs \
	bluetooth \
	chown \
	chroot \
	config \
	cron \
	crunch \
	daemon \
	devctl \
	devinfo \
	dhcpd \
	eeprom \
	flowctl \
	ftp-proxy \
	fwcontrol \
	gpioctl \
	i2c \
	inetd \
	ip6addrctl \
	ipfwpcap \
	kldxref \
	lastlogin \
	lpr \
	lptcontrol \
	mailstats \
	mailwrapper \
	makefs \
	mixer \
	mld6query \
	mtest \
	ndp \
	newsyslog \
	ngctl \
	nghook \
	nmtree \
	nologin \
	nscd \
	ntp \
	pciconf \
	periodic \
	phttpget \
	powerd \
	ppp \
	pppctl \
	pstat \
	pw \
	pwd_mkdb \
	rarpd \
	rip6query \
	route6d \
	rpc.umntall \
	rrenumd \
	rtadvctl \
	rtadvd \
	rtprio \
	rtsold \
	sendmail \
	services_mkdb \
	setfib \
	setfmac \
	setpmac \
	snapinfo \
	syslogd \
	tcpdrop \
	tcpdump \
	timed \
	traceroute \
	traceroute6 \
	tzsetup \
	uathload \
	ugidfw \
	uhsoctl \
	usbconfig \
	usbdump \
	utx \
	vidcontrol \
	watch \
	watchdogd \
	wlandebug \
	wlconfig \
	wpa \
	zic \
	zzz

SUBDIR.${MK_TESTS}+=	tests

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
