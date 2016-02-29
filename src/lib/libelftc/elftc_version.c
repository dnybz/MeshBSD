/* $FreeBSD: head/lib/libelftc/elftc_version.c 294297 2016-01-18 21:53:39Z emaste $ */

#include <sys/types.h>
#include <libelftc.h>

const char *
elftc_version(void)
{
	return "elftoolchain r3272M";
}
