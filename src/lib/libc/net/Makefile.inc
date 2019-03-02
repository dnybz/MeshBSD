#	from @(#)Makefile.inc	8.2 (Berkeley) 9/5/93
# $FreeBSD: releng/11.0/lib/libc/net/Makefile.inc 297283 2016-03-26 03:46:12Z bdrewery $

# machine-independent net sources
.PATH: ${LIBC_SRCTOP}/net

SRCS+=	base64.c ether_addr.c eui64.c \
	gai_strerror.c getaddrinfo.c \
	gethostbydns.c gethostbyht.c gethostbynis.c gethostnamadr.c \
	getifaddrs.c getifmaddrs.c getnameinfo.c \
	getnetbydns.c getnetbyht.c getnetbynis.c getnetnamadr.c \
	getproto.c getprotoent.c getprotoname.c getservent.c \
	if_indextoname.c if_nameindex.c if_nametoindex.c \
	ip6opt.c linkaddr.c map_v4v6.c name6.c ntoh.c \
	nsdispatch.c nslexer.l nsparser.y nss_compat.c \
	rcmd.c rcmdsh.c recv.c rthdr.c sctp_sys_calls.c send.c \
        sockatmark.c sourcefilter.c vars.c

.if ${MK_NS_CACHING} != "no"
SRCS+=	nscache.c nscachedcli.c
.endif

SYM_MAPS+=${LIBC_SRCTOP}/net/Symbol.map

.if ${MK_INET6_SUPPORT} != "no"
CFLAGS+=-DINET6
.endif

CFLAGS+=-I${.OBJDIR}

# name6.c refers res_private.h
CFLAGS+=-I${LIBC_SRCTOP}/resolv

YFLAGS+=-p_nsyy
LFLAGS+=-P_nsyy

CFLAGS.nslexer.c= -DYY_BUF_SIZE=1024

.if ${MK_HESIOD} != "no"
SRCS+=	hesiod.c 
MAN+=	hesiod.3
.endif

