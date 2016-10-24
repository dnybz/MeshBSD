#	@(#)Makefile	8.5 (Berkeley) 3/31/94
# $FreeBSD: head/sbin/Makefile 291021 2015-11-18 17:52:38Z bdrewery $

.include <src.opts.mk>

SUBDIR= 	adjkerntz
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
SUBDIR+= 	newfs
SUBDIR+= 	newfs_msdos
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
SUBDIR+= 	tunefs
SUBDIR+= 	umount

.if ${MK_INET_SUPPORT} != "no"
SUBDIR+= 	nos-tun
SUBDIR+= 	ping
.endif

.if ${MK_INET6_SUPPORT} != "no"
SUBDIR+= 	ping6
.endif

.if ${MK_INET_SUPPORT} != "no" || \
    ${MK_INET6_SUPPORT} != "no"
SUBDIR+= 	nfsiod
.endif

.if ${MK_NAND} != "no"
SUBDIR+= 	nandfs
SUBDIR+= 	newfs_nandfs
.endif

.if ${MK_PF} != "no"
SUBDIR+= 	pfctl
SUBDIR+= 	pflogd
.endif

.if ${MK_RACOON2} != "no"
SUBDIR+= 	iked

.if ${MK_KERBEROS} != "no"
SUBDIR+= 	kinkd
.endif
.endif

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
