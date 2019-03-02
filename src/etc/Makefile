#	from: @(#)Makefile	5.11 (Berkeley) 5/21/91
# $FreeBSD: head/etc/Makefile 298140 2016-04-17 03:45:45Z gjb $

.include <src.opts.mk>

FILESGROUPS=	FILES

# No need as it is empty and just causes rebuilds since this file does so much.
UPDATE_DEPENDFILE=	no
SUBDIR=	

.if exists(${.CURDIR}/etc.${TARGET}/rc)
BIN1=	etc.${TARGET}/rc
.elif exists(${.CURDIR}/etc.${TARGET_ARCH}/rc)
BIN1=	etc.${TARGET_ARCH}/rc
.else
.error etc.TARGET/rc missing
.endif

BIN1+= crontab
BIN1+= ddb.conf
BIN1+= devd.conf
BIN1+= devfs.conf
BIN1+= dhclient.conf
BIN1+= dhcpd.conf
BIN1+= fbtab
BIN1+= gettytab
BIN1+= group
BIN1+= hosts
BIN1+= hosts.allow
BIN1+= hosts.equiv
BIN1+= ipsec.conf
BIN1+= libalias.conf
BIN1+= login.access
BIN1+= login.conf
BIN1+= mac.conf
BIN1+= master.passwd
BIN1+= myname
BIN1+= netconfig
BIN1+= networks
BIN1+= newsyslog.conf
BIN1+= nsswitch.conf
BIN1+= passwd
BIN1+= pf.conf
BIN1+= phones
BIN1+= profile
BIN1+= protocols
BIN1+= rc.subr
BIN1+= rc2
BIN1+= remote
BIN1+= rpc
BIN1+= services
BIN1+= shells
BIN1+= sysctl.conf
BIN1+= syslog.conf

.if exists(${.CURDIR}/etc.${TARGET}/rc.conf)
BIN1+=	etc.${TARGET}/rc.conf
.elif exists(${.CURDIR}/etc.${TARGET_ARCH}/rc.conf)
BIN1+=	etc.${TARGET}/rc.conf
.else
.error etc.TARGET/rc.conf missing
.endif

.if exists(${.CURDIR}/etc.${TARGET}/ttys)
BIN1+=	etc.${TARGET}/ttys
.elif exists(${.CURDIR}/etc.${TARGET_ARCH}/ttys)
BIN1+=	etc.${TARGET_ARCH}/ttys
.else
.error etc.TARGET/ttys missing
.endif

OPENBSMDIR=			${.CURDIR}/../external/openbsm
BSM_ETC_OPEN_FILES=		${OPENBSMDIR}/etc/audit_class \
				${OPENBSMDIR}/etc/audit_event
BSM_ETC_RESTRICTED_FILES=	${OPENBSMDIR}/etc/audit_control \
				${OPENBSMDIR}/etc/audit_user
BSM_ETC_EXEC_FILES=		${OPENBSMDIR}/etc/audit_warn
BSM_ETC_DIR=			${DESTDIR}/etc/security

.if ${MK_FTP} != "no"
BIN1+= ftpusers
.endif

.if ${MK_INETD} != "no"
BIN1+= inetd.conf
.endif

BIN1+=	ntpd.conf

.if ${MK_OPENSSH} != "no"
SSH=	${.CURDIR}/../external/openssh/ssh_config \
	${.CURDIR}/../external/openssh/sshd_config \
	${.CURDIR}/../external/openssh/moduli
.endif
.if ${MK_LIBRESSL} != "no"
SSL=	${.CURDIR}/../external/libressl/apps/openssl/openssl.cnf
.endif

.if ${MK_PF} != "no"
BIN1+= pf.os
.endif

.if ${MK_WIRELESS} != "no"
BIN1+= hostapd.conf
BIN1+= regdomain.xml
BIN1+= wpa_supplicant.conf
.endif

# -rwxr-xr-x root:wheel, for the new cron root:wheel
BIN2=	

MTREE= 	BSD.include.dist
MTREE+= 	BSD.root.dist 
MTREE+= 	BSD.usr.dist
MTREE+= 	BSD.var.dist
.if ${MK_LIB32} != "no"
MTREE+=	BSD.lib32.dist
.endif
.if ${MK_LIBSOFT} != "no"
MTREE+=	BSD.libsoft.dist
.endif

PPPCNF=	

# Special top level files for FreeBSD
FREEBSD=COPYRIGHT

