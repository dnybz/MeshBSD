# $FreeBSD: releng/11.0/lib/libarchive/Makefile 299529 2016-05-12 10:16:16Z mm $
.include <src.opts.mk>

PACKAGE=lib${LIB}
_LIBARCHIVEDIR=	${.CURDIR}/../../contrib/libarchive

LIB=	archive

LIBADD=	z bz2 lzma bsdxml
CFLAGS+= -DHAVE_BZLIB_H=1 -DHAVE_LIBLZMA=1 -DHAVE_LZMA_H=1

# FreeBSD SHLIB_MAJOR value is managed as part of the FreeBSD system.
# It has no real relation to the libarchive version number.
SHLIB_MAJOR= 6

CFLAGS+=	-DPLATFORM_CONFIG_H=\"${.CURDIR}/config_freebsd.h\"
CFLAGS+=	-I${.OBJDIR}

.if ${MK_LIBRESSL} != "no"
CFLAGS+=	-DWITH_OPENSSL
LIBADD+=	crypto
.else
LIBADD+=	md
.endif

.if ${MK_ICONV} != "no"
# TODO: This can be changed back to CFLAGS once iconv works correctly
# with statically linked binaries.
SHARED_CFLAGS+=	-DHAVE_ICONV=1 -DHAVE_ICONV_H=1 -DICONV_CONST=
.endif

.if ${MACHINE_ARCH:Marm*} != "" || ${MACHINE_ARCH:Mmips*} != "" || \
	${MACHINE_ARCH:Msparc64*} != ""
NO_WCAST_ALIGN=	yes
.if ${MACHINE_ARCH:M*64*} == ""
CFLAGS+=	-DPPMD_32BIT
.endif
.endif
NO_WCAST_ALIGN.clang=

.PATH: ${_LIBARCHIVEDIR}/libarchive

# Headers to be installed in /usr/include
INCS=	archive.h archive_entry.h

# Sources to be compiled.
SRCS=	archive_acl.c					\
	archive_check_magic.c				\
	archive_cmdline.c				\
	archive_cryptor.c				\
	archive_digest.c				\
	archive_entry.c					\
	archive_entry_copy_stat.c			\
	archive_entry_link_resolver.c			\
	archive_entry_sparse.c				\
	archive_entry_stat.c				\
	archive_entry_strmode.c				\
	archive_entry_xattr.c				\
	archive_getdate.c				\
	archive_hmac.c					\
	archive_match.c					\
	archive_options.c				\
	archive_pack_dev.c				\
	archive_pathmatch.c				\
	archive_ppmd7.c					\
	archive_random.c				\
	archive_rb.c					\
	archive_read.c					\
	archive_read_add_passphrase.c			\
	archive_read_append_filter.c			\
	archive_read_data_into_fd.c			\
	archive_read_disk_entry_from_file.c		\
	archive_read_disk_posix.c			\
	archive_read_disk_set_standard_lookup.c		\
	archive_read_extract.c				\
	archive_read_extract2.c				\
	archive_read_open_fd.c				\
	archive_read_open_file.c			\
	archive_read_open_filename.c			\
	archive_read_open_memory.c			\
	archive_read_set_format.c			\
	archive_read_set_options.c			\
	archive_read_support_filter_all.c		\
	archive_read_support_filter_bzip2.c		\
	archive_read_support_filter_compress.c		\
	archive_read_support_filter_gzip.c		\
	archive_read_support_filter_grzip.c		\
	archive_read_support_filter_lrzip.c		\
	archive_read_support_filter_lz4.c		\
	archive_read_support_filter_lzop.c		\
	archive_read_support_filter_none.c		\
	archive_read_support_filter_program.c		\
	archive_read_support_filter_rpm.c		\
	archive_read_support_filter_uu.c		\
	archive_read_support_filter_xz.c		\
	archive_read_support_format_7zip.c		\
	archive_read_support_format_all.c		\
	archive_read_support_format_ar.c		\
	archive_read_support_format_by_code.c		\
	archive_read_support_format_cab.c		\
	archive_read_support_format_cpio.c		\
	archive_read_support_format_empty.c		\
	archive_read_support_format_iso9660.c		\
	archive_read_support_format_lha.c		\
	archive_read_support_format_mtree.c		\
	archive_read_support_format_rar.c		\
	archive_read_support_format_raw.c		\
	archive_read_support_format_tar.c		\
	archive_read_support_format_warc.c		\
	archive_read_support_format_xar.c		\
	archive_read_support_format_zip.c		\
	archive_string.c				\
	archive_string_sprintf.c			\
	archive_util.c					\
	archive_virtual.c				\
	archive_write.c					\
	archive_write_add_filter.c			\
	archive_write_disk_acl.c			\
	archive_write_disk_set_standard_lookup.c	\
	archive_write_disk_posix.c			\
	archive_write_open_fd.c				\
	archive_write_open_file.c			\
	archive_write_open_filename.c			\
	archive_write_open_memory.c			\
	archive_write_add_filter_b64encode.c		\
	archive_write_add_filter_by_name.c		\
	archive_write_add_filter_bzip2.c		\
	archive_write_add_filter_compress.c		\
	archive_write_add_filter_grzip.c		\
	archive_write_add_filter_gzip.c			\
	archive_write_add_filter_lrzip.c		\
	archive_write_add_filter_lz4.c			\
	archive_write_add_filter_lzop.c			\
	archive_write_add_filter_none.c			\
	archive_write_add_filter_program.c		\
	archive_write_add_filter_uuencode.c		\
	archive_write_add_filter_xz.c			\
	archive_write_set_format.c			\
	archive_write_set_format_7zip.c			\
	archive_write_set_format_ar.c			\
	archive_write_set_format_by_name.c		\
	archive_write_set_format_cpio.c			\
	archive_write_set_format_cpio_newc.c		\
	archive_write_set_format_filter_by_ext.c	\
	archive_write_set_format_gnutar.c		\
	archive_write_set_format_iso9660.c		\
	archive_write_set_format_mtree.c		\
	archive_write_set_format_pax.c			\
	archive_write_set_format_raw.c			\
	archive_write_set_format_shar.c			\
	archive_write_set_format_ustar.c		\
	archive_write_set_format_v7tar.c		\
	archive_write_set_format_warc.c			\
	archive_write_set_format_xar.c			\
	archive_write_set_format_zip.c			\
	archive_write_set_passphrase.c			\
	archive_write_set_options.c			\
	filter_fork_posix.c

.if ${MK_TESTS} != "no"
SUBDIR+=	tests
.endif

.include <bsd.lib.mk>
