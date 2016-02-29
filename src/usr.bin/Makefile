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
	chat \
	cksum \
	cmp \
	comm \
	compile_et \
	compress \
	cut \
	dc \
	du \
	env \
	expand \
	false \
	fetch \
	find \
	fold \
	fsync \
	ftp \
	getopt \
	grep \
	gzip \
	head \
	hexdump \
	host \
	id \
	ipcrm \
	ipcs \
	killall \
	last \
	less \
	logger \
	login \
	logname \
	lorder \
	m4 \
	man \
	mesg \
	mkfifo \
	mktemp \
	mt \
	ncal \
	netstat \
	nice \
	nohub \
	patch \
	passwd \
	printenv \
	printf \
	renice \
	rpcgen \
	script \
	sed \
	seq \
	sockstat \
	sort \
	split \
	stat \
	su \
	systat \
	tail \
	tar \
	tee \
	telnet \
	tftp \
	time \
	touch \
	top \
	tput \
	tr \
	true \
	tset \
	tty \
	uname \
	unexpand \
	uniq \
	unzip \
	uudecode \
	uuencode \
	vmstat \
	w \
	wall \
	wc \
	which \
	who \
	whois \
	xargs \
	xinstall \
	xz \
	xzdec \
	yes \
	yacc

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
