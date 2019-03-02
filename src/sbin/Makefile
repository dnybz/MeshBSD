#	@(#)Makefile	8.5 (Berkeley) 3/31/94
# $FreeBSD: releng/11.0/sbin/Makefile 296047 2016-02-25 18:23:40Z oshogbo $

.include <src.opts.mk>

# XXX MISSING:		icheck ncheck

SUBDIR= adjkerntz \
	comcontrol \
	conscontrol \
	ddb \
	devfs \
	dhclient \
	dmesg \
	etherswitchcfg \
	fsck \
	fsck_ffs \
	fsck_msdosfs \
	gbde \
	geom \
	ggate \
	ifconfig \
	iked \
	init \
	kinkd \
	kldconfig \
	kldload \
	kldstat \
	kldunload \
	ldconfig \
	md5 \
	mdconfig \
	mdmfs \
	mknod \
	mount \
	mount_msdosfs \
	mount_nullfs \
	newfs \
	newfs_msdos \
	nos-tun \
	ping \
	reboot \
	resolvconf \
	route \
	savecore \
	setkey \
	shutdown \
	sysctl \
	umount

SUBDIR.${MK_CXX}+=	devd
SUBDIR.${MK_INET6}+=	ping6
SUBDIR.${MK_INET6}+=	rtsol
SUBDIR.${MK_NAND}+=	nandfs
SUBDIR.${MK_NAND}+=	newfs_nandfs
SUBDIR.${MK_PF}+=	pfctl
SUBDIR.${MK_PF}+=	pflogd
SUBDIR.${MK_ROUTED}+=	routed

SUBDIR.${MK_TESTS}+=	tests

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
