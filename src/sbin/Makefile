#	@(#)Makefile	8.5 (Berkeley) 3/31/94
# $FreeBSD: head/sbin/Makefile 291021 2015-11-18 17:52:38Z bdrewery $

.include <src.opts.mk>

# XXX MISSING:		icheck ncheck

SUBDIR=adjkerntz \
	camcontrol \
	comcontrol \
	conscontrol \
	devfs \
	dhclient \
	dmesg \
	dump \
	etherswitchcfg \
	fsck \
	fsck_ffs \
	fsck_msdosfs \
	gbde \
	geom \
	ggate \
	ifconfig \
	init \
	kldconfig \
	kldload \
	kldstat \
	kldunload \
	ldconfig \
	md5 \
	mdconfig \
	mdmfs \
	mknod \
	mksnap_ffs \
	mount \
	mount_msdosfs \
	mount_nfs \
	mount_nullfs \
	newfs \
	newfs_msdos \
	ping \
	reboot \
	restore \
	route \
	setkey \
	shutdown \
	swapon \
	sysctl \
	umount

SUBDIR.${MK_CASPER}+=	casperd
SUBDIR.${MK_CXX}+=	devd
SUBDIR.${MK_INET6}+=	ping6
SUBDIR.${MK_INET6}+=	rtsol
SUBDIR.${MK_IPFW}+=	ipfw
SUBDIR.${MK_IPFW}+=	natd
SUBDIR.${MK_ISCSI}+=	iscontrol
SUBDIR.${MK_NAND}+=	nandfs
SUBDIR.${MK_NAND}+=	newfs_nandfs
SUBDIR.${MK_PF}+=	pfctl
SUBDIR.${MK_PF}+=	pflogd
SUBDIR.${MK_QUOTAS}+=	quotacheck
SUBDIR.${MK_ROUTED}+=	routed

SUBDIR.${MK_TESTS}+=	tests

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
