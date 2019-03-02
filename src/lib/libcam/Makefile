# $FreeBSD: releng/11.0/lib/libcam/Makefile 298107 2016-04-16 07:45:30Z gjb $

PACKAGE=	lib${LIB}
LIB=		cam
SHLIBDIR?=	/lib
SRCS=		camlib.c scsi_cmdparse.c scsi_all.c scsi_da.c scsi_sa.c cam.c \
		ata_all.c smp_all.c
INCS=		camlib.h

LIBADD=		sbuf

WARNS?=		2

.PATH:		${.CURDIR}/../../sys/cam/scsi ${.CURDIR}/../../sys/cam/ata \
		${.CURDIR}/../../sys/cam

SDIR=		${.CURDIR}/../../sys
CFLAGS+=	-I${.CURDIR} -I${SDIR}

SHLIB_MAJOR=	7

.include <bsd.lib.mk>
