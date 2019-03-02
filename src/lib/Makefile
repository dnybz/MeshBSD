#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: releng/11.0/lib/Makefile 301553 2016-06-07 16:31:03Z lidl $

.include <src.opts.mk>

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
	${_libcplusplus} \
	${_libcxxrt} \
	libelf \
	msun

# The main list; please keep these sorted alphabetically.

SUBDIR=	${SUBDIR_ORDERED} \
	.WAIT \
	libalias \
	libarchive \
	libbegemot \
	${_libblacklist} \
	libblocksruntime \
	libbsdstat \
	libbz2 \
	libcalendar \
	libcam \
	${_libcasper} \
	libcompat \
	libcrypt \
	libdevctl \
	libdevdctl \
	libdevinfo \
	libdevstat \
	libdpv \
	libdwarf \
	libedit \
	${_libelftc} \
	libevent \
	libexecinfo \
	libexpat \
	libfetch \
	libfigpar \
	libgeom \
	${_libgpio} \
	${_libgssapi} \
	${_librpcsec_gss} \
	${_libiconv_modules} \
	libipsec \
	libjail \
	libkiconv \
	libkvm \
	${_libldns} \
	liblzma \
	${_libmagic} \
	libmemstat \
	libmd \
	${_libmp} \
	libmt \
	${_libnandfs} \
	lib80211 \
	${_libnetgraph} \
	${_libngatm} \
	libnv \
	libopie \
	libpam \
	libpcap \
	${_libpe} \
	libpjdlog \
	${_libpmc} \
	${_libproc} \
	libprocstat \
	libracoon \
	${_libradius} \
	librpcsvc \
	librt \
	${_librtld_db} \
	libsbuf \
	${_libsdp} \
	libsqlite3 \
	libstdbuf \
	libstdthreads \
	libsysdecode \
	libtacplus \
	${_libthr} \
	libthread_db \
	libucl \
	libufs \
	libugidfw \
	libulog \
	${_libunbound} \
	${_libusbhid} \
	${_libusb} \
	libutil \
	libwrap \
	libxo \
	liby \
	libz \
	ncurses \
	${_atf} \
	${_clang} \
	${_tests}

# Inter-library dependencies.  When the makefile for a library contains LDADD
# libraries, those libraries should be listed as build order dependencies here.

SUBDIR_DEPEND_libarchive= libz libbz2 libexpat liblzma libmd
SUBDIR_DEPEND_libc++:= libcxxrt
SUBDIR_DEPEND_libc= libcompiler_rt
SUBDIR_DEPEND_libcam= libsbuf
SUBDIR_DEPEND_libcasper= libnv
SUBDIR_DEPEND_libdevstat= libkvm
SUBDIR_DEPEND_libdpv= libfigpar ncurses libutil
SUBDIR_DEPEND_libedit= ncurses
SUBDIR_DEPEND_libgeom= libexpat libsbuf
SUBDIR_DEPEND_liblibrpcsec_gss= libgssapi
SUBDIR_DEPEND_libmagic= libz
SUBDIR_DEPEND_libmemstat= libkvm
SUBDIR_DEPEND_libopie= libmd
SUBDIR_DEPEND_libpam= libcrypt libopie ${_libradius} librpcsvc libtacplus libutil 
SUBDIR_DEPEND_libpjdlog= libutil
SUBDIR_DEPEND_libprocstat= libkvm libutil
SUBDIR_DEPEND_libradius= libmd
SUBDIR_DEPEND_libtacplus= libmd
SUBDIR_DEPEND_libulog= libmd
SUBDIR_DEPEND_libunbound= ${_libldns}
SUBDIR_DEPEND_liblzma= ${_libthr}

# NB: keep these sorted by MK_* knobs

.if ${MK_BLACKLIST} != "no"
_libblacklist=	libblacklist
.endif

.if ${MK_CASPER} != "no"
_libcasper=	libcasper
.endif

.if ${MK_CLANG} != "no" && !defined(COMPAT_32BIT) && !defined(COMPAT_SOFTFP)
_clang=		clang
.endif

.if ${MK_TOOLCHAIN} != "no"
_libelftc=	libelftc
_libpe=		libpe
.endif

.if ${MK_FILE} != "no"
_libmagic=	libmagic
.endif

.if ${MK_GPIO} != "no"
_libgpio=	libgpio
.endif

.if ${MK_GSSAPI} != "no"
_libgssapi=	libgssapi
_librpcsec_gss=	librpcsec_gss
.endif

.if ${MK_ICONV} != "no"
_libiconv_modules=	libiconv_modules
.endif

.if ${MK_LDNS} != "no"
_libldns=	libldns
.endif

# The libraries under libclang_rt can only be built by clang, and only make
# sense to build when clang is enabled at all.  Furthermore, they can only be
# built for certain architectures.
.if ${MK_CLANG} != "no" && ${COMPILER_TYPE} == "clang" && \
    (${MACHINE_CPUARCH} == "aarch64" || ${MACHINE_CPUARCH} == "amd64" || \
    (${MACHINE_CPUARCH} == "arm" && ${MACHINE_ARCH} != "armeb") || \
    (${MACHINE_CPUARCH} == "i386"))
_libclang_rt=	libclang_rt
.endif

.if ${MK_LIBCPLUSPLUS} != "no"
_libcxxrt=	libcxxrt
_libcplusplus=	libc++
.endif

.if ${MK_LIBTHR} != "no"
_libthr=	libthr
.endif

.if ${MK_NAND} != "no"
_libnandfs=	libnandfs
.endif

.if ${MK_NETGRAPH} != "no"
_libnetgraph=	libnetgraph
.endif

.if ${MACHINE_CPUARCH} == "mips" || ${MACHINE_CPUARCH} == "riscv"
_libproc=	libproc
_librtld_db=	librtld_db
.endif

.if ${MK_LIBRESSL} != "no"
_libmp=		libmp
.endif

.if ${MK_PMC} != "no"
_libpmc=	libpmc
.endif

.if ${MK_RADIUS_SUPPORT} != "no"
_libradius=	libradius
.endif

.if ${MK_TESTS_SUPPORT} != "no"
_atf=		atf
.endif
.if ${MK_TESTS} != "no"
_tests=		tests
.endif

.if ${MK_UNBOUND} != "no"
_libunbound=	libunbound
.endif

.if ${MK_USB} != "no"
_libusbhid=	libusbhid
_libusb=	libusb
.endif

.if !make(install)
SUBDIR_PARALLEL=
.endif

.include <bsd.subdir.mk>
