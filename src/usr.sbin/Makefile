#	From: @(#)Makefile	5.20 (Berkeley) 6/12/93
# $FreeBSD: head/usr.sbin/Makefile 292553 2015-12-21 17:41:08Z ian $

.include <src.opts.mk>

#
# XXX: httpd(8) commented out, but after substtution  
# XXX: of OpenSSL with LibreSSL, I'll re-integrate
# XXX: this deamon in conjuction with libtls(3) 
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
	nandsim \
	nandtool \
	ndp \
	newsyslog \
	ngctl \
	nghook \
	nmtree \
	nologin \
	nscd \
	ntp \
	nvram \
	pciconf \
	periodic \
	phttpget \
	pmcannotate \
	pmccontrol \
	pmcstat \
	pmcstudy \
	powerd \
	ppp \
	pppctl \
	pstat \
	pw \
	pwd_mkdb \
	rarpd \
	rip6query \
	route6d \
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
