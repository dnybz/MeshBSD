#	from: @(#)Makefile    5.10 (Berkeley) 6/24/90
# $FreeBSD: releng/11.0/lib/librpcsvc/Makefile 298107 2016-04-16 07:45:30Z gjb $

.include <src.opts.mk>

.PATH: ${.CURDIR}/../../include/rpcsvc

PACKAGE=lib${LIB}
LIB=    rpcsvc

RPCSRCS= klm_prot.x mount.x nfs_prot.x nlm_prot.x rex.x rnusers.x \
	rquota.x rstat.x rwall.x sm_inter.x spray.x yppasswd.x ypxfrd.x \
	ypupdate_prot.x

OTHERSRCS=  rnusers.c rstat.c rwall.c
SECRPCSRCS= secretkey.c xcrypt.c

RPCCOM=	RPCGEN_CPP=${CPP:Q} rpcgen -C

INCDIRS= -I${DESTDIR}/usr/include/rpcsvc

CFLAGS+= -DYP ${INCDIRS}

GENSRCS= ${RPCSRCS:R:S/$/_xdr.c/g}
SRCS+= ${GENSRCS} ${OTHERSRCS} ${SECRPCSRCS}

CLEANFILES+= ${GENSRCS}

WARNS?=	1

.include <bsd.lib.mk>

.SUFFIXES: .x _xdr.c

.x_xdr.c:
	${RPCCOM} -c ${.IMPSRC} -o ${.TARGET}

OBJS=	${RPCSRCS:R:S/$/_xdr.o/g} ${SECRPCSRCS:R:S/$/.o/g} \
	${OTHERSRCS:R:S/$/.o/g}
