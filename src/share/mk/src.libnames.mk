# $FreeBSD: releng/11.0/share/mk/src.libnames.mk 301226 2016-06-02 19:06:04Z lidl $
#
# The include file <src.libnames.mk> define library names suitable
# for INTERNALLIB and PRIVATELIB definition

.if !target(__<bsd.init.mk>__)
.error src.libnames.mk cannot be included directly.
.endif

.if !target(__<src.libnames.mk>__)
__<src.libnames.mk>__:

.include <src.opts.mk>

_PRIVATELIBS=	\
		atf_c \
		atf_cxx \
		bsdstat \
		devdctl \
		event \
		ldns \
		sqlite3 \
		ssh \
		ucl \
		unbound

_INTERNALLIBS=	\
		amu \
		cron \
		elftc \
		fifolog \
		netbsd \
		opts \
		parse \
		pe \
		readline \
		sl \
		sm \
		smdb \
		smutil \
		vers

_LIBRARIES=	\
		${_PRIVATELIBS} \
		${_INTERNALLIBS} \
		${LOCAL_LIBRARIES} \
		80211 \
		alias \
		archive \
		avl \
		begemot \
		bsdxml \
		bz2 \
		c \
		c_pic \
		calendar \
		cam \
		casper \
		cap_dns \
		cap_grp \
		cap_pwd \
		cap_random \
		cap_sysctl \
		compiler_rt \
		crypt \
		crypto \
		ctf \
		cxxrt \
		devctl \
		devdctl \
		devinfo \
		devstat \
		dialog \
		dpv \
		dtrace \
		dwarf \
		edit \
		elf \
		execinfo \
		fetch \
		figpar \
		geom \
		gnuregex \
		gpio \
		gssapi \
		ipsec \
		jail \
		kiconv \
		kvm \
		l \
		lzma \
		m \
		magic \
		md \
		memstat \
		mp \
		mt \
		nandfs \
		ncurses \
		ncursesw \
		netgraph \
		nv \
		nvpair \
		opie \
		pam \
		panel \
		panelw \
		pcap \
		pcsclite \
		pjdlog \
		pmc \
		proc \
		procstat \
		pthread \
		racoon \
		radius \
		readline \
		roken \
		rpcsec_gss \
		rpcsvc \
		rt \
		rtld_db \
		sbuf \
		sdp \
		sm \
		ssl \
		ssp_nonshared \
		stdthreads \
		supcplusplus \
		sysdecode \
		tacplus \
		termcap \
		termcapw \
		tls \
		ufs \
		ugidfw \
		ulog \
		umem \
		usb \
		usbhid \
		util \
		uutil \
		vmmapi \
		wind \
		wrap \
		xo \
		y \
		z

.if ${MK_BLACKLIST} != "no"
_LIBRARIES+= \
		blacklist \

.endif

# Each library's LIBADD needs to be duplicated here for static linkage of
# 2nd+ order consumers.  Auto-generating this would be better.
_DP_80211=	sbuf bsdxml
_DP_archive=	z bz2 lzma bsdxml
.if ${MK_BLACKLIST} != "no"
_DP_blacklist+=	pthread
.endif
.if ${MK_LIBRESSL} != "no"
_DP_archive+=	crypto
.else
_DP_archive+=	md
.endif
_DP_sqlite3=	pthread
_DP_ssl=	crypto
_DP_ssh=	crypto crypt z
.if ${MK_LDNS} != "no"
_DP_ssh+=	ldns
.endif
_DP_edit=	ncursesw
.if ${MK_LIBRESSL} != "no"
_DP_racoon= 	crypto
_DP_tls= 	ssl crypto 
.endif
_DP_geom=	bsdxml sbuf
_DP_cam=	sbuf
_DP_kvm=	elf
_DP_casper=	nv
_DP_cap_dns=	nv
_DP_cap_grp=	nv
_DP_cap_pwd=	nv
_DP_cap_random=	nv
_DP_cap_sysctl=	nv
_DP_pjdlog=	util
_DP_opie=	md
_DP_usb=	pthread
_DP_unbound=	ssl crypto pthread
_DP_rt=	pthread
.if ${MK_LIBRESSL} == "no"
_DP_radius=	md
.else
_DP_radius=	crypto
.endif
_DP_procstat=	kvm util elf
.if ${MK_CXX} == "yes"
.if ${MK_LIBCPLUSPLUS} != "no"
_DP_proc=	cxxrt
.else
_DP_proc=	supcplusplus
.endif
.endif
_DP_proc+=	elf rtld_db util
_DP_mp=	crypto
_DP_memstat=	kvm
_DP_magic=	z
_DP_mt=		sbuf bsdxml
_DP_ldns=	crypto
.if ${MK_LIBRESSL} != "no"
_DP_fetch=	ssl crypto
.else
_DP_fetch=	md
.endif
_DP_execinfo=	elf
_DP_dwarf=	elf
_DP_dpv=	dialog figpar util ncursesw
_DP_dialog=	ncursesw m
_DP_atf_cxx=	atf_c
_DP_devstat=	kvm
_DP_pam=	radius tacplus opie md util
.if ${MK_OPENSSH} != "no"
_DP_pam+=	ssh
.endif
_DP_readline=	ncursesw
_DP_lzma=	pthread
_DP_ucl=	m
_DP_vmmapi=	util
_DP_xo=		util
# The libc dependencies are not strictly needed but are defined to make the
# assert happy.
_DP_c=		compiler_rt
.if ${MK_SSP} != "no"
_DP_c+=		ssp_nonshared
.endif
_DP_stdthreads=	pthread
_DP_tacplus=	md
_DP_panel=	ncurses
_DP_panelw=	ncursesw
_DP_rpcsec_gss=	gssapi
_DP_ulog=	md
_DP_fifolog=	z

