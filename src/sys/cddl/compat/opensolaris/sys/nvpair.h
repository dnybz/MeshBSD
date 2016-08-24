/*-
 * Copyright (c) 2014 Sandvine Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: head/sys/cddl/compat/opensolaris/sys/nvpair.h 286796 2015-08-15 06:34:49Z oshogbo $
 */

#ifndef _OPENSOLARIS_SYS_NVPAIR_H_
#define _OPENSOLARIS_SYS_NVPAIR_H_

#ifdef _KERNEL

/*
 * Some of the symbols in the Illumos nvpair library conflict with symbols
 * provided by nv(9), so we use this preprocessor hack to avoid the conflict.
 *
 * This list was generated by:
 *   cat nv.h nv_impl.h nvlist_* nvpair_impl.h | \
 *     sed -nE 's/^[[:alnum:]_][[:alnum:]_ ]*[[:space:]]+[*]*([[:alnum:]_]+)\(.*$/#define \1 illumos_\1/p' | \
 *     sort -u
 */
#define nvlist_add_binary illumos_nvlist_add_binary
#define nvlist_add_bool illumos_nvlist_add_bool
#define nvlist_add_bool_array illumos_nvlist_add_bool_array
#define nvlist_add_descriptor illumos_nvlist_add_descriptor
#define nvlist_add_descriptor_array illumos_nvlist_add_descriptor_array
#define nvlist_add_null illumos_nvlist_add_null
#define nvlist_add_number illumos_nvlist_add_number
#define nvlist_add_number_array illumos_nvlist_add_number_array
#define nvlist_add_nvlist illumos_nvlist_add_nvlist
#define nvlist_add_nvlist_array illumos_nvlist_add_nvlist_array
#define nvlist_add_nvpair illumos_nvlist_add_nvpair
#define nvlist_add_string illumos_nvlist_add_string
#define nvlist_add_string_array illumos_nvlist_add_string_array
#define nvlist_add_stringf illumos_nvlist_add_stringf
#define nvlist_add_stringv illumos_nvlist_add_stringv
#define nvlist_clone illumos_nvlist_clone
#define nvlist_create illumos_nvlist_create
#define nvlist_descriptors illumos_nvlist_descriptors
#define nvlist_destroy illumos_nvlist_destroy
#define nvlist_dump illumos_nvlist_dump
#define nvlist_empty illumos_nvlist_empty
#define nvlist_error illumos_nvlist_error
#define nvlist_exists illumos_nvlist_exists
#define nvlist_exists_binary illumos_nvlist_exists_binary
#define nvlist_exists_bool illumos_nvlist_exists_bool
#define nvlist_exists_bool_array illumos_nvlist_exists_bool_array
#define nvlist_exists_descriptor illumos_nvlist_exists_descriptor
#define nvlist_exists_descriptor_array illumos_nvlist_exists_descriptor_array
#define nvlist_exists_null illumos_nvlist_exists_null
#define nvlist_exists_number illumos_nvlist_exists_number
#define nvlist_exists_number_array illumos_nvlist_exists_number_array
#define nvlist_exists_nvlist illumos_nvlist_exists_nvlist
#define nvlist_exists_nvlist_array illumos_nvlist_exists_nvlist_array
#define nvlist_exists_string illumos_nvlist_exists_string
#define nvlist_exists_string_array illumos_nvlist_exists_string_array
#define nvlist_exists_type illumos_nvlist_exists_type
#define nvlist_fdump illumos_nvlist_fdump
#define nvlist_first_nvpair illumos_nvlist_first_nvpair
#define nvlist_flags illumos_nvlist_flags
#define nvlist_free illumos_nvlist_free
#define nvlist_free_binary illumos_nvlist_free_binary
#define nvlist_free_binary_array illumos_nvlist_free_binary_array
#define nvlist_free_bool illumos_nvlist_free_bool
#define nvlist_free_bool_array illumos_nvlist_free_bool_array
#define nvlist_free_descriptor illumos_nvlist_free_descriptor
#define nvlist_free_descriptor_array illumos_nvlist_free_descriptor_array
#define nvlist_free_null illumos_nvlist_free_null
#define nvlist_free_number illumos_nvlist_free_number
#define nvlist_free_number_array illumos_nvlist_free_number_array
#define nvlist_free_nvlist illumos_nvlist_free_nvlist
#define nvlist_free_nvlist_array illumos_nvlist_free_nvlist_array
#define nvlist_free_nvpair illumos_nvlist_free_nvpair
#define nvlist_free_string illumos_nvlist_free_string
#define nvlist_free_string_array illumos_nvlist_free_string_array
#define nvlist_free_type illumos_nvlist_free_type
#define nvlist_get_array_next illumos_nvlist_get_array_next
#define nvlist_get_binary illumos_nvlist_get_binary
#define nvlist_get_bool illumos_nvlist_get_bool
#define nvlist_get_bool_array illumos_nvlist_get_bool_array
#define nvlist_get_descriptor illumos_nvlist_get_descriptor
#define nvlist_get_descriptor_array illumos_nvlist_get_descriptor_array
#define nvlist_get_number illumos_nvlist_get_number
#define nvlist_get_number_array illumos_nvlist_get_number_array
#define nvlist_get_nvlist illumos_nvlist_get_nvlist
#define nvlist_get_nvpair illumos_nvlist_get_nvpair
#define nvlist_get_nvpair_parent illumos_nvlist_get_nvpair_parent
#define nvlist_get_pararr illumos_nvlist_get_pararr
#define nvlist_get_parent illumos_nvlist_get_parent
#define nvlist_get_string illumos_nvlist_get_string
#define nvlist_in_array illumos_nvlist_in_array
#define nvlist_move_binary illumos_nvlist_move_binary
#define nvlist_move_bool_array illumos_nvlist_move_bool_array
#define nvlist_move_descriptor illumos_nvlist_move_descriptor
#define nvlist_move_descriptor_array illumos_nvlist_move_descriptor_array
#define nvlist_move_number_array illumos_nvlist_move_number_array
#define nvlist_move_nvlist illumos_nvlist_move_nvlist
#define nvlist_move_nvlist_array illumos_nvlist_move_nvlist_array
#define nvlist_move_nvpair illumos_nvlist_move_nvpair
#define nvlist_move_string illumos_nvlist_move_string
#define nvlist_move_string_array illumos_nvlist_move_string_array
#define nvlist_ndescriptors illumos_nvlist_ndescriptors
#define nvlist_next illumos_nvlist_next
#define nvlist_next_nvpair illumos_nvlist_next_nvpair
#define nvlist_pack illumos_nvlist_pack
#define nvlist_prev_nvpair illumos_nvlist_prev_nvpair
#define nvlist_recv illumos_nvlist_recv
#define nvlist_remove_nvpair illumos_nvlist_remove_nvpair
#define nvlist_send illumos_nvlist_send
#define nvlist_set_array_next illumos_nvlist_set_array_next
#define nvlist_set_error illumos_nvlist_set_error
#define nvlist_set_flags illumos_nvlist_set_flags
#define nvlist_set_parent illumos_nvlist_set_parent
#define nvlist_size illumos_nvlist_size
#define nvlist_take_binary illumos_nvlist_take_binary
#define nvlist_take_bool illumos_nvlist_take_bool
#define nvlist_take_bool_array illumos_nvlist_take_bool_array
#define nvlist_take_descriptor illumos_nvlist_take_descriptor
#define nvlist_take_descriptor_array illumos_nvlist_take_descriptor_array
#define nvlist_take_number illumos_nvlist_take_number
#define nvlist_take_number_array illumos_nvlist_take_number_array
#define nvlist_take_nvlist illumos_nvlist_take_nvlist
#define nvlist_take_nvlist_array illumos_nvlist_take_nvlist_array
#define nvlist_take_nvpair illumos_nvlist_take_nvpair
#define nvlist_take_string illumos_nvlist_take_string
#define nvlist_take_string_array illumos_nvlist_take_string_array
#define nvlist_unpack illumos_nvlist_unpack
#define nvlist_unpack_header illumos_nvlist_unpack_header
#define nvlist_xfer illumos_nvlist_xfer
#define nvpair_assert illumos_nvpair_assert
#define nvpair_clone illumos_nvpair_clone
#define nvpair_create_binary illumos_nvpair_create_binary
#define nvpair_create_bool illumos_nvpair_create_bool
#define nvpair_create_bool_array illumos_nvpair_create_bool_array
#define nvpair_create_descriptor illumos_nvpair_create_descriptor
#define nvpair_create_descriptor_array illumos_nvpair_create_descriptor_array
#define nvpair_create_null illumos_nvpair_create_null
#define nvpair_create_number illumos_nvpair_create_number
#define nvpair_create_number_array illumos_nvpair_create_number_array
#define nvpair_create_nvlist illumos_nvpair_create_nvlist
#define nvpair_create_nvlist_array illumos_nvpair_create_nvlist_array
#define nvpair_create_string illumos_nvpair_create_string
#define nvpair_create_string_array illumos_nvpair_create_string_array
#define nvpair_create_stringf illumos_nvpair_create_stringf
#define nvpair_create_stringv illumos_nvpair_create_stringv
#define nvpair_free illumos_nvpair_free
#define nvpair_free_structure illumos_nvpair_free_structure
#define nvpair_get_binary illumos_nvpair_get_binary
#define nvpair_get_bool illumos_nvpair_get_bool
#define nvpair_get_bool_array illumos_nvpair_get_bool_array
#define nvpair_get_descriptor illumos_nvpair_get_descriptor
#define nvpair_get_descriptor_array illumos_nvpair_get_descriptor_array
#define nvpair_get_number illumos_nvpair_get_number
#define nvpair_get_number_array illumos_nvpair_get_number_array
#define nvpair_get_nvlist illumos_nvpair_get_nvlist
#define nvpair_get_string illumos_nvpair_get_string
#define nvpair_header_size illumos_nvpair_header_size
#define nvpair_init_datasize illumos_nvpair_init_datasize
#define nvpair_insert illumos_nvpair_insert
#define nvpair_move_binary illumos_nvpair_move_binary
#define nvpair_move_bool_array illumos_nvpair_move_bool_array
#define nvpair_move_descriptor illumos_nvpair_move_descriptor
#define nvpair_move_descriptor_array illumos_nvpair_move_descriptor_array
#define nvpair_move_number_array illumos_nvpair_move_number_array
#define nvpair_move_nvlist illumos_nvpair_move_nvlist
#define nvpair_move_nvlist_array illumos_nvpair_move_nvlist_array
#define nvpair_move_string illumos_nvpair_move_string
#define nvpair_move_string_array illumos_nvpair_move_string_array
#define nvpair_name illumos_nvpair_name
#define nvpair_next illumos_nvpair_next
#define nvpair_nvlist illumos_nvpair_nvlist
#define nvpair_pack_binary illumos_nvpair_pack_binary
#define nvpair_pack_bool illumos_nvpair_pack_bool
#define nvpair_pack_bool_array illumos_nvpair_pack_bool_array
#define nvpair_pack_descriptor illumos_nvpair_pack_descriptor
#define nvpair_pack_descriptor_array illumos_nvpair_pack_descriptor_array
#define nvpair_pack_header illumos_nvpair_pack_header
#define nvpair_pack_null illumos_nvpair_pack_null
#define nvpair_pack_number illumos_nvpair_pack_number
#define nvpair_pack_number_array illumos_nvpair_pack_number_array
#define nvpair_pack_nvlist_array_next illumos_nvpair_pack_nvlist_array_next
#define nvpair_pack_nvlist_up illumos_nvpair_pack_nvlist_up
#define nvpair_pack_string illumos_nvpair_pack_string
#define nvpair_pack_string_array illumos_nvpair_pack_string_array
#define nvpair_prev illumos_nvpair_prev
#define nvpair_remove illumos_nvpair_remove
#define nvpair_size illumos_nvpair_size
#define nvpair_type illumos_nvpair_type
#define nvpair_type_string illumos_nvpair_type_string
#define nvpair_unpack illumos_nvpair_unpack
#define nvpair_unpack_binary illumos_nvpair_unpack_binary
#define nvpair_unpack_bool illumos_nvpair_unpack_bool
#define nvpair_unpack_bool_array illumos_nvpair_unpack_bool_array
#define nvpair_unpack_descriptor illumos_nvpair_unpack_descriptor
#define nvpair_unpack_descriptor_array illumos_nvpair_unpack_descriptor_array
#define nvpair_unpack_header illumos_nvpair_unpack_header
#define nvpair_unpack_null illumos_nvpair_unpack_null
#define nvpair_unpack_number illumos_nvpair_unpack_number
#define nvpair_unpack_number_array illumos_nvpair_unpack_number_array
#define nvpair_unpack_nvlist illumos_nvpair_unpack_nvlist
#define nvpair_unpack_nvlist_array illumos_nvpair_unpack_nvlist_array
#define nvpair_unpack_string illumos_nvpair_unpack_string
#define nvpair_unpack_string_array illumos_nvpair_unpack_string_array

#endif /* _KERNEL */

#include_next <sys/nvpair.h>

#endif
