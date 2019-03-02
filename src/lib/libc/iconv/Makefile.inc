# $FreeBSD: releng/11.0/lib/libc/iconv/Makefile.inc 296915 2016-03-15 19:26:32Z emaste $

# iconv sources
.PATH: ${LIBC_SRCTOP}/iconv

SRCS+=	citrus_bcs.c citrus_bcs_strtol.c citrus_bcs_strtoul.c \
	citrus_csmapper.c citrus_db.c citrus_db_factory.c citrus_db_hash.c \
	citrus_esdb.c citrus_hash.c citrus_iconv.c citrus_lookup.c \
	citrus_lookup_factory.c citrus_mapper.c citrus_memstream.c \
	citrus_mmap.c citrus_module.c citrus_none.c citrus_pivot_factory.c \
	citrus_prop.c citrus_stdenc.c bsd_iconv.c
.if ${MK_SYMVER} == yes
SRCS+=	iconv_compat.c
.endif

SYM_MAPS+= ${LIBC_SRCTOP}/iconv/Symbol.map

.if ${MK_ICONV} == yes
.include "${LIBC_SRCTOP}/../libc_nonshared/Makefile.iconv"
.endif
