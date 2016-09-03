#	@(#)Makefile	8.5 (Berkeley) 3/31/94
# $FreeBSD: head/sbin/Makefile 291021 2015-11-18 17:52:38Z bdrewery $

.include <src.opts.mk>

SUBDIR+= 	adjkerntz
SUBDIR+= 	badsect
SUBDIR+= 	camcontrol
SUBDIR+= 	clri
SUBDIR+= 	comcontrol
SUBDIR+= 	conscontrol
SUBDIR+= 	ddb
SUBDIR+= 	devd
SUBDIR+= 	devfs
SUBDIR+= 	dhclient
SUBDIR+= 	dmesg
SUBDIR+= 	dump
SUBDIR+= 	dumpfs
SUBDIR+= 	dumpon
SUBDIR+= 	etherswitchcfg
SUBDIR+= 	ffsinfo
SUBDIR+= 	fsck
SUBDIR+= 	fsck_ffs
SUBDIR+= 	fsck_msdosfs
SUBDIR+= 	fsdb
SUBDIR+= 	fsirand
SUBDIR+= 	gbde
SUBDIR+= 	geom
SUBDIR+= 	ggate
SUBDIR+= 	growfs
SUBDIR+= 	gvinum
SUBDIR+= 	ifconfig
SUBDIR+= 	init
SUBDIR+= 	kldconfig
SUBDIR+= 	kldload
SUBDIR+= 	kldstat
SUBDIR+= 	kldunload
SUBDIR+= 	ldconfig
SUBDIR+= 	md5
SUBDIR+= 	mdconfig
SUBDIR+= 	mdmfs
SUBDIR+= 	mknod
SUBDIR+= 	mksnap_ffs
SUBDIR+= 	mount
SUBDIR+= 	mount_msdosfs
SUBDIR+= 	mount_nfs
SUBDIR+= 	mount_nullfs
SUBDIR+= 	nandfs
SUBDIR+= 	newfs
SUBDIR+= 	newfs_msdos
SUBDIR+= 	newfs_nandfs
SUBDIR+= 	nfsiod
SUBDIR+= 	nos-tun
SUBDIR+= 	pfctl
SUBDIR+= 	pflogd
SUBDIR+= 	ping
SUBDIR+= 	ping6
SUBDIR+= 	quotacheck
SUBDIR+= 	rcorder
SUBDIR+= 	reboot
SUBDIR+= 	recoverdisk
SUBDIR+= 	restore
SUBDIR+= 	route
SUBDIR+= 	routed
SUBDIR+= 	rtsol
SUBDIR+= 	savecore
SUBDIR+= 	setkey
SUBDIR+= 	shutdown
SUBDIR+= 	spppcontrol
SUBDIR+= 	swapon
SUBDIR+= 	sysctl
SUBDIR+= 	tests
SUBDIR+= 	tunefs
SUBDIR+= 	umount

SUBDIR.${MK_TESTS}+=	tests

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
