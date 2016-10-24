# $FreeBSD: head/sys/Makefile 296192 2016-02-29 16:39:27Z andrew $

# Directories to include in cscope name file and TAGS.
CSCOPEDIRS= 	boot 
CSCOPEDIRS+= 	bsm 
CSCOPEDIRS+= 	cam
CSCOPEDIRS+= 	cddl
CSCOPEDIRS+= 	conf
CSCOPEDIRS+= 	crypto
CSCOPEDIRS+= 	ddb
CSCOPEDIRS+= 	dev
CSCOPEDIRS+= 	fs
CSCOPEDIRS+= 	freebsd32
CSCOPEDIRS+= 	gdb 
CSCOPEDIRS+= 	geom
CSCOPEDIRS+= 	gnu
CSCOPEDIRS+= 	isa
CSCOPEDIRS+= 	kern
CSCOPEDIRS+= 	libfdt
CSCOPEDIRS+= 	libkern
CSCOPEDIRS+= 	libnv
CSCOPEDIRS+= 	modules
CSCOPEDIRS+= 	net
CSCOPEDIRS+= 	net80211
CSCOPEDIRS+= 	netgraph
CSCOPEDIRS+= 	netinet
CSCOPEDIRS+= 	netinet6
CSCOPEDIRS+= 	netipsec
CSCOPEDIRS+= 	netpfil
CSCOPEDIRS+= 	nfs
CSCOPEDIRS+= 	nfsclient
CSCOPEDIRS+= 	nfsserver
CSCOPEDIRS+= 	nlm
CSCOPEDIRS+= 	opencrypto
CSCOPEDIRS+= 	rpc
CSCOPEDIRS+= 	security
CSCOPEDIRS+= 	sys
CSCOPEDIRS+= 	ufs
CSCOPEDIRS+= 	vm
CSCOPEDIRS+= 	xdr
CSCOPEDIRS+= 	${CSCOPE_ARCHDIR}
.if !defined(CSCOPE_ARCHDIR)
.if defined(ALL_ARCH)
CSCOPE_ARCHDIR= 	arm
CSCOPE_ARCHDIR+= 	mips
CSCOPE_ARCHDIR+= 	riscv
.else
CSCOPE_ARCHDIR = ${MACHINE}
.if ${MACHINE} != ${MACHINE_CPUARCH}
CSCOPE_ARCHDIR += ${MACHINE_CPUARCH}
.endif
.endif
.endif

HTAGSFLAGS+= -at `awk -F= '/^RELEASE *=/{release=$2}; END {print "FreeBSD", release, "kernel"}' < conf/newvers.sh`

# You need the devel/cscope port for this.
cscope: cscope.out
cscope.out: ${.CURDIR}/cscope.files
	cd ${.CURDIR}; cscope -k -buq -p4 -v

${.CURDIR}/cscope.files: .PHONY
	cd ${.CURDIR}; \
		find ${CSCOPEDIRS} -name "*.[chSsly]" -a -type f > ${.TARGET}

cscope-clean:
	rm -f cscope.files cscope.out cscope.in.out cscope.po.out

#
# Installs SCM hooks to update the cscope database every time the source tree
# is updated.
# cscope understands incremental updates, so it's considerably faster when only
# a few files have changed.
#
HG_DIR=${.CURDIR}/../.hg
HG_HOOK=if [ \$$HG_ERROR -eq 0 ]; then cd sys && make -m ../share/mk cscope; fi
cscope-hook:
	@if [ -d ${HG_DIR} ]; then 					\
		if [ "`grep hooks ${HG_DIR}/hgrc`" = "" ]; then		\
			echo "[hooks]" >> ${HG_DIR}/hgrc;		\
			echo "update = ${HG_HOOK}" >> ${HG_DIR}/hgrc;	\
			echo "Hook installed in ${HG_DIR}/hgrc";	\
		else 							\
			echo "Mercurial update hook already exists.";	\
		fi;							\
	fi

# You need the devel/global and one of editor/emacs* ports for that.
TAGS ${.CURDIR}/TAGS: ${.CURDIR}/cscope.files
	rm -f ${.CURDIR}/TAGS
	cd ${.CURDIR}; xargs etags -a < ${.CURDIR}/cscope.files

# You need the textproc/glimpse ports for this.
glimpse:
.if !exists(${.CURDIR}/.glimpse_exclude)
	echo .svn > ${.CURDIR}/.glimpse_exclude
	echo /compile/ >> ${.CURDIR}/.glimpse_exclude
.endif
	cd ${.CURDIR}; glimpseindex -H . -B -f -o .

glimpse-clean:
	cd ${.CURDIR}; rm -f .glimpse_*

.if !(make(cscope) || make(cscope-clean) || make(cscope-hook) || make(TAGS) || \
    make(glimpse) || make(glimpse-clean))
.include <src.opts.mk>

# The boot loader
.if ${MK_BOOT} != "no"
SUBDIR=	boot
.endif

# Loadable kernel modules

.if defined(MODULES_WITH_WORLD)
SUBDIR+=modules
.endif

.include <bsd.subdir.mk>
.endif