# Define special cases
LDADD_supcplusplus=	-lsupc++
LIBATF_C=	${DESTDIR}${LIBDIR}/libprivateatf-c.a
LIBATF_CXX=	${DESTDIR}${LIBDIR}/libprivateatf-c++.a
LDADD_atf_c=	-lprivateatf-c
LDADD_atf_cxx=	-lprivateatf-c++

.for _l in ${_PRIVATELIBS}
LIB${_l:tu}?=	${DESTDIR}${LIBDIR}/libprivate${_l}.a
.endfor

.for _l in ${_LIBRARIES}
.if ${_INTERNALLIBS:M${_l}}
LDADD_${_l}_L+=		-L${LIB${_l:tu}DIR}
.endif
DPADD_${_l}?=	${LIB${_l:tu}}
.if ${_PRIVATELIBS:M${_l}}
LDADD_${_l}?=	-lprivate${_l}
.else
LDADD_${_l}?=	${LDADD_${_l}_L} -l${_l}
.endif
# Add in all dependencies for static linkage.
.if defined(_DP_${_l}) && (${_INTERNALLIBS:M${_l}} || \
    (defined(NO_SHARED) && (${NO_SHARED} != "no" && ${NO_SHARED} != "NO")))
.for _d in ${_DP_${_l}}
DPADD_${_l}+=	${DPADD_${_d}}
LDADD_${_l}+=	${LDADD_${_d}}
.endfor
.endif
.endfor

# These are special cases where the library is broken and anything that uses
# it needs to add more dependencies.  Broken usually means that it has a
# cyclic dependency and cannot link its own dependencies.  This is bad, please
# fix the library instead.
# Unless the library itself is broken then the proper place to define
# dependencies is _DP_* above.

# libatf-c++ exposes libatf-c abi hence we need to explicit link to atf_c for
# atf_cxx
DPADD_atf_cxx+=	${DPADD_atf_c}
LDADD_atf_cxx+=	${LDADD_atf_c}

