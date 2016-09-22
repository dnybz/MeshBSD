# $FreeBSD: head/sys/modules/Makefile 300062 2016-05-17 15:36:40Z gnn $

SYSDIR?=${.CURDIR}/..
.include "${SYSDIR}/conf/kern.opts.mk"

SUBDIR_PARALLEL=

# Modules that include binary-only blobs of microcode should be selectable by
# MK_SOURCELESS_UCODE option (see below).

.if defined(MODULES_OVERRIDE) && !defined(ALL_MODULES)
SUBDIR=${MODULES_OVERRIDE}
.else
SUBDIR=	\
SUBDIR+= 	... \ # XXX: ...
	

.if ${MK_AUTOFS} != "no" || defined(ALL_MODULES)
_autofs=	autofs
.endif

.if ${MK_CRYPT} != "no" || defined(ALL_MODULES)
.if exists(${.CURDIR}/../opencrypto)
_crypto=	crypto
_cryptodev=	cryptodev
_random_fortuna=random_fortuna
_random_yarrow=	random_yarrow
_random_other=	random_other
.endif
.endif

.if ${MK_CUSE} != "no" || defined(ALL_MODULES)
SUBDIR+=	cuse
.endif

.if (${MK_INET_SUPPORT} != "no" || ${MK_INET6_SUPPORT} != "no") || \
	defined(ALL_MODULES)
_carp=		carp
_toecore=	toecore
_if_enc=	if_enc
_if_gif=	if_gif
_if_gre=	if_gre
.endif

.if (${MK_INET_SUPPORT} != "no" && ${MK_INET6_SUPPORT} != "no") || \
	defined(ALL_MODULES)
_if_stf=	if_stf
.endif

.if ${MK_INET_SUPPORT} != "no" || defined(ALL_MODULES)
_if_me=		if_me
_ipdivert=	ipdivert
_ipfw=		ipfw
.endif

.if ${MK_ISCSI} != "no" || defined(ALL_MODULES)
SUBDIR+=	iscsi
SUBDIR+=	iscsi_initiator
.endif

.if ${MK_NAND} != "no" || defined(ALL_MODULES)
_nandfs=	nandfs
_nandsim=	nandsim
.endif

.if ${MK_NETGRAPH} != "no" || defined(ALL_MODULES)
_netgraph=	netgraph
.endif

.if (${MK_PF} != "no" && (${MK_INET_SUPPORT} != "no" || \
	${MK_INET6_SUPPORT} != "no")) || defined(ALL_MODULES)
_pf=		pf
_pflog=		pflog
.if ${MK_INET_SUPPORT} != "no"
_pfsync=	pfsync
.endif
.endif

.if ${MK_SOURCELESS_UCODE} != "no"
_bce=		bce
_fatm=		fatm
_fxp=		fxp
_ispfw=		ispfw
_mwlfw=		mwlfw
_ralfw=		ralfw
_rtwnfw=	rtwnfw
_urtwnfw=	urtwnfw
_sf=		sf
_ti=		ti
_txp=		txp
.endif

.if ${TARGET_ARCH} != "mips"
# no BUS_SPACE_UNSPECIFIED
# No barrier instruction support (specific to this driver)
_sym=		sym
# intr_disable() is a macro, causes problems
.endif

.if ${TARGET_ARCH} == "aarch64"
_em=		em
_igb=		igb
.endif


.if ${TARGET_ARCH} == "arm"
_cfi=		cfi
_cpsw=		cpsw
.endif

SUBDIR+=${MODULES_EXTRA}

.for reject in ${WITHOUT_MODULES}
SUBDIR:= ${SUBDIR:N${reject}}
.endfor

# Calling kldxref(8) for each module is expensive.
.if !defined(NO_XREF)
.MAKEFLAGS+=	-DNO_XREF
afterinstall:
	@if type kldxref >/dev/null 2>&1; then \
		${ECHO} kldxref ${DESTDIR}${KMODDIR}; \
		kldxref ${DESTDIR}${KMODDIR}; \
	fi
.endif

.include "${SYSDIR}/conf/config.mk"

SUBDIR:= ${SUBDIR:u:O}

.include <bsd.subdir.mk>
