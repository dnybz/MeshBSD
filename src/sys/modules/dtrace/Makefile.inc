# $FreeBSD: head/sys/modules/dtrace/Makefile.inc 285703 2015-07-19 22:14:09Z markj $

IGNORE_PRAGMA=	1

load	:
	-kldload dtrace
	-kldload profile
	-kldload systrace
	kldstat

unload	:
	-kldunload systrace
	-kldunload profile
	-kldunload dtrace
	kldstat
