#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: head/lib/Makefile 298107 2016-04-16 07:45:30Z gjb $

.include <src.opts.mk>

#
# XXX: after replacement of OpenSSL by LibreSSL,
# XXX: libtls(3) as dependency for httpd(8) shall 
# XXX: integrated on library set
#

# The SUBDIR_ORDERED list is a small set of libraries which are used by many
# of the other libraries.  These are built first with a .WAIT between them
# and the main list to avoid needing a SUBDIR_DEPEND line on every library
# naming just these few items.

SUBDIR_ORDERED=	csu \
	.WAIT \
	libc \
	libc_nonshared \
	libcompiler_rt \
	${_libclang_rt} \
	libelf \
	msun

# The main list; please keep these sorted alphabetically.

SUBDIR=	${SUBDIR_ORDERED} \
	.WAIT \
	libalias \
	libarchive \
	libbegemot \
	libbluetooth \
	libbsnmp \
	libbsm \
	libbz2 \
	libcalendar \
	libcam \
	libcasper \
	libcom_err \
	libcrypt \
	libdevctl \
	libdevinfo \
	libdevstat \
	libedit \
	libelftc \
	libevent \
	libexpat \
	libfetch \
	libgeom \
	libgpio \
	libgssapi \
	libipsec \
	libjail \
	libkiconv \
	libkvm \
	${_libldns} \
	liblzma \
	libmemstat \
	libmd \
	${_libmp} \
	libmt \
	libnandfs \
	lib80211 \
	libnetbsd \
	libnetgraph \
	libnv \
	libopie \
	libpam \
	libpcap \
	libpjdlog \
	libpmc \
	${_libproc} \
	libprocstat \
	libradius \
	librpcsvc \
	librtld_db \
	libsbuf \
	libtacplus \
	libthr \
	libthread_db \
	libufs \
	libugidfw \
	libusbhid \
	libusb \
	libutil \
	${_libvgl} \
	libwrap \
	libxo \
	libz \
	ncurses \
	${_tests}

# Inter-library dependencies.  When the makefile for a library contains LDADD
# libraries, those libraries should be listed as build order dependencies here.

SUBDIR_DEPEND_libarchive= libz libbz2 libexpat liblzma libmd
SUBDIR_DEPEND_libbsnmp= libnetgraph
SUBDIR_DEPEND_libc= libcompiler_rt
SUBDIR_DEPEND_libcam= libsbuf
SUBDIR_DEPEND_libcapsicum= libnv
SUBDIR_DEPEND_libcasper= libcapsicum libnv libpjdlog
SUBDIR_DEPEND_libdevstat= libkvm
SUBDIR_DEPEND_libedit= ncurses
SUBDIR_DEPEND_libgeom= libexpat libsbuf
SUBDIR_DEPEND_libmemstat= libkvm
SUBDIR_DEPEND_libopie= libmd
SUBDIR_DEPEND_libpam= libcrypt libopie libradius librpcsvc libtacplus libutil ${_libypclnt} libcom_err
SUBDIR_DEPEND_libprocstat= libkvm libutil
SUBDIR_DEPEND_libradius= libmd
SUBDIR_DEPEND_libtacplus= libmd
SUBDIR_DEPEND_liblzma= libthr

# The libraries under libclang_rt can only be built by clang, and only make
# sense to build when clang is enabled at all.  Furthermore, they can only be
# built for certain architectures.
.if ${MK_CLANG} != "no" && ${COMPILER_TYPE} == "clang" && \
    (${MACHINE_CPUARCH} == "aarch64" || ${MACHINE_CPUARCH} == "amd64" || \
    (${MACHINE_CPUARCH} == "arm" && ${MACHINE_ARCH} != "armeb") || \
    (${MACHINE_CPUARCH} == "i386"))
_libclang_rt=	libclang_rt
.endif

.if ${MACHINE_CPUARCH} == "i386" || ${MACHINE_CPUARCH} == "amd64"
_libvgl=	libvgl
_libproc=	libproc
.endif

.if ${MACHINE_CPUARCH} == "mips"
_libproc=	libproc
.endif

.if ${MACHINE_CPUARCH} == "powerpc"
_libproc=	libproc
.endif

.if ${MACHINE_CPUARCH} == "aarch64" || ${MACHINE_CPUARCH} == "arm"
_libproc=	libproc
.endif

.if ${MK_OPENSSL} != "no"
_libmp=		libmp
.endif

.if ${MK_SENDMAIL} != "no"
_libmilter=	libmilter
_libsm=		libsm
_libsmdb=	libsmdb
_libsmutil=	libsmutil
.endif

.if ${MK_TESTS} != "no"
_tests=		tests
.endif

.if !make(install)
SUBDIR_PARALLEL=
.endif

.include <bsd.subdir.mk>
