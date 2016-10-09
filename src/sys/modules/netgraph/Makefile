# $Whistle: Makefile,v 1.5 1999/01/24 06:48:37 archie Exp $
# $FreeBSD: head/sys/modules/netgraph/Makefile 290665 2015-11-10 17:15:36Z bdrewery $

SYSDIR?=${.CURDIR}/../..
.include "${SYSDIR}/conf/kern.opts.mk"

SUBDIR=	async \
	${_bluetooth} \
	ether \
	netflow \
	netgraph \
	ppp \
	pppoe \
	socket \
	tty

.if ${MK_BLUETOOTH} != "no" || defined(ALL_MODULES)
_bluetooth=	bluetooth
.endif

.if ${MK_CRYPT} != "no" && exists(${SYSDIR}/crypto/rc4/rc4.c)
_mppc=		mppc
.endif

.include <bsd.subdir.mk>
