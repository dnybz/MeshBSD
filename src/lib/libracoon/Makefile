
PACKAGE=lib${LIB}
SHLIBDIR?= /lib

.include <src.opts.mk>

RACOONDIR= 	${.CURDIR}/../../external/racoon2

SHLIB_MAJOR= 0
LIB=	racoon

.PATH:		${RACOONDIR}/lib
SRCS+=	vmbuf.c rc_type.c rc_net.c rc_str.c \
	if_pfkeyv2.c if_spmd.c \
	cfparse.y cftoken.l cfsetup.c  \
	pidfile.c plog.c safefile.c version.c rbuf.c addresspool.c

MAN=

CFLAGS+= 	-DENABLE_SECURE
CFLAGS+= 	-Werror 
CFLAGS+= 	-Wall 
CFLAGS+= 	-Wmissing-prototypes 
CFLAGS+= 	-Wmissing-declarations

CFLAGS+= 	-DSTDC_HEADERS
CFLAGS+= 	-DHAVE_SYS_WAIT_H
CFLAGS+= 	-DHAVE_SYS_TIME_H 
CFLAGS+= 	-DHAVE_NET_IF_VAR_H 
CFLAGS+= 	-DHAVE_NET_ETHERNET_H 
CFLAGS+= 	-DHAVE_LIMITS_H 
CFLAGS+= 	-DHAVE_UNISTD_H 
CFLAGS+= 	-DHAVE_STDARG_H 
CFLAGS+= 	-DHAVE_NETIPSEC_IPSEC_H 
CFLAGS+= 	-DHAVE_NET_PFKEYV2_H 
CFLAGS+= 	-DENABLE_NATT 
CFLAGS+= 	-DTIME_WITH_SYS_TIME 
CFLAGS+= 	-DHAVE_STRFTIME 
CFLAGS+= 	-DHAVE_VPRINTF 
CFLAGS+= 	-DHAVE_GETTIMEOFDAY 
CFLAGS+= 	-DHAVE_MKTIME 
CFLAGS+= 	-DHAVE_SOCKET 
CFLAGS+= 	-DHAVE_STRDUP 
CFLAGS+= 	-DHAVE_STRERROR 
CFLAGS+= 	-DHAVE_STRTOL 
CFLAGS+= 	-DHAVE_GETIFADDRS 
CFLAGS+= 	-DHAVE_STRLCPY 
CFLAGS+= 	-DHAVE_STRLCAT 
CFLAGS+= 	-DHAVE_ATOLL 
CFLAGS+= 	-DHAVE_SA_LEN 
CFLAGS+= 	-DHAVE_OPENSSL_EVP_H 

.if ${MK_INET6_SUPPORT} != "no"
CFLAGS+= 	-DINET6
.endif

CFLAGS+= 	-DCF_DEBUG

CFLAGS+= 	-I${RACOONDIR}/lib	
LIBADD= 	crypto

CLEANFILES+= 	cfparse.c
CLEANFILES+= 	cftoken.c
CLEANFILES+= 	y.tab.c
CLEANFILES+= 	y.tab.h

SRCS+=  y.tab.h
y.tab.h:	cfparse.y

WARNS?=	0

.include <bsd.lib.mk>

