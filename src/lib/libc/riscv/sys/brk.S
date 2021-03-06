/*-
 * Copyright (c) 2015 Ruslan Bukin <br@bsdpad.com>
 * All rights reserved.
 *
 * Portions of this software were developed by SRI International and the
 * University of Cambridge Computer Laboratory under DARPA/AFRL contract
 * FA8750-10-C-0237 ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * Portions of this software were developed by the University of Cambridge
 * Computer Laboratory as part of the CTSRD Project, with support from the
 * UK Higher Education Innovation Fund (HEIF).
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <machine/asm.h>
__FBSDID("$FreeBSD: head/lib/libc/riscv/sys/brk.S 294227 2016-01-17 15:21:23Z br $");

#include "SYS.h"

	.globl	_C_LABEL(_end)

	.data
	.align	3
	.globl	_C_LABEL(minbrk)
	.type	_C_LABEL(minbrk), %object
_C_LABEL(minbrk):
	.quad	_C_LABEL(_end)

	.text
/*
 * int brk(const void *addr);
 */
ENTRY(_brk)
	WEAK_REFERENCE(_brk, brk)

	/* Load the address of minbrk */
	la	a3, minbrk
	ld	a2, 0(a3)

	/* Validate the address */
	bge	a0, a2, 1f
	/* Invalid, set it to the minimum */
	mv	a0, a2

	/* Backup the new address */
1:	mv	a4, a0

	/* Update for this value, will overwrite a0 and a1 */
	_SYSCALL(break)
	bnez	t0, cerror

	/* Store the new curbrk value */
	la	a2, curbrk
	sd	a4, 0(a2)

	/* Return success */
	li	a0, 0
	ret
END(_brk)
