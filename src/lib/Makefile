#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: head/lib/Makefile 298107 2016-04-16 07:45:30Z gjb $

.include <src.opts.mk>

#
# XXX: this file is still subject of ongoing change...
#

# The SUBDIR_ORDERED list is a small set of libraries which are used by many
# of the other libraries.  These are built first with a .WAIT between them
# and the main list to avoid needing a SUBDIR_DEPEND line on every library
# naming just these few items.

SUBDIR_ORDERED= 	csu 
SUBDIR_ORDERED+= 	.WAIT 
SUBDIR_ORDERED+= 	libc
SUBDIR_ORDERED+= 	libc_nonshared
SUBDIR_ORDERED+= 	libcompiler_rt
SUBDIR_ORDERED+= 	${_libcplusplus}
SUBDIR_ORDERED+= 	${_libcxxrt}
SUBDIR_ORDERED+= 	libelf
SUBDIR_ORDERED+= 	msun

# The main list; please keep these sorted alphabetically.

SUBDIR= 	${SUBDIR_ORDERED}
SUBDIR+= 	.WAIT
SUBDIR+= 	libalias
SUBDIR+= 	libarchive
SUBDIR+= 	libauditd
SUBDIR+= 	libbegemot
SUBDIR+= 	libblocksruntime
SUBDIR+= 	${_libbluetooth}
SUBDIR+= 	${_libbsnmp}
SUBDIR+= 	libbsm
SUBDIR+= 	libbz2
SUBDIR+= 	libcalendar
SUBDIR+= 	libcam
SUBDIR+= 	${_libcasper}
SUBDIR+= 	${_libcom_err}
SUBDIR+= 	libcompat
SUBDIR+= 	libcrypt
SUBDIR+= 	${_libcrypto}
SUBDIR+= 	libdevctl 
SUBDIR+= 	libdevinfo
SUBDIR+= 	libdevstat
SUBDIR+= 	libdpv
SUBDIR+= 	libdwarf
SUBDIR+= 	libedit
SUBDIR+= 	${_libelftc}
SUBDIR+= 	libevent
SUBDIR+= 	libexecinfo
SUBDIR+= 	libexpat
SUBDIR+= 	libfetch
SUBDIR+= 	libfigpar
SUBDIR+= 	libgeom
SUBDIR+= 	${_libgpio}
SUBDIR+= 	${_libgssapi}
SUBDIR+= 	${_librpcsec_gss}
SUBDIR+= 	${_libiconv_modules}
SUBDIR+= 	libipsec
SUBDIR+= 	libjail
SUBDIR+= 	libkiconv
SUBDIR+= 	libkvm
SUBDIR+= 	${_libldns}
SUBDIR+= 	liblzma
SUBDIR+= 	${_libmagic}
SUBDIR+= 	libmemstat
SUBDIR+= 	libmd
SUBDIR+= 	${_libmp}
SUBDIR+= 	libmt 
SUBDIR+= 	${_libnandfs}
SUBDIR+= 	lib80211
SUBDIR+= 	libnetbsd
SUBDIR+= 	${_libnetgraph}
SUBDIR+= 	libnv
SUBDIR+= 	libopenbsd
SUBDIR+= 	libopie
SUBDIR+= 	libpam
SUBDIR+= 	libpcap
SUBDIR+= 	${_libpe}
SUBDIR+= 	libpjdlog
SUBDIR+= 	${_libpmc}
SUBDIR+= 	${_libproc}
SUBDIR+= 	libprocstat
SUBDIR+= 	${_libradius}
SUBDIR+= 	librpcsvc
SUBDIR+= 	librt
SUBDIR+= 	${_librtld_db}
SUBDIR+= 	libsbuf
SUBDIR+= 	${_libsdp}
SUBDIR+= 	libsqlite3
SUBDIR+= 	${_libssl}
SUBDIR+= 	${_libssh}
SUBDIR+= 	libstdbuf
SUBDIR+= 	libstdthreads
SUBDIR+= 	libsysdecode
SUBDIR+= 	libtacplus
SUBDIR+= 	${_libthr}
SUBDIR+= 	libtls
SUBDIR+= 	libthread_db
SUBDIR+= 	${_libucl}
SUBDIR+= 	libufs
SUBDIR+= 	libugidfw
SUBDIR+= 	libulog
SUBDIR+= 	${_libusbhid}
SUBDIR+= 	${_libusb}
SUBDIR+= 	libutil
SUBDIR+= 	${_libvgl}
SUBDIR+= 	${_libvmmapi}
SUBDIR+= 	libwrap
SUBDIR+= 	${_libxo}
SUBDIR+= 	liby
SUBDIR+= 	${_libypclnt}
SUBDIR+= 	libz
SUBDIR+= 	ncurses
SUBDIR+= 	${_atf}
SUBDIR+= 	${_clang}
SUBDIR+= 	${_cuse}
SUBDIR+= 	${_tests}

