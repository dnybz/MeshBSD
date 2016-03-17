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
	bluetooth \
	bzip2 \
	cap_mkdb \
	chat \
	cksum \
	cmp \
	comm \
	compile_et \
	compress \
	cpio \
	cut \
	dc \
	du \
	ee \
	elfcopy \
	env \
	expand \
	false \
	fetch \
	find \
	fold \
	fsync \
	ftp \
	getcap \
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
	mail \
	mesg \
	mkcsmapper \
	mkcsmapper_static \
	mkesdb \
	mkesdb_static \
	mkfifo \
	mktemp \
	mt \
	nc \
	ncal \
	netstat \
	nice \
	nm \
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
	size \
	sockstat \
	sort \
	split \
	stat \
	strings \
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
	vi \
	vmstat \
	vtfontcvt \
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

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O:u}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
