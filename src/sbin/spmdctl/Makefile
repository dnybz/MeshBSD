
.include <src.opts.mk>

PACKAGE= 	spmdctl
PROG= 	spmdctl

RACOONDIR= 	${.CURDIR}/../../external/racoon2
RACOON2_CONFIG_DIR= 	/etc/racoon2

.PATH: 	${RACOONDIR}/spmd
SRCS= 	spmdctl.c

MAN=	spmdctl.8

LIBADD= 	racoon
LIBADD+= 	crypto 

CFLAGS= 	-I${RACOONDIR}/../../sbin/spmd
CFLAGS+= 	-I${RACOONDIR}/spmd 
CFLAGS+= 	-I${RACOONDIR}/lib

CFLAGS+= 	-DHAVE_CONFIG_H -DSYSCONFDIR=\"${RACOON2_CONFIG_DIR}\" 

WARNS?= 	0

.include <bsd.prog.mk>
