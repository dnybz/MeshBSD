# $FreeBSD: head/etc/devd/Makefile 298140 2016-04-17 03:45:45Z gjb $

.include <src.opts.mk>

FILEGROUPS=	FILES

SUBDIR=	\
	autofs \
	bluetooth \
	casper \
	cfg \
	devd \
	pam.d \
	ppp \
	rc.d

FILES+= crontab
FILES+= ddb.conf
FILES+= devd.conf
FILES+= devfs.conf
FILES+= dhclient.conf
FILES+= dhcpd.conf
FILES+= fbtab
FILES+= ftpusers
FILES+= gettytab
FILES+= group
FILES+= hostapd.conf
FILES+= hosts
FILES+= hosts.allow
FILES+= hosts.equiv
FILES+= inetd.conf
FILES+= ipsec.conf
FILES+= libalias.conf
FILES+= login.access
FILES+= login.conf
FILES+= mac.conf
FILES+= Makefile
FILES+= master.passwd
FILES+= myname
FILES+= netconfig
FILES+= networks
FILES+= newsyslog.conf
FILES+= nsswitch.conf
FILES+= ntp.conf
FILES+= passwd
FILES+= pf.conf
FILES+= phones
FILES+= profile
FILES+= protocols
FILES+= rc.conf
FILES+= rc.subr
FILES+= rc2
FILES+= remote
FILES+= rpc
FILES+= services
FILES+= shells
FILES+= sysctl.conf
FILES+= syslog.conf
FILES+= wpa_supplicant.conf

.if exists(${.CURDIR}/etc.${MACHINE}/ttys)
BIN1+=	etc.${MACHINE}/ttys
.elif exists(${.CURDIR}/etc.${MACHINE_ARCH}/ttys)
BIN1+=	etc.${MACHINE_ARCH}/ttys
.elif exists(${.CURDIR}/etc.${MACHINE_CPUARCH}/ttys)
BIN1+=	etc.${MACHINE_CPUARCH}/ttys
.else
.error etc.MACHINE/ttys missing
.endif

NO_OBJ=
FILESDIR=	/etc/c
FILESMODE=	644

.include <bsd.prog.mk>
