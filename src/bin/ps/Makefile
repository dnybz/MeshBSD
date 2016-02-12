# $FreeBSD: head/bin/ps/Makefile 289490 2015-10-18 07:30:50Z ngie $
#	@(#)Makefile	8.1 (Berkeley) 6/2/93

PROG=	ps
SRCS=	fmt.c keyword.c nlist.c print.c ps.c

#
# To support "lazy" ps for non root/wheel users
# add -DLAZY_PS to the cflags.  This helps
# keep ps from being an unnecessary load
# on large systems.
#
CFLAGS+=-DLAZY_PS
LIBADD=	m kvm jail xo

.include <bsd.prog.mk>
