#	@(#)Makefile	8.1 (Berkeley) 6/4/93
#	$FreeBSD: releng/11.0/lib/libutil/Makefile 298107 2016-04-16 07:45:30Z gjb $

PACKAGE=lib${LIB}
SHLIBDIR?= /lib

.include <src.opts.mk>

LIB=	util
SHLIB_MAJOR= 9

SRCS=	_secure_path.c auth.c expand_number.c flopen.c fparseln.c gr_util.c \
	hexdump.c humanize_number.c kinfo_getfile.c \
	imsg.c imsg-buffer.c \
	kinfo_getallproc.c kinfo_getproc.c kinfo_getvmmap.c \
	kinfo_getvmobject.c kld.c \
	login_auth.c login_cap.c \
	login_class.c login_crypt.c login_ok.c login_times.c login_tty.c \
	ohash.c \
	pidfile.c property.c pty.c pw_util.c quotafile.c realhostname.c \
	stat_flags.c stub.c trimdomain.c uucplock.c
INCS=	imsg.h libutil.h login_cap.h ohash.h util.h

CFLAGS+= -DLIBC_SCCS

.if ${MK_INET6_SUPPORT} != "no"
CFLAGS+= -DINET6
.endif

CFLAGS+= -I${.CURDIR} -I${.CURDIR}/../libc/gen/

.if ${MK_TESTS} != "no"
SUBDIR+=	tests
.endif

WARNS?=		2

.include <bsd.lib.mk>
