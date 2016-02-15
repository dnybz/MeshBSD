#	From: @(#)Makefile	8.1 (Berkeley) 5/31/93
# $FreeBSD: head/bin/Makefile 288904 2015-10-05 21:41:55Z imp $

.include <src.opts.mk>

SUBDIR= cat \
	chflags \
	chmod \
	cp \
	date \
	dd \
	df \
	expr \
	hostname \
	kenv \
	kill \
	ln \
	ls \
	mkdir \
	mv \
	pkill \
	ps \
	rm \
	sh \
	sleep \
	stty \
	test \
	uuidgen

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
