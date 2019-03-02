#	From: @(#)Makefile	8.3 (Berkeley) 1/7/94
# $FreeBSD: releng/11.0/usr.bin/Makefile 299529 2016-05-12 10:16:16Z mm $

.include <src.opts.mk>

# XXX MISSING:		deroff diction graph learn plot
#			spell spline struct xsend
# XXX Use GNU versions: diff ld patch
# Moved to secure: bdes
#

SUBDIR=	alias \
	apply \
	awk \
	basename \
	brandelf \
	bsdcat \
	bsdiff \
	bzip2 \
	bzip2recover \
	cap_mkdb \
	chat \
	chpass \
	cksum \
	cmp \
	col \
	colldef \
	colrm \
	column \
	comm \
	compress \
	cpuset \
	csplit \
	ctlstat \
	cut \
	dirname \
	dpv \
	du \
	elf2aout \
	elfdump \
	env \
	expand \
	false \
	fetch \
	find \
	fmt \
	fold \
	fstat \
	fsync \
	gcore \
	gencat \
	getconf \
	getent \
	getopt \
	grep \
	gzip \
	head \
	hexdump \
	id \
	ident \
	ipcrm \
	ipcs \
	join \
	keylogin \
	keylogout \
	killall \
	ktrace \
	ktrdump \
	lam \
	lastcomm \
	ldd \
	leave \
	less \
	lessecho \
	lesskey \
	limits \
	locale \
	localedef \
	lock \
	lockf \
	logger \
	login \
	logins \
	logname \
	look \
	lorder \
	lsvfs \
	lzmainfo \
	m4 \
	mesg \
	minigzip \
	mkdep \
	mkfifo \
	mkimg \
	mklocale \
	mktemp \
	mkuzip \
	ncal \
	netstat \
	newgrp \
	nice \
	nl \
	numactl \
	nohup \
	opieinfo \
	opiekey \
	opiepasswd \
	pagesize \
	passwd \
	paste \
	patch \
	pathchk \
	perror \
	pr \
	printenv \
	printf \
	procstat \
	protect \
	rctl \
	renice \
	resizewin \
	rev \
	revoke \
	rpcinfo \
	rs \
	rup \
	script \
	sed \
	shar \
	showmount \
	sockstat \
	soelim \
	sort \
	split \
	stat \
	stdbuf \
	su \
	systat \
	tabs \
	tail \
	tar \
	tee \
	time \
	timeout \
	tip \
	top \
	touch \
	tput \
	tr \
	true \
	truncate \
	tset \
	tsort \
	tty \
	uname \
	unexpand \
	uniq \
	unzip \
	units \
	unvis \
	vis \
	vmstat \
	w \
	wc \
	what \
	whereis \
	which \
	xargs \
	xinstall \
	xz \
	xzdec \
	yes

# NB: keep these sorted by MK_* knobs

SUBDIR.${MK_AT}+=	at
SUBDIR.${MK_BSD_CPIO}+=	cpio
SUBDIR.${MK_CALENDAR}+=	calendar
SUBDIR.${MK_CLANG}+=	clang
SUBDIR.${MK_EE}+=	ee
SUBDIR.${MK_FILE}+=	file
SUBDIR.${MK_FTP}+=	ftp

.if ${MK_GPL_DTC} != "yes"
.if ${COMPILER_FEATURES:Mc++11}
SUBDIR+=	dtc
.endif
.endif
SUBDIR.${MK_GROFF}+=	vgrind
SUBDIR.${MK_HESIOD}+=	hesinfo
SUBDIR.${MK_ICONV}+=	iconv
SUBDIR.${MK_ICONV}+=	mkcsmapper
SUBDIR.${MK_ICONV}+=	mkesdb
SUBDIR.${MK_KDUMP}+=	kdump
SUBDIR.${MK_KDUMP}+=	truss
SUBDIR.${MK_LDNS_UTILS}+=	drill
SUBDIR.${MK_LDNS_UTILS}+=	host
SUBDIR.${MK_MAKE}+=	bmake
SUBDIR.${MK_NETCAT}+=	nc
SUBDIR.${MK_OPENSSH}+=	ssh-copy-id
SUBDIR.${MK_LIBRESSL}+=	bc
SUBDIR.${MK_LIBRESSL}+=	chkey
SUBDIR.${MK_LIBRESSL}+=	dc
SUBDIR.${MK_LIBRESSL}+=	newkey
SUBDIR.${MK_QUOTAS}+=	quota
SUBDIR.${MK_RCMDS}+=	rlogin
SUBDIR.${MK_RCMDS}+=	rsh
SUBDIR.${MK_RCMDS}+=	ruptime
SUBDIR.${MK_TESTS}+=	tests
SUBDIR.${MK_TEXTPROC}+=	checknr
SUBDIR.${MK_TEXTPROC}+=	colcrt
SUBDIR.${MK_TEXTPROC}+=	ul
SUBDIR.${MK_TFTP}+=	tftp
SUBDIR.${MK_TOOLCHAIN}+=	addr2line
SUBDIR.${MK_TOOLCHAIN}+=	ar
SUBDIR.${MK_TOOLCHAIN}+=	c89
SUBDIR.${MK_TOOLCHAIN}+=	c99
SUBDIR.${MK_TOOLCHAIN}+=	ctags
SUBDIR.${MK_TOOLCHAIN}+=	cxxfilt
SUBDIR.${MK_TOOLCHAIN}+=	elfcopy
SUBDIR.${MK_TOOLCHAIN}+=	file2c
.if ${MACHINE_CPUARCH} != "riscv" # RISCVTODO gprof does not build
SUBDIR.${MK_TOOLCHAIN}+=	gprof
.endif
SUBDIR.${MK_TOOLCHAIN}+=	indent
SUBDIR.${MK_TOOLCHAIN}+=	lex
SUBDIR.${MK_TOOLCHAIN}+=	mkstr
SUBDIR.${MK_TOOLCHAIN}+=	nm
SUBDIR.${MK_TOOLCHAIN}+=	readelf
SUBDIR.${MK_TOOLCHAIN}+=	rpcgen
SUBDIR.${MK_TOOLCHAIN}+=	unifdef
SUBDIR.${MK_TOOLCHAIN}+=	size
SUBDIR.${MK_TOOLCHAIN}+=	strings
SUBDIR.${MK_TOOLCHAIN}+=	xlint
SUBDIR.${MK_TOOLCHAIN}+=	xstr
SUBDIR.${MK_TOOLCHAIN}+=	yacc
SUBDIR.${MK_VI}+=	vi
SUBDIR.${MK_VT}+=	vtfontcvt
SUBDIR.${MK_UTMPX}+=	last
.if ${MACHINE_CPUARCH} != "riscv" # RISCVTODO users does not build
SUBDIR.${MK_UTMPX}+=	users
.endif
SUBDIR.${MK_UTMPX}+=	who

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O:u}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
