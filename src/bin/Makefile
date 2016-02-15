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
	echo \
	expr \
	getfacl \
	hostname \
	kenv \
	kill \
	ln \
	ls \
	mkdir \
	mv \
	pax \
	pkill \
	ps \
	pwait \
	pwd \
	realpath \
	rm \
	setfacl \
	sh \
	sleep \
	stty \
	sync \
	test \
	uuidgen

SUBDIR.${MK_RCMDS}+=	rcp
SUBDIR.${MK_SENDMAIL}+=	rmail
SUBDIR.${MK_TCSH}+=	csh
SUBDIR.${MK_TESTS}+=	tests

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
