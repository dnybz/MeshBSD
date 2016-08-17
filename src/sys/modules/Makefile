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
	${_3dfx} \
	${_3dfx_linux} \
	${_aac} \
	${_aacraid} \
	accf_data \
	accf_dns \
	accf_http \
	acl_nfs4 \
	acl_posix1e \
	${_acpi} \
	ae \
	${_aesni} \
	age \
	${_agp} \
	aha \
	${_ahb} \
	ahci \
	${_aic} \
	aic7xxx \
	alc \
	ale \
	alq \
	${_amdsbwd} \
	${_amdtemp} \
	amr \
	${_an} \
	${_aout} \
	${_apm} \
	${_arcmsr} \
	${_arcnet} \
	${_asmc} \
	ata \
	ath \
	ath_pci \
	${_autofs} \
	${_auxio} \
	${_bce} \
	bfe \
	bhnd \
	bge \
	bhnd \
	${_bxe} \
	${_bios} \
	${_bktr} \
	${_bm} \
	bridgestp \
	bwi \
	bwn \
	bwn_pci \
	cam \
	${_canbepm} \
	${_canbus} \
	${_cardbus} \
	${_carp} \
	cas \
	${_cbb} \
	cc \
	${_ce} \
	${_cfi} \
	${_ciss} \
	cloudabi \
	${_cloudabi64} \
	${_cm} \
	${_cmx} \
	${_coff} \
	${_coretemp} \
	${_cp} \
	${_cpsw} \
	${_cpuctl} \
	${_cpufreq} \
	${_crypto} \
	${_cryptodev} \
	${_cs} \
	${_ct} \
	${_ctau} \
	ctl \
	dc \
	dcons \
	dcons_crom \
	de \
	${_dpms} \
	${_dpt} \
	${_drm} \
	${_drm2} \
	dummynet \
	${_ed} \
	${_elink} \
	${_em} \
	en \
	${_ep} \
	${_epic} \
	esp \
	${_et} \
	${_ex} \
	${_exca} \
	ext2fs \
	${_fatm} \
	fdc \
	fdescfs \
	${_fe} \
	${_filemon} \
	firmware \
	fuse \
	${_fxp} \
	gem \
	geom \
	${_glxiic} \
	${_glxsb} \
	hatm \
	hifn \
	hme \
	${_hpt27xx} \
	${_hptiop} \
	${_hptmv} \
	${_hptnr} \
	${_hptrr} \
	hwpmc \
	${_hyperv} \
	i2c \
        ${_ibcore} \
	${_ibcs2} \
	${_ichwd} \
	${_ida} \
	${_ie} \
	if_bridge \
	if_disc \
	if_edsc \
	${_if_enc} \
	if_epair \
	${_if_gif} \
	${_if_gre} \
	${_if_me} \
	if_lagg \
	${_if_ndis} \
	${_if_stf} \
	if_tap \
	if_tun \
	if_vether \
	if_vlan \
	if_vxlan \
	${_igb} \
	${_iir} \
	imgact_binmisc \
	${_io} \
	${_ioat} \
        ${_ipoib} \
	${_ipdivert} \
	${_ipfilter} \
	${_ipfw} \
	ipfw_nat \
	${_ipmi} \
	ip6_mroute_mod \
	ip_mroute_mod \
	${_ips} \
	${_ipw} \
	${_ipwfw} \
	${_isci} \
	${_iwi} \
	${_iwifw} \
	${_iwm} \
	${_iwmfw} \
	${_iwn} \
	${_iwnfw} \
	${_ix} \
	${_ixv} \
	${_ixgb} \
	${_ixl} \
	${_ixlv} \
	jme \
	joy \
	kbdmux \
	kgssapi \
	kgssapi_krb5 \
	khelp \
	krpc \
	ksyms \
	le \
	lge \
	libalias \
	libiconv \
	libmbpool \
	libmchain \
	lmc \
	lpt \
	mac_biba \
	mac_bsdextended \
	mac_ifoff \
	mac_lomac \
	mac_mls \
	mac_none \
	mac_partition \
	mac_portacl \
	mac_seeotheruids \
	mac_stub \
	mac_test \
	malo \
	mcd \
	md \
	mdio \
	mem \
	mfi \
	mii \
	mlx \
	${_mlx4} \
	${_mlx4ib} \
	${_mlxen} \
	${_mlx5} \
	${_mlx5en} \
	${_mly} \
	mmc \
	mmcsd \
	mpr \
	mps \
	mpt \
	mqueue \
	mrsas \
	msdosfs \
	msdosfs_iconv \
	${_mse} \
	msk \
	${_mthca} \
	mvs \
	mwl \
	${_mwlfw} \
	mxge \
	my \
	${_nandfs} \
	${_nandsim} \
	${_ncr} \
	${_nctgpio} \
	${_ncv} \
	${_ndis} \
	netfpga10g \
	${_netgraph} \
	${_nfe} \
	nfscl \
	nfscommon \
	nfsd \
	nfslock \
	nfslockd \
	nfssvc \
	nge \
	nmdm \
	${_nsp} \
	nullfs \
	${_ntb} \
	${_nvd} \
	${_nvme} \
	${_nvram} \
	${_nxge} \
	oce \
	otus \
	otusfw \
	ow \
	${_padlock} \
	${_padlock_rng} \
	patm \
	${_pccard} \
	${_pcfclock} \
	pcn \
	${_pf} \
	${_pflog} \
	${_pfsync} \
	plip \
	${_pmc} \
	${_pms} \
	ppbus \
	ppc \
	ppi \
	pps \
	procfs \
	proto \
	pseudofs \
	${_pst} \
	pty  \
	puc \
	${_qlxge} \
	${_qlxgb} \
	${_qlxgbe} \
	ral \
	${_ralfw} \
	${_random_fortuna} \
	${_random_yarrow} \
	${_random_other} \
	rc4 \
	${_rdma} \
	${_rdrand_rng} \
	re \
	rl \
	rtwn \
	${_rtwnfw} \
	${_s3} \
	${_safe} \
	${_sbni} \
	scc \
	scd \
	${_scsi_low} \
	sdhci \
	sdhci_pci \
	sem \
	send \
	${_sf} \
	${_sfxge} \
	sge \
	${_si} \
	siba_bwn \
	siftr \
	siis \
	sis \
	sk \
	smbfs \
	sn \
	${_snc} \
	snp \
	sound \
	${_speaker} \
	${_splash} \
	${_sppp} \
	ste \
	${_stg} \
	stge \
	${_streams} \
	${_svr4} \
	${_sym} \
	${_syscons} \
	sysvipc \
	${_ti} \
	tcp/fastpath \
	tests/framework \
	tests/callout_test \
	tl \
	tmpfs \
	${_toecore} \
	${_tpm} \
	trm \
	${_twa} \
	twe \
	tws \
	tx \
	${_txp} \
	uart \
	ubsec \
	udf \
	udf_iconv \
	ufs \
	unionfs \
	urtwn \
	${_urtwnfw} \
	usb \
	utopia \
	${_vesa} \
	${_virtio} \
	vge \
	${_viawd} \
	videomode \
	vkbd \
	${_vmm} \
	${_vmware} \
	${_vpo} \
	vr \
	vte \
	vx \
	${_vxge} \
	wb \
	${_wbwd} \
	${_wds} \
	${_wi} \
	${_wl} \
	wlan \
	wlan_acl \
	wlan_amrr \
	wlan_ccmp \
	wlan_rssadapt \
	wlan_tkip \
	wlan_wep \
	wlan_xauth \
	${_wpi} \
	${_wpifw} \
	${_x86bios} \
	${_xe} \
	xl \
	zlib

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

.if ${MACHINE_CPUARCH} != "mips"
# no BUS_SPACE_UNSPECIFIED
# No barrier instruction support (specific to this driver)
_sym=		sym
# intr_disable() is a macro, causes problems
.endif

.if ${MACHINE_CPUARCH} == "aarch64"
_em=		em
_igb=		igb
.endif


.if ${MACHINE_CPUARCH} == "arm"
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
