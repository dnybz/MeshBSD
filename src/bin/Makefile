#	From: @(#)Makefile	8.1 (Berkeley) 5/31/93
# $FreeBSD: head/bin/Makefile 288904 2015-10-05 21:41:55Z imp $

.include <src.opts.mk>

SUBDIR= 	cat
SUBDIR+= 	cfg_load
SUBDIR+= 	cfg_safe
SUBDIR+= 	chflags
SUBDIR+= 	chio
SUBDIR+= 	chmod
SUBDIR+= 	cp
SUBDIR+= 	date
SUBDIR+= 	dd
SUBDIR+= 	df
SUBDIR+= 	domainname
SUBDIR+= 	echo
SUBDIR+= 	ed
SUBDIR+= 	expr
SUBDIR+= 	getfacl
SUBDIR+= 	hostname
SUBDIR+= 	kenv
SUBDIR+= 	kill
SUBDIR+= 	ln
SUBDIR+= 	ls
SUBDIR+= 	mkdir
SUBDIR+= 	mv
SUBDIR+= 	pkill
SUBDIR+= 	ps
SUBDIR+= 	pwait
SUBDIR+= 	pwd
SUBDIR+= 	rcp
SUBDIR+= 	realpath
SUBDIR+= 	rm
SUBDIR+= 	rmail
SUBDIR+= 	rmdir
SUBDIR+= 	setfacl
SUBDIR+= 	sh
SUBDIR+= 	sleep
SUBDIR+= 	stty
SUBDIR+= 	sync
SUBDIR+= 	test
SUBDIR+= 	tests
SUBDIR+= 	uuidgen

SUBDIR.${MK_TESTS}+=	tests

.include <bsd.arch.inc.mk>

SUBDIR:=	${SUBDIR:O}

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