# Inter-library dependencies.  When the makefile for a library contains LDADD
# libraries, those libraries should be listed as build order dependencies here.

SUBDIR_DEPEND_libarchive= libz libbz2 libexpat liblzma libmd
SUBDIR_DEPEND_libauditdm= libbsm
SUBDIR_DEPEND_libbsnmp= ${_libnetgraph}
SUBDIR_DEPEND_libc++:= libcxxrt
SUBDIR_DEPEND_libc= libcompiler_rt
SUBDIR_DEPEND_libcam= libsbuf
SUBDIR_DEPEND_libcasper= libnv
SUBDIR_DEPEND_libdevstat= libkvm
SUBDIR_DEPEND_libdpv= libfigpar ncurses libutil
SUBDIR_DEPEND_libedit= ncurses
SUBDIR_DEPEND_libgeom= libexpat libsbuf
SUBDIR_DEPEND_librpcsec_gss= libgssapi
SUBDIR_DEPEND_libmagic= libz
SUBDIR_DEPEND_libmemstat= libkvm
SUBDIR_DEPEND_libopie= libmd
SUBDIR_DEPEND_libpam= libcrypt libopie ${_libradius} librpcsvc libtacplus libutil ${_libypclnt} ${_libcom_err} 
SUBDIR_DEPEND_libpjdlog= libutil
SUBDIR_DEPEND_libprocstat= libkvm libutil
SUBDIR_DEPEND_libtls= libssl libcrypto
SUBDIR_DEPEND_libradius= libmd
SUBDIR_DEPEND_libsmb= libkiconv
SUBDIR_DEPEND_libstdc++:= msun
SUBDIR_DEPEND_libtacplus= libmd
SUBDIR_DEPEND_libulog= libmd
SUBDIR_DEPEND_liblzma= ${_libthr}
SUBDIR_DEPEND_lib80211= libsbuf libexpat

# NB: keep these sorted by MK_* knobs

.if ${MK_ATF} != "no"
_atf=		atf
.endif

.if ${MK_BLUETOOTH} != "no"
_libbluetooth=	libbluetooth
_libsdp=	libsdp
.endif

.if ${MK_BSNMP} != "no"
_libbsnmp=	libbsnmp
.endif

.if ${MK_CASPER} != "no"
_libcasper=	libcasper
.endif

.if ${MK_CUSE} != "no"
_cuse=		libcuse
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
_libgssapi= 	libgssapi
_librpcsec_gss= 	librpcsec_gss
.endif

.if ${MK_ICONV} != "no"
_libiconv_modules=	libiconv_modules
.endif

.if ${MK_KERBEROS_SUPPORT} != "no"
_libcom_err= libcom_err
.endif

.if ${MK_LDNS} != "no"
_libldns=	libldns
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

.if ${MK_NIS} != "no"
_libypclnt=	libypclnt
.endif

.if ${MACHINE_CPUARCH} == "arm" || \
	${MACHINE_CPUARCH} == "mips" || \
    ${MACHINE_CPUARCH} == "riscv"
_libproc=	libproc
_librtld_db=	librtld_db
.endif

.if ${MK_LIBRESSL} != "no"
_libmp=		libmp
_libcrypto= 	libcrypto
_libssl= 	libssl

.if ${MK_OPENSSH} != "no"
_libssh= 	libssh
.endif
SUBDIR+= 	libtls
.endif

.if ${MK_PMC} != "no"
_libpmc=	libpmc
.endif

.if ${MK_RADIUS_SUPPORT} != "no"
_libradius=	libradius
.endif

.if ${MK_UCL} != "no"
_libucl= 	libucl
.endif

.if ${MK_USB} != "no"
_libusbhid=	libusbhid
_libusb=	libusb
.endif

.if ${MK_XO} != "no"
_libxo= 	libxo
.endif

.if !make(install)
SUBDIR_PARALLEL=
.endif

.include <bsd.subdir.mk>
