#	From: @(#)Makefile	8.3 (Berkeley) 1/7/94
# $FreeBSD: head/usr.bin/Makefile 291115 2015-11-20 23:15:05Z bapt $

.include <src.opts.mk>

# XXX MISSING:		deroff diction graph learn plot
#			spell spline struct xsend
# XXX Use GNU versions: diff ld patch
# Moved to secure: bdes
#

SUBDIR=	alias \
	ar \
	at \
	ath \
	awk \
	bzip2 \
	cap_mkdb \
	cut \
	false \
	fetch \
	grep \
	gzip \
	head \
	hexdump \
	less \
	login \
	m4 \
	man \
	ncal \
	netstat \
	passwd \
	printenv \
	printf \
	sed \
	sockstat \
	sort \
	su \
	systat \
	tail \
	tar \
	touch \
	tput \
	true \
	tset \
	uname \
	uniq \
	unzip \
	vmstat \
	w \
	wall 

# NB: keep these sorted by MK_* knobs

SUBDIR.${MK_AT}+=	at
SUBDIR.${MK_BLUETOOTH}+=	bluetooth
SUBDIR.${MK_BSD_CPIO}+=	cpio
SUBDIR.${MK_EE}+=	ee

SUBDIR.${MK_MAN_UTILS}+=	man

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O:u}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
