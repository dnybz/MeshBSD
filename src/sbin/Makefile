#	@(#)Makefile	8.5 (Berkeley) 3/31/94
# $FreeBSD: head/sbin/Makefile 291021 2015-11-18 17:52:38Z bdrewery $

.include <src.opts.mk>

SUBDIR= adjkerntz \
	camcontrol \
	comcontrol \
	conscontrol \
	devd \
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
	ipfw \
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
	natd \
	nandfs \
	newfs \
	newfs_msdos \
	newfs_nandfs \
	pfctl \
	ping \
	ping6 \
	pflogd \
	quotacheck \
	reboot \
	restore \
	route \
	routed \
	rtsol \
	setkey \
	shutdown \
	swapon \
	sysctl \
	tests \
	umount

SUBDIR.${MK_TESTS}+=	tests

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
