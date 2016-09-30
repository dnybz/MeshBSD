# $FreeBSD: head/lib/libprocstat/Makefile 298107 2016-04-16 07:45:30Z gjb $

.include <src.opts.mk>

PACKAGE=lib${LIB}
LIB=	procstat

SRCS=	common_kvm.c	\
	core.c		\
	libprocstat.c	\
        msdosfs.c

VERSION_DEF=	${.CURDIR}/Versions.def
SYMBOL_MAPS=	${.CURDIR}/Symbol.map

INCS=		libprocstat.h
CFLAGS+=	-I. -I${.CURDIR} -D_KVM_VNODE
SHLIB_MAJOR=	1

LIBADD=		elf kvm util

MAN=		libprocstat.3
MLINKS+=libprocstat.3 procstat_close.3 \
		libprocstat.3 procstat_freeargv.3 \
		libprocstat.3 procstat_freeauxv.3 \
		libprocstat.3 procstat_freeenvv.3 \
		libprocstat.3 procstat_freefiles.3 \
		libprocstat.3 procstat_freegroups.3 \
		libprocstat.3 procstat_freekstack.3 \
		libprocstat.3 procstat_freeprocs.3 \
		libprocstat.3 procstat_freevmmap.3 \
		libprocstat.3 procstat_get_pipe_info.3 \
		libprocstat.3 procstat_get_pts_info.3 \
		libprocstat.3 procstat_get_sem_info.3 \
		libprocstat.3 procstat_get_shm_info.3 \
		libprocstat.3 procstat_get_socket_info.3 \
		libprocstat.3 procstat_get_vnode_info.3 \
		libprocstat.3 procstat_getargv.3 \
		libprocstat.3 procstat_getauxv.3 \
		libprocstat.3 procstat_getenvv.3 \
		libprocstat.3 procstat_getfiles.3 \
		libprocstat.3 procstat_getgroups.3 \
		libprocstat.3 procstat_getkstack.3 \
		libprocstat.3 procstat_getosrel.3 \
		libprocstat.3 procstat_getpathname.3 \
		libprocstat.3 procstat_getprocs.3 \
		libprocstat.3 procstat_getrlimit.3 \
		libprocstat.3 procstat_getumask.3 \
		libprocstat.3 procstat_getvmmap.3 \
		libprocstat.3 procstat_open_core.3 \
		libprocstat.3 procstat_open_kvm.3 \
		libprocstat.3 procstat_open_sysctl.3

.include <bsd.lib.mk>
