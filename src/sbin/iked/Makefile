
.include <src.opts.mk>

PACKAGE= 	iked
PROG= 	iked

RACOONDIR= 	${.CURDIR}/../../external/racoon2

.PATH: 	${RACOONDIR}/iked
IKEV1_SRC= 	ikev1/ikev1.c ikev1/genlist.c ikev1/ipsec_doi.c \
	ikev1/isakmp_ident.c ikev1/isakmp_quick.c ikev1/isakmp_inf.c \
	ikev1/handler.c ikev1/strnames.c ikev1/oakley.c ikev1/vendorid.c \
	ikev1/algorithm.c ikev1/proposal.c  ikev1/pfkey.c 

IKEV1_NATT_SRC= 	ikev1/ikev1_natt.c

IKEV2_SRC= 	ikev2.c ikev2_auth.c ikev2_child.c ikev2_cookie.c \
	ikev2_notify.c ikev2_packet.c ikev2_payload.c ikev2_proposal.c \
	ikev2_rekey.c ikev2_config.c 

IKEV2_NATT_SRC= 	nattraversal.c 

SRCS= 	main.c dh.c ike_sa.c ike_conf.c isakmp.c authenticator.c \
	encryptor.c crypto_openssl.c str2val.c keyed_hash.c sockmisc.c \
	ike_pfkey.c ike_spmif.c schedule.c ratelimit.c script.c \
	${IKEV1_SRC} ${IKEV1_NATT_SRC} ${IKEV2_SRC} ${IKEV2_NATT_SRC}

MAN=	iked.8

LIBADD= 	racoon crypto 

CFLAGS= 	-I${RACOONDIR}/../../sbin/iked
CFLAGS+= 	-I${RACOONDIR}/iked 
CFLAGS+= 	-I${RACOONDIR}/iked/missing
CFLAGS+= 	-I${RACOONDIR}/lib

CFLAGS+= 	-DDEBUG
CFLAGS+= 	-DDEBUG_TRACE
# CFLAGS+= 	-DSELFTEST
CFLAGS+= 	-DHAVE_CONFIG_H
# CFLAGS+= 	-DSADB_X_MIGRATE

#CFLAGS+= 	-DHAVE_LIBPCAP
#CFLAGS+= 	-DWITH_RTSOCK

CFLAGS+= 	-DSYSCONFDIR=\"/etc/c/racoon2\"

.if ${MK_INET6_SUPPORT} != "no"
CFLAGS+= 	-DINET6
CFLAGS+= 	-DADVAPI
.endif

CFLAGS+= 	-Wall 

WARNS?= 	0

.include <bsd.prog.mk>
