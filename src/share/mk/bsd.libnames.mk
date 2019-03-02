# $FreeBSD: releng/11.0/share/mk/bsd.libnames.mk 301226 2016-06-02 19:06:04Z lidl $

# The include file <bsd.libnames.mk> define library names.
# Other include files (e.g. bsd.prog.mk, bsd.lib.mk) include this
# file where necessary.

.if !target(__<bsd.init.mk>__)
.error bsd.libnames.mk cannot be included directly.
.endif

.sinclude <src.libnames.mk>

# Src directory locations are also defined in src.libnames.mk.

LIBCRT0?=	${DESTDIR}${LIBDIR}/crt0.o

LIB80211?=	${DESTDIR}${LIBDIR}/lib80211.a
LIBALIAS?=	${DESTDIR}${LIBDIR}/libalias.a
LIBARCHIVE?=	${DESTDIR}${LIBDIR}/libarchive.a

LIBBEGEMOT?=	${DESTDIR}${LIBDIR}/libbegemot.a
LIBBLACKLIST?=	${DESTDIR}${LIBDIR}/libblacklist.a
LIBBSDXML?=	${DESTDIR}${LIBDIR}/libbsdxml.a

LIBBZ2?=	${DESTDIR}${LIBDIR}/libbz2.a
LIBC?=		${DESTDIR}${LIBDIR}/libc.a
LIBCALENDAR?=	${DESTDIR}${LIBDIR}/libcalendar.a
LIBCAM?=	${DESTDIR}${LIBDIR}/libcam.a
LIBCAP_DNS?=	${DESTDIR}${LIBDIR}/libcap_dns.a
LIBCAP_GRP?=	${DESTDIR}${LIBDIR}/libcap_grp.a
LIBCAP_PWD?=	${DESTDIR}${LIBDIR}/libcap_pwd.a
LIBCAP_RANDOM?=	${DESTDIR}${LIBDIR}/libcap_random.a
LIBCAP_SYSCTL?=	${DESTDIR}${LIBDIR}/libcap_sysctl.a
LIBCASPER?=	${DESTDIR}${LIBDIR}/libcasper.a
LIBCOMPAT?=	${DESTDIR}${LIBDIR}/libcompat.a
LIBCOMPILER_RT?=${DESTDIR}${LIBDIR}/libcompiler_rt.a
LIBCPLUSPLUS?=	${DESTDIR}${LIBDIR}/libc++.a
LIBCRYPT?=	${DESTDIR}${LIBDIR}/libcrypt.a
LIBCRYPTO?=	${DESTDIR}${LIBDIR}/libcrypto.a

LIBCURSES?=	${DESTDIR}${LIBDIR}/libcurses.a
LIBCXXRT?=	${DESTDIR}${LIBDIR}/libcxxrt.a
LIBC_PIC?=	${DESTDIR}${LIBDIR}/libc_pic.a
LIBDEVCTL?=	${DESTDIR}${LIBDIR}/libdevctl.a
LIBDEVDCTL?=	${DESTDIR}${LIBDIR}/libdevdctl.a
LIBDEVINFO?=	${DESTDIR}${LIBDIR}/libdevinfo.a
LIBDEVSTAT?=	${DESTDIR}${LIBDIR}/libdevstat.a
LIBDIALOG?=	${DESTDIR}${LIBDIR}/libdialog.a
LIBDNS?=	${DESTDIR}${LIBDIR}/libdns.a
LIBDPV?=	${DESTDIR}${LIBDIR}/libdpv.a

LIBDWARF?=	${DESTDIR}${LIBDIR}/libdwarf.a
LIBEDIT?=	${DESTDIR}${LIBDIR}/libedit.a
LIBELF?=	${DESTDIR}${LIBDIR}/libelf.a
LIBEXECINFO?=	${DESTDIR}${LIBDIR}/libexecinfo.a
LIBFETCH?=	${DESTDIR}${LIBDIR}/libfetch.a
LIBFIGPAR?=	${DESTDIR}${LIBDIR}/libfigpar.a
LIBFL?=		"don't use LIBFL, use LIBL"
LIBFORM?=	${DESTDIR}${LIBDIR}/libform.a
LIBG2C?=	${DESTDIR}${LIBDIR}/libg2c.a
LIBGEOM?=	${DESTDIR}${LIBDIR}/libgeom.a
LIBGNUREGEX?=	${DESTDIR}${LIBDIR}/libgnuregex.a
LIBGPIO?=	${DESTDIR}${LIBDIR}/libgpio.a
LIBGSSAPI?=	${DESTDIR}${LIBDIR}/libgssapi.a 	# XXX

LIBIPSEC?=	${DESTDIR}${LIBDIR}/libipsec.a
LIBJAIL?=	${DESTDIR}${LIBDIR}/libjail.a

LIBKEYCAP?=	${DESTDIR}${LIBDIR}/libkeycap.a
LIBKICONV?=	${DESTDIR}${LIBDIR}/libkiconv.a

LIBKVM?=	${DESTDIR}${LIBDIR}/libkvm.a
LIBL?=		${DESTDIR}${LIBDIR}/libl.a
LIBLN?=		"don't use LIBLN, use LIBL"
LIBLZMA?=	${DESTDIR}${LIBDIR}/liblzma.a
LIBM?=		${DESTDIR}${LIBDIR}/libm.a
LIBMAGIC?=	${DESTDIR}${LIBDIR}/libmagic.a
LIBMD?=		${DESTDIR}${LIBDIR}/libmd.a
LIBMEMSTAT?=	${DESTDIR}${LIBDIR}/libmemstat.a
LIBMENU?=	${DESTDIR}${LIBDIR}/libmenu.a