# Detect LDADD/DPADD that should be LIBADD, before modifying LDADD here.
_BADLDADD=
.for _l in ${LDADD:M-l*:N-l*/*:C,^-l,,}
.if ${_LIBRARIES:M${_l}} && !${_PRIVATELIBS:M${_l}}
_BADLDADD+=	${_l}
.endif
.endfor
.if !empty(_BADLDADD)
.error ${.CURDIR}: These libraries should be LIBADD+=foo rather than DPADD/LDADD+=-lfoo: ${_BADLDADD}
.endif

.for _l in ${LIBADD}
DPADD+=		${DPADD_${_l}}
LDADD+=		${LDADD_${_l}}
.endfor

# INTERNALLIB definitions.
LIBELFTCDIR=	${OBJTOP}/lib/libelftc
LIBELFTC?=	${LIBELFTCDIR}/libelftc.a

LIBPEDIR=	${OBJTOP}/lib/libpe
LIBPE?=		${LIBPEDIR}/libpe.a

LIBREADLINEDIR=	${OBJTOP}/gnu/lib/libreadline/readline
LIBREADLINE?=	${LIBREADLINEDIR}/libreadline.a

LIBCRONDIR=	${OBJTOP}/usr.sbin/cron/lib
LIBCRON?=	${LIBCRONDIR}/libcron.a

LIBLPRDIR=	${OBJTOP}/usr.sbin/lpr/common_source
LIBLPR?=	${LIBOPTSDIR}/liblpr.a

LIBFIFOLOGDIR=	${OBJTOP}/usr.sbin/fifolog/lib
LIBFIFOLOG?=	${LIBOPTSDIR}/libfifolog.a

# Define a directory for each library.  This is useful for adding -L in when
# not using a --sysroot or for meta mode bootstrapping when there is no
# Makefile.depend.  These are sorted by directory.
LIBDIALOGDIR=	${OBJTOP}/gnu/lib/libdialog
LIBGCOVDIR=	${OBJTOP}/gnu/lib/libgcov
LIBGOMPDIR=	${OBJTOP}/gnu/lib/libgomp
LIBGNUREGEXDIR=	${OBJTOP}/gnu/lib/libregex
LIBSSPDIR=	${OBJTOP}/gnu/lib/libssp
LIBSSP_NONSHAREDDIR=	${OBJTOP}/gnu/lib/libssp/libssp_nonshared
LIBSUPCPLUSPLUSDIR=	${OBJTOP}/gnu/lib/libsupc++

LIBATF_CDIR=	${OBJTOP}/lib/atf/libatf-c
LIBATF_CXXDIR=	${OBJTOP}/lib/atf/libatf-c++
LIBALIASDIR=	${OBJTOP}/lib/libalias/libalias
LIBBLACKLISTDIR=	${OBJTOP}/lib/libblacklist
LIBBLOCKSRUNTIMEDIR=	${OBJTOP}/lib/libblocksruntime
LIBCASPERDIR=	${OBJTOP}/lib/libcasper/libcasper
LIBCAP_DNSDIR=	${OBJTOP}/lib/libcasper/services/cap_dns
LIBCAP_GRPDIR=	${OBJTOP}/lib/libcasper/services/cap_grp
LIBCAP_PWDDIR=	${OBJTOP}/lib/libcasper/services/cap_pwd
LIBCAP_RANDOMDIR=	${OBJTOP}/lib/libcasper/services/cap_random
LIBCAP_SYSCTLDIR=	${OBJTOP}/lib/libcasper/services/cap_sysctl
LIBBSDXMLDIR=	${OBJTOP}/lib/libexpat
LIBKVMDIR=	${OBJTOP}/lib/libkvm
LIBPTHREADDIR=	${OBJTOP}/lib/libthr
LIBMDIR=	${OBJTOP}/lib/msun
LIBFORMDIR=	${OBJTOP}/lib/ncurses/form
LIBFORMLIBWDIR=	${OBJTOP}/lib/ncurses/formw
LIBMENUDIR=	${OBJTOP}/lib/ncurses/menu
LIBMENULIBWDIR=	${OBJTOP}/lib/ncurses/menuw
LIBNCURSESDIR=	${OBJTOP}/lib/ncurses/ncurses
LIBNCURSESWDIR=	${OBJTOP}/lib/ncurses/ncursesw
LIBPANELDIR=	${OBJTOP}/lib/ncurses/panel
LIBPANELWDIR=	${OBJTOP}/lib/ncurses/panelw
LIBCRYPTODIR=	${OBJTOP}/lib/libcrypto
LIBSSHDIR=	${OBJTOP}/lib/libssh
LIBSSLDIR=	${OBJTOP}/lib/libssl
LIBTEKENDIR=	${OBJTOP}/sys/teken/libteken
LIBEGACYDIR=	${OBJTOP}/tools/build
LIBLNDIR=	${OBJTOP}/usr.bin/lex/lib

LIBRACOONDIR= 	${OBJTOP}/lib/libracoon

LIBTLSDIR=	${OBJTOP}/lib/libtls
LIBTLS?=	${LIBTELNETDIR}/libtls.a

LIBTERMCAPDIR=	${LIBNCURSESDIR}
LIBTERMCAPWDIR=	${LIBNCURSESWDIR}

# Default other library directories to lib/libNAME.
.for lib in ${_LIBRARIES}
LIB${lib:tu}DIR?=	${OBJTOP}/lib/lib${lib}
.endfor

# Validate that listed LIBADD are valid.
.for _l in ${LIBADD}
.if empty(_LIBRARIES:M${_l})
_BADLIBADD+= ${_l}
.endif
.endfor
.if !empty(_BADLIBADD)
.error ${.CURDIR}: Invalid LIBADD used which may need to be added to ${_this:T}: ${_BADLIBADD}
.endif

# Sanity check that libraries are defined here properly when building them.
.if defined(LIB) && ${_LIBRARIES:M${LIB}} != ""
.if !empty(LIBADD) && \
    (!defined(_DP_${LIB}) || ${LIBADD:O:u} != ${_DP_${LIB}:O:u})
.error ${.CURDIR}: Missing or incorrect _DP_${LIB} entry in ${_this:T}.  Should match LIBADD for ${LIB} ('${LIBADD}' vs '${_DP_${LIB}}')
.endif
# Note that OBJTOP is not yet defined here but for the purpose of the check
# it is fine as it resolves to the SRC directory.
.if !defined(LIB${LIB:tu}DIR) || !exists(${SRCTOP}/${LIB${LIB:tu}DIR:S,^${OBJTOP}/,,})
.error ${.CURDIR}: Missing or incorrect value for LIB${LIB:tu}DIR in ${_this:T}: ${LIB${LIB:tu}DIR:S,^${OBJTOP}/,,}
.endif
.if ${_INTERNALLIBS:M${LIB}} != "" && !defined(LIB${LIB:tu})
.error ${.CURDIR}: Missing value for LIB${LIB:tu} in ${_this:T}.  Likely should be: LIB${LIB:tu}?= $${LIB${LIB:tu}DIR}/lib${LIB}.a
.endif
.endif

.endif	# !target(__<src.libnames.mk>__)
