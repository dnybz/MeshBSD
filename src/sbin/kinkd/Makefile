
.include <src.opts.mk>

PACKAGE= 	kinkd
PROG= 	kinkd

RACOONDIR= 	${.CURDIR}/../../external/racoon2
RACOON2_CONFIG_DIR= 	/etc/racoon2

.PATH: 	${RACOONDIR}/kinkd
SRCS= 	main.c \
	kink_fmt.c kink_crypto.c bbkk_common.c bbkk_heimdal.c \
	session.c base.c dpd.c peer.c handle.c \
	algorithm.c crypto_openssl.c oakley.c \
	ipsec_doi.c isakmp.c isakmp_quick.c isakmp_inf.c proposal.c \
	strnames.c \
	sockmisc.c \
	pfkey.c kink_conf.c rct_ipsecdoi.c \
	etchosts.c \
	sched_select.c utils.c

MAN=	kinkd.8

LIBADD= 	racoon
LIBADD+= 	krb5
LIBADD+= 	hx509
LIBADD+= 	asn1
LIBADD+= 	roken
LIBADD+= 	com_err
LIBADD+= 	crypt
LIBADD+= 	crypto 

CFLAGS= 	-I${RACOONDIR}/../../sbin/kinkd
CFLAGS+= 	-I${RACOONDIR}/kinkd 
CFLAGS+= 	-I${RACOONDIR}/lib

CFLAGS+= 	-DWITH_PLOG 
CFLAGS+= 	-DSTDC_HEADERS 
CFLAGS+= 	-DHAVE_SYS_TIME_H 
CFLAGS+= 	-DHAVE_STDARG_H 
CFLAGS+= 	-DHAVE_UNISTD_H 
CFLAGS+= 	-DHAVE_KRB5_H 
CFLAGS+= 	-DHAVE_NET_PFKEYV2_H 
CFLAGS+= 	-DHAVE_NETIPSEC_IPSEC_H 
CFLAGS+= 	-DTIME_WITH_SYS_TIME 
CFLAGS+= 	-DRETSIGTYPE=void 
CFLAGS+= 	-DHAVE_GETPROGNAME 
CFLAGS+= 	-DHAVE_GETTIMEOFDAY 
CFLAGS+= 	-DHAVE_SELECT 
CFLAGS+= 	-DHAVE_SOCKET 
CFLAGS+= 	-DHAVE_STRDUP 
CFLAGS+= 	-DHAVE_STRERROR 
CFLAGS+= 	-DHAVE_STRSTR 
CFLAGS+= 	-DHAVE_LIBCRYPTO 
CFLAGS+= 	-DHAVE_LIBCRYPT 
CFLAGS+= 	-DHAVE_LIBCOM_ERR 
CFLAGS+= 	-DHAVE_LIBROKEN 
CFLAGS+= 	-DHAVE_LIBASN1 
CFLAGS+= 	-DHAVE_LIBHX509 
CFLAGS+= 	-DHAVE_LIBKRB5 
CFLAGS+= 	-DHAVE_LIBKRB5 
CFLAGS+= 	-DHAVE_MIT_COMPAT_KRB5_FREE_TICKET 
CFLAGS+= 	-DHAVE_KRB5_GET_MAX_TIME_SKEW 
CFLAGS+= 	-DHAVE_KRB5_GET_KDC_SEC_OFFSET 
CFLAGS+= 	-DHAVE_RFC3962_AES 
CFLAGS+= 	-DHAVE_SA_LEN 
CFLAGS+= 	-DHAVE_SYS_QUEUE_H  

CFLAGS+= 	-DRACOON2_CONFIG_DIR=\"${RACOON2_CONFIG_DIR}\"

CFLAGS+= 	-Wall -Wmissing-prototypes -Wmissing-declarations

WARNS?= 	0

.include <bsd.prog.mk>
