#	From: @(#)Makefile	8.3 (Berkeley) 1/7/94
# $FreeBSD: head/usr.bin/Makefile 291115 2015-11-20 23:15:05Z bapt $

.include <src.opts.mk>

SUBDIR= 	addr2line
SUBDIR+= 	alias
SUBDIR+= 	ar
SUBDIR+= 	awk
SUBDIR+= 	basename
SUBDIR+= 	bmake
SUBDIR+= 	bzip2
SUBDIR+= 	c89
SUBDIR+= 	c99
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
SUBDIR+= 	newkey
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

.if ${MK_AT} != "no"
SUBDIR+= 	at
.endif

.if ${MK_BLUETOOTH} != "no"
SUBDIR+= 	bluetooth
.endif

.if ${MK_GPL_DTC} != "yes"
SUBDIR+= usr.bin/dtc
.endif

.if ${MK_INET_SUPPORT} != "no" || \
    ${MK_INET6_SUPPORT} != "no"
SUBDIR+= 	fetch
SUBDIR+= 	netstat
SUBDIR+= 	rpcgen
SUBDIR+= 	rpcinfo
SUBDIR+= 	sockstat
SUBDIR+= 	ssh-copy-id
SUBDIR+= 	whois
.endif

.if ${MK_KERBEROS_SUPPORT} != "no"
SUBDIR+= 	compile_et
.endif

.if ${MK_LIBRESSL} != "no"
SUBDIR+= 	bdes openssl
.if ${MK_NETCAT} != "no"
SUBDIR+= 	nc
.endif
.if ${MK_OPENSSH} != "no"
SUBDIR+=scp sftp ssh ssh-add ssh-agent ssh-keygen ssh-keyscan
.endif
.endif

.if ${MK_MAIL} != "no" 
SUBDIR+= 	mail
.endif 

.if ${MK_NIS} != "no"
SUBDIR+= 	ypcat
SUBDIR+= 	ypmatch
SUBDIR+= 	ypwhich
.endif

.if ${MK_RCMDS} != "no"
SUBDIR+= 	rlogin
SUBDIR+= 	rsh
.endif

.if ${MK_TFTP} != "no"
SUBDIR+= 	tftp
.endif

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O:u}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
