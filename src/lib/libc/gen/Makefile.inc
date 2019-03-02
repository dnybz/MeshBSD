#	@(#)Makefile.inc	8.6 (Berkeley) 5/4/95
# $FreeBSD: releng/11.0/lib/libc/gen/Makefile.inc 303853 2016-08-08 20:15:00Z bdrewery $

# machine-independent gen sources
.PATH: ${LIBC_SRCTOP}/${LIBC_ARCH}/gen ${LIBC_SRCTOP}/gen

SRCS+=	__getosreldate.c \
	__pthread_mutex_init_calloc_cb_stub.c \
	__xuname.c \
	_once_stub.c \
	_pthread_stubs.c \
	_rand48.c \
	_spinlock_stub.c \
	_thread_init.c \
	alarm.c \
	arc4random.c \
	assert.c \
	auxv.c \
	basename.c \
	bcrypt.c \
	blowfish.c \
	cap_sandboxed.c \
	check_utility_compat.c \
	clock.c \
	clock_getcpuclockid.c \
	closedir.c \
	confstr.c \
	crypt.c \
	crypt2.c \
	cryptutil.c \
	ctermid.c \
	daemon.c \
	devname.c \
	dirfd.c \
	dirname.c \
	disklabel.c \
	dlfcn.c \
	drand48.c \
	dup3.c \
	elf_utils.c \
	erand48.c \
	err.c \
	errlst.c \
	errno.c \
	exec.c \
	fdevname.c \
	feature_present.c \
	fmtcheck.c \
	fmtmsg.c \
	fnmatch.c \
	fpclassify.c \
	frexp.c \
	fstab.c \
	ftok.c \
	fts.c \
	ftw.c \
	getbootfile.c \
	getbsize.c \
	getcap.c \
	getcwd.c \
	getdomainname.c \
	getdtablecount.c \
	getgrent.c \
	getgrouplist.c \
	gethostname.c \
	getloadavg.c \
	getlogin.c \
	getmntinfo.c \
	getnetgrent.c \
	getosreldate.c \
	getpagesize.c \
	getpagesizes.c \
	getpeereid.c \
	getprogname.c \
	getpwent.c \
	getttyent.c \
	getusershell.c \
	getutxent.c \
	getvfsbyname.c \
	glob.c \
	initgroups.c \
	isatty.c \
	isinf.c \
	isnan.c \
	jrand48.c \
	lcong48.c \
	libc_dlopen.c \
	lockf.c \
	lrand48.c \
	mrand48.c \
	nftw.c \
	nice.c \
	nlist.c \
	nrand48.c \
	opendir.c \
	pause.c \
	pmadvise.c \
	popen.c \
	posix_spawn.c \
	psignal.c \
	pututxline.c \
	pw_scan.c \
	raise.c \
	readdir.c \
	readpassphrase.c \
	recvmmsg.c \
	rewinddir.c \
	scandir.c \
	seed48.c \
	seekdir.c \
	semctl.c \
	sendmmsg.c \
	setdomainname.c \
	sethostname.c \
	setjmperr.c \
	setmode.c \
	setproctitle.c \
	setprogname.c \
	siginterrupt.c \
	siglist.c \
	signal.c \
	sigsetops.c \
	sleep.c \
	srand48.c \
	statvfs.c \
	stringlist.c \
	strtofflags.c \
	sysconf.c \
	sysctl.c \
	sysctlbyname.c \
	sysctlnametomib.c \
	syslog.c \
	telldir.c \
	termios.c \
	time.c \
	times.c \
	timezone.c \
	tls.c \
	ttyname.c \
	ttyslot.c \
	ualarm.c \
	ulimit.c \
	uname.c \
	usleep.c \
	utime.c \
	utxdb.c \
	valloc.c \
	wait.c \
	wait3.c \
	waitpid.c \
	waitid.c \
	wordexp.c
.if ${MK_SYMVER} == yes
SRCS+=	fts-compat.c \
	unvis-compat.c
.endif

.PATH: ${LIBC_SRCTOP}/../../contrib/libc-pwcache
SRCS+=	pwcache.c pwcache.h

.PATH: ${LIBC_SRCTOP}/../../contrib/libc-vis
CFLAGS+=	-I${LIBC_SRCTOP}/../../contrib/libc-vis
SRCS+=	unvis.c vis.c

MISRCS+=modf.c

CANCELPOINTS_SRCS=sem.c sem_new.c
.for src in ${CANCELPOINTS_SRCS}
SRCS+=cancelpoints_${src}
CLEANFILES+=cancelpoints_${src}
cancelpoints_${src}: ${LIBC_SRCTOP}/gen/${src} .NOMETA
	ln -sf ${.ALLSRC} ${.TARGET}
.endfor

SYM_MAPS+=${LIBC_SRCTOP}/gen/Symbol.map

# machine-dependent gen sources
.sinclude "${LIBC_SRCTOP}/${LIBC_ARCH}/gen/Makefile.inc"

