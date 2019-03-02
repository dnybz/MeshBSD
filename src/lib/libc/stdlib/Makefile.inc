#	from @(#)Makefile.inc	8.3 (Berkeley) 2/4/95
# $FreeBSD: releng/11.0/lib/libc/stdlib/Makefile.inc 304592 2016-08-22 07:38:44Z kib $

# machine-independent stdlib sources
.PATH: ${LIBC_SRCTOP}/${LIBC_ARCH}/stdlib ${LIBC_SRCTOP}/stdlib

MISRCS+=_Exit.c a64l.c abort.c abs.c atexit.c atof.c atoi.c atol.c atoll.c \
	bsearch.c cxa_thread_atexit.c div.c exit.c getenv.c getopt.c getopt_long.c \
	getsubopt.c hcreate.c hcreate_r.c hdestroy_r.c heapsort.c heapsort_b.c \
	hsearch_r.c imaxabs.c imaxdiv.c \
	insque.c l64a.c labs.c ldiv.c llabs.c lldiv.c lsearch.c \
	merge.c mergesort_b.c ptsname.c qsort.c qsort_r.c quick_exit.c \
	radixsort.c rand.c \
	random.c reallocarray.c reallocf.c realpath.c remque.c strfmon.c \
	strsuftoll.c strtoimax.c \
	strtol.c strtoll.c strtoq.c strtoul.c strtonum.c strtoull.c \
        strtoumax.c strtouq.c system.c tdelete.c tfind.c tsearch.c twalk.c

SYM_MAPS+= ${LIBC_SRCTOP}/stdlib/Symbol.map

# machine-dependent stdlib sources
.sinclude "${LIBC_SRCTOP}/${LIBC_ARCH}/stdlib/Makefile.inc"

