# $FreeBSD: releng/11.0/lib/libc/posix1e/Makefile.inc 296915 2016-03-15 19:26:32Z emaste $

.PATH: ${LIBC_SRCTOP}/posix1e

CFLAGS+=-D_ACL_PRIVATE

# Copy kern/subr_acl_nfs4.c to the libc object directory.
subr_acl_nfs4.c: ${LIBC_SRCTOP}/../../sys/kern/subr_acl_nfs4.c
	cat ${.ALLSRC} > ${.TARGET}

SRCS+=	acl_branding.c			\
	acl_calc_mask.c			\
	acl_copy.c			\
	acl_delete.c			\
	acl_delete_entry.c		\
	acl_entry.c			\
	acl_flag.c			\
	acl_free.c			\
	acl_from_text.c			\
	acl_from_text_nfs4.c		\
	acl_get.c			\
	acl_id_to_name.c		\
	acl_init.c			\
	acl_perm.c			\
	acl_set.c			\
	acl_strip.c			\
	acl_support.c			\
	acl_support_nfs4.c		\
	acl_to_text.c			\
	acl_to_text_nfs4.c		\
	acl_valid.c			\
	extattr.c			\
	mac.c				\
	mac_exec.c			\
	mac_get.c			\
	mac_set.c			\
	subr_acl_nfs4.c
.if ${MK_SYMVER} == yes
SRCS+=	acl_compat.c
.endif

SYM_MAPS+=${LIBC_SRCTOP}/posix1e/Symbol.map

CLEANFILES+=	subr_acl_nfs4.c