LIBMP?=		${DESTDIR}${LIBDIR}/libmp.a
LIBMT?=		${DESTDIR}${LIBDIR}/libmt.a

LIBNANDFS?=	${DESTDIR}${LIBDIR}/libnandfs.a
LIBNCURSES?=	${DESTDIR}${LIBDIR}/libncurses.a
LIBNCURSESW?=	${DESTDIR}${LIBDIR}/libncursesw.a
LIBNETGRAPH?=	${DESTDIR}${LIBDIR}/libnetgraph.ae
LIBNV?=		${DESTDIR}${LIBDIR}/libnv.a

LIBOPENSM?=	${DESTDIR}${LIBDIR}/libopensm.a
LIBOPIE?=	${DESTDIR}${LIBDIR}/libopie.a

LIBPAM?=	${DESTDIR}${LIBDIR}/libpam.a
LIBPANEL?=	${DESTDIR}${LIBDIR}/libpanel.a
LIBPANELW?=	${DESTDIR}${LIBDIR}/libpanelw.a
LIBPCAP?=	${DESTDIR}${LIBDIR}/libpcap.a
LIBPJDLOG?=	${DESTDIR}${LIBDIR}/libpjdlog.a
LIBPMC?=	${DESTDIR}${LIBDIR}/libpmc.a
LIBPROC?=	${DESTDIR}${LIBDIR}/libproc.a
LIBPROCSTAT?=	${DESTDIR}${LIBDIR}/libprocstat.a
LIBPTHREAD?=	${DESTDIR}${LIBDIR}/libpthread.a

LIBRACOON?=	${DESTDIR}${LIBDIR}/libracoon.a

LIBRADIUS?=	${DESTDIR}${LIBDIR}/libradius.a

LIBRPCSEC_GSS?=	${DESTDIR}${LIBDIR}/librpcsec_gss.a
LIBRPCSVC?=	${DESTDIR}${LIBDIR}/librpcsvc.a
LIBRT?=		${DESTDIR}${LIBDIR}/librt.a
LIBRTLD_DB?=	${DESTDIR}${LIBDIR}/librtld_db.a
LIBSBUF?=	${DESTDIR}${LIBDIR}/libsbuf.a
LIBSSL?=	${DESTDIR}${LIBDIR}/libssl.a
LIBSSP_NONSHARED?=	${DESTDIR}${LIBDIR}/libssp_nonshared.a
LIBSTDCPLUSPLUS?= ${DESTDIR}${LIBDIR}/libstdc++.a
LIBSTDTHREADS?=	${DESTDIR}${LIBDIR}/libstdthreads.a
LIBSYSDECODE?=	${DESTDIR}${LIBDIR}/libsysdecode.a
LIBTACPLUS?=	${DESTDIR}${LIBDIR}/libtacplus.a
LIBTERMCAP?=	${DESTDIR}${LIBDIR}/libtermcap.a
LIBTERMCAPW?=	${DESTDIR}${LIBDIR}/libtermcapw.a
LIBTERMLIB?=	"don't use LIBTERMLIB, use LIBTERMCAP"
LIBTINFO?=	"don't use LIBTINFO, use LIBNCURSES"

LIBTLS?=	${DESTDIR}${LIBDIR}/libtls.a

LIBUFS?=	${DESTDIR}${LIBDIR}/libufs.a
LIBUGIDFW?=	${DESTDIR}${LIBDIR}/libugidfw.a
LIBULOG?=	${DESTDIR}${LIBDIR}/libulog.a

LIBUSB?=	${DESTDIR}${LIBDIR}/libusb.a
LIBUSBHID?=	${DESTDIR}${LIBDIR}/libusbhid.a
LIBUTIL?=	${DESTDIR}${LIBDIR}/libutil.a


LIBWRAP?=	${DESTDIR}${LIBDIR}/libwrap.a
LIBXO?=		${DESTDIR}${LIBDIR}/libxo.a
LIBXPG4?=	${DESTDIR}${LIBDIR}/libxpg4.a
LIBY?=		${DESTDIR}${LIBDIR}/liby.a
LIBZ?=		${DESTDIR}${LIBDIR}/libz.a



# enforce the 2 -lpthread and -lc to always be the last in that exact order
.if defined(LDADD)
.if ${LDADD:M-lpthread}
LDADD:=	${LDADD:N-lpthread} -lpthread
.endif
.if ${LDADD:M-lc}
LDADD:=	${LDADD:N-lc} -lc
.endif
.endif

# Only do this for src builds.
.if defined(SRCTOP)
.if defined(_LIBRARIES) && defined(LIB) && \
    ${_LIBRARIES:M${LIB}} != ""
.if !defined(LIB${LIB:tu})
.error ${.CURDIR}: Missing value for LIB${LIB:tu} in ${_this:T}.  Likely should be: LIB${LIB:tu}?= $${DESTDIR}$${LIBDIR}/lib${LIB}.a
.endif
.endif

# Derive LIB*SRCDIR from LIB*DIR
.for lib in ${_LIBRARIES}
LIB${lib:tu}SRCDIR?=	${SRCTOP}/${LIB${lib:tu}DIR:S,^${OBJTOP}/,,}
.endfor
.endif
