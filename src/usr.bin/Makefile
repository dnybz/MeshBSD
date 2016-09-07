#	From: @(#)Makefile	8.3 (Berkeley) 1/7/94
# $FreeBSD: head/usr.bin/Makefile 291115 2015-11-20 23:15:05Z bapt $

.include <src.opts.mk>

SUBDIR= 	addr2line
SUBDIR+= 	alias
SUBDIR+= 	ar
SUBDIR+= 	at
SUBDIR+= 	awk
SUBDIR+= 	basename
SUBDIR+= 	bluetooth
SUBDIR+= 	bmake
SUBDIR+= 	bzip2
SUBDIR+= 	cap_mkdb
SUBDIR+= 	chkey
SUBDIR+= 	cksum
SUBDIR+= 	cmp
SUBDIR+= 	cpio
SUBDIR+= 	cut
SUBDIR+= 	dirname
SUBDIR+= 	du
SUBDIR+= 	ee
SUBDIR+= 	elfcopy
SUBDIR+= 	env
SUBDIR+= 	expand
SUBDIR+= 	false

SUBDIR+= 	find
SUBDIR+= 	fold
SUBDIR+= 	fortune
SUBDIR+= 	fstat
SUBDIR+= 	fsync
SUBDIR+= 	ftp
SUBDIR+= 	getconf
SUBDIR+= 	getent
SUBDIR+= 	getopt
SUBDIR+= 	grep
SUBDIR+= 	gzip
SUBDIR+= 	head
SUBDIR+= 	hexdump
SUBDIR+= 	host
SUBDIR+= 	iconv
SUBDIR+= 	id
SUBDIR+= 	ipcrm
SUBDIR+= 	ipcs
SUBDIR+= 	killall
SUBDIR+= 	last
SUBDIR+= 	less
SUBDIR+= 	lex
SUBDIR+= 	localedef
SUBDIR+= 	logger
SUBDIR+= 	login
SUBDIR+= 	logins
SUBDIR+= 	logname
SUBDIR+= 	lorder
SUBDIR+= 	lsvfs
SUBDIR+= 	lzmainfo
SUBDIR+= 	m4
SUBDIR+= 	makewhatis
SUBDIR+= 	man
SUBDIR+= 	mandoc
SUBDIR+= 	mesg
SUBDIR+= 	minigzip
SUBDIR+= 	mkcsmapper
SUBDIR+= 	mkcsmapper_static
SUBDIR+= 	mkesdb
SUBDIR+= 	mkesdb_static
SUBDIR+= 	mkfifo
SUBDIR+= 	mktemp
SUBDIR+= 	mkuzip
SUBDIR+= 	mt
SUBDIR+= 	ncal
SUBDIR+= 	nice
SUBDIR+= 	nm
SUBDIR+= 	nohup
SUBDIR+= 	pamtest
SUBDIR+= 	passwd
SUBDIR+= 	paste
SUBDIR+= 	patch
SUBDIR+= 	printenv
SUBDIR+= 	printf
SUBDIR+= 	procstat
SUBDIR+= 	quota
SUBDIR+= 	renice
SUBDIR+= 	script
SUBDIR+= 	sed
SUBDIR+= 	seq
SUBDIR+= 	size
SUBDIR+= 	soelim
SUBDIR+= 	sort
SUBDIR+= 	split
SUBDIR+= 	stat
SUBDIR+= 	strings
SUBDIR+= 	su
SUBDIR+= 	systat
SUBDIR+= 	tail
SUBDIR+= 	tar
SUBDIR+= 	tee
SUBDIR+= 	time
SUBDIR+= 	top
SUBDIR+= 	touch
SUBDIR+= 	tput
SUBDIR+= 	tr
SUBDIR+= 	true
SUBDIR+= 	tset
SUBDIR+= 	tsort
SUBDIR+= 	tty
SUBDIR+= 	uname
SUBDIR+= 	unexpand
SUBDIR+= 	uniq
SUBDIR+= 	unzip
SUBDIR+= 	usbhidaction
SUBDIR+= 	usbhidctl
SUBDIR+= 	users
SUBDIR+= 	uudecode
SUBDIR+= 	uuencode
SUBDIR+= 	vi
SUBDIR+= 	vmstat
SUBDIR+= 	vtfontcvt
SUBDIR+= 	w
SUBDIR+= 	wall
SUBDIR+= 	wc
SUBDIR+= 	what
SUBDIR+= 	which
SUBDIR+= 	who
SUBDIR+= 	xargs
SUBDIR+= 	xinstall
SUBDIR+= 	xz
SUBDIR+= 	xzdec
SUBDIR+= 	yacc
SUBDIR+= 	yes

.if ${MK_INET_SUPPORT} != "no" || \
    ${MK_INET6_SUPPORT} != "no"
SUBDIR+= 	fetch
SUBDIR+= 	mail
SUBDIR+= 	nc
SUBDIR+= 	netstat
SUBDIR+= 	rlogin
SUBDIR+= 	rpcgen
SUBDIR+= 	rpcinfo
SUBDIR+= 	rsh
SUBDIR+= 	sockstat
SUBDIR+= 	ssh-copy-id
SUBDIR+= 	tftp
SUBDIR+= 	whois
.endif

SUBDIR.${MK_TESTS}+=	tests

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O:u}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