# Sanitize DESTDIR
DESTDIR:=	${DESTDIR:C://*:/:g}

afterinstall:

distribute:
	# Avoid installing tests here; "make distribution" will do this and
	# correctly place them in the right location.
	${_+_}cd ${.CURDIR} ; ${MAKE} MK_TESTS=no install \
	    DESTDIR=${DISTDIR}/${DISTRIBUTION}
	${_+_}cd ${.CURDIR} ; ${MAKE} distribution DESTDIR=${DISTDIR}/${DISTRIBUTION}

.include <bsd.endian.mk>
.if ${TARGET_ENDIANNESS} == "1234"
CAP_MKDB_ENDIAN?= -l
.elif ${TARGET_ENDIANNESS} == "4321"
CAP_MKDB_ENDIAN?= -b
.else
CAP_MKDB_ENDIAN?=
.endif

.if defined(NO_ROOT)
METALOG.add?=	cat -l >> ${METALOG}
.endif

distribution:
.if !defined(DESTDIR)
	@echo "set DESTDIR before running \"make ${.MACHINE}\""
	@false
.endif
	cd ${.CURDIR}; \
	    ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 644 \
		${BIN1} ${DESTDIR}/etc; \
	    ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 755 \
		${BIN2} ${DESTDIR}/etc; \
	    ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 600 \
		master.passwd nsmb.conf opieaccess ${DESTDIR}/etc;
.if ${MK_AT} == "no"
	sed -i "" -e 's;.*/usr/libexec/atrun;#&;' ${DESTDIR}/etc/crontab
.endif
	pwd_mkdb -i -p -d ${DESTDIR}/etc ${DESTDIR}/etc/master.passwd
.if defined(NO_ROOT)
	( \
		echo "./etc/login.conf.db type=file mode=0644 uname=root gname=wheel"; \
		echo "./etc/passwd type=file mode=0644 uname=root gname=wheel"; \
		echo "./etc/pwd.db type=file mode=0644 uname=root gname=wheel"; \
		echo "./etc/spwd.db type=file mode=0600 uname=root gname=wheel"; \
	) | ${METALOG.add}
.endif
.if ${MK_CASPER} != "no"
	${_+_}cd ${.CURDIR}/casper; ${MAKE} install
.endif
	${_+_}cd ${.CURDIR}/defaults; ${MAKE} install
	${_+_}cd ${.CURDIR}/devd; ${MAKE} install
	${_+_}cd ${.CURDIR}/gss; ${MAKE} install
	${_+_}cd ${.CURDIR}/periodic; ${MAKE} install
	${_+_}cd ${.CURDIR}/rc.d; ${MAKE} install
	${_+_}cd ${.CURDIR}/../share/termcap; ${MAKE} etc-termcap
	${_+_}cd ${.CURDIR}/../usr.sbin/rmt; ${MAKE} etc-rmt
	${_+_}cd ${.CURDIR}/pam.d; ${MAKE} install
	cd ${.CURDIR}; ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 0444 \
	    ${BSM_ETC_OPEN_FILES} ${BSM_ETC_DIR}
	cd ${.CURDIR}; ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 0600 \
	    ${BSM_ETC_RESTRICTED_FILES} ${BSM_ETC_DIR}
	cd ${.CURDIR}; ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 0500 \
	    ${BSM_ETC_EXEC_FILES} ${BSM_ETC_DIR}
.if ${MK_OPENSSH} != "no"
	cd ${.CURDIR}; ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 644 \
	    ${SSH} ${DESTDIR}/etc/ssh
.endif
.if ${MK_LIBRESSL} != "no"
	cd ${.CURDIR}; ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 644 \
	    ${SSL} ${DESTDIR}/etc/ssl
.endif
	cd ${.CURDIR}/root; \
	    ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 644 \
		dot.profile ${DESTDIR}/root/.profile; \
	    rm -f ${DESTDIR}/.profile; \
	    ln ${DESTDIR}/root/.profile ${DESTDIR}/.profile
	cd ${.CURDIR}/mtree; ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 444 \
	    ${MTREE} ${DESTDIR}/etc/mtree
	${INSTALL} -o ${BINOWN} -g operator -m 664 /dev/null \
	    ${DESTDIR}/etc/dumpdates
.if ${MK_LOCATE} != "no"
	${INSTALL} -o nobody -g ${BINGRP} -m 644 /dev/null \
	    ${DESTDIR}/var/db/locate.database
.endif
	${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 644 ${.CURDIR}/minfree \
	    ${DESTDIR}/var/crash
	cd ${.CURDIR}/..; ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 444 \
		${FREEBSD} ${DESTDIR}/
.if ${MK_BOOT} != "no"
.if exists(${.CURDIR}/../sys/${MACHINE}/conf/GENERIC.hints)
	${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 444 \
	    ${.CURDIR}/../sys/${MACHINE}/conf/GENERIC.hints \
	    ${DESTDIR}/boot/device.hints
.endif
.endif

MTREE_CMD?=	mtree

.if ${MK_INSTALL_AS_USER} == "yes" && ${_uid} != 0
MTREE_FILTER= sed -e 's,\([gu]\)name=,\1id=,g' \
	-e 's,\(uid=\)[^ ]* ,\1${_uid} ,' \
	-e 's,\(gid=\)[^ ]* ,\1${_gid} ,' \
	-e 's,\(uid=\)[^ ]*$$,\1${_uid},' \
	-e 's,\(gid=\)[^ ]*$$,\1${_gid},' 
.else
MTREE_FILTER= cat
.if !defined(NO_FSCHG)
MTREE_FSCHG=	-i
.endif
.endif

MTREES=		mtree/BSD.root.dist		/		\
		mtree/BSD.var.dist		/var		\
		mtree/BSD.usr.dist		/usr		\
		mtree/BSD.include.dist		/usr/include
.if ${MK_GROFF} != "no"
MTREES+=	mtree/BSD.groff.dist		/usr
.endif
.if ${MK_LIB32} != "no"
MTREES+=	mtree/BSD.lib32.dist		/usr
.endif
.if ${MK_LIBSOFT} != "no"
MTREES+=	mtree/BSD.libsoft.dist		/usr
.endif
.for mtree in ${LOCAL_MTREE}
MTREES+=	../${mtree}			/
.endfor

# Clean up some directories that where mistakenly created as files that
# should not have been as part of the nvi update in r281994.
# This should be removed after 11.0-RELEASE.
DISTRIB_CLEANUP_SHARE_FILES=	${SHAREDIR}/doc/usd/10.exref ${SHAREDIR}/doc/usd/11.edit
DISTRIB_CLEANUP_SHARE_FILES+=	${SHAREDIR}/doc/usd/12.vi ${SHAREDIR}/doc/usd/13.viref
distrib-cleanup: .PHONY
	for file in ${DISTRIB_CLEANUP_SHARE_FILES}; do \
		if [ -f ${DESTDIR}/$${file} ]; then \
			rm -f ${DESTDIR}/$${file}; \
		fi; \
	done

distrib-dirs: ${MTREES:N/*} distrib-cleanup .PHONY
	@set ${MTREES}; \
	while test $$# -ge 2; do \
		m=${.CURDIR}/$$1; \
		shift; \
		d=${DESTDIR}$$1; \
		shift; \
		test -d $$d || mkdir -p $$d; \
		${ECHO} ${MTREE_CMD} -deU ${MTREE_FSCHG} \
		    ${MTREE_FOLLOWS_SYMLINKS} -f $$m -p $$d; \
		${MTREE_FILTER} $$m | \
		${MTREE_CMD} -deU ${MTREE_FSCHG} ${MTREE_FOLLOWS_SYMLINKS} \
		    -p $$d; \
	done; true
.if defined(NO_ROOT)
	@set ${MTREES}; \
	while test $$# -ge 2; do \
		m=${.CURDIR}/$$1; \
		shift; \
		d=$$1; \
		test "$$d" == "/" && d=""; \
		d=${DISTBASE}$$d; \
		shift; \
		test -d ${DESTDIR}/$$d || mkdir -p ${DESTDIR}/$$d; \
		${ECHO} "${MTREE_CMD:N-W} -C -f $$m -K uname,gname | " \
		    "sed s#^\.#.$$d# | ${METALOG.add}" ; \
		${MTREE_FILTER} $$m | \
		${MTREE_CMD:N-W} -C -K uname,gname | sed s#^\.#.$$d# | \
		    ${METALOG.add} ; \
	done; true
.endif
	${INSTALL_SYMLINK} usr/src/sys ${DESTDIR}/sys
.if ${MK_NLS} != "no"
	set - `grep "^[a-zA-Z]" ${.CURDIR}/nls.alias`; \
	while [ $$# -gt 0 ] ; do \
		${INSTALL_SYMLINK} "$$2" "${DESTDIR}${SHAREDIR}/nls/$$1"; \
		shift; shift; \
	done
.endif

etc-examples-install: ${META_DEPS}
	cd ${.CURDIR}; ${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 444 \
	    ${BIN1} ${BIN2} nsmb.conf opieaccess \
	    ${DESTDIR}${SHAREDIR}/examples/etc

etc-examples: etc-examples-install
	${_+_}cd ${.CURDIR}/defaults; \
	    ${MAKE} ${${MK_STAGING} == "yes":?all:install} \
	    DESTDIR=${DESTDIR}${SHAREDIR}/examples

.include <bsd.prog.mk>
