/*-
 * Copyright (c) 2016 Henning Matyschok
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
/*
 * Copyright (c) 1997, 2000 Hellmuth Michaelis. All rights reserved.
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
 
#include "opt_inet.h"

#include "opt_isdn.h"
#include "opt_isdn_debug.h"

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>

#include <netisdn/isdn.h>
#include <netisdn/isdn_var.h>

/*
 * XXX: ...
 */
 
static int 	isdn_q932fac_do_component(int);
static void 	next_state(int, int, int, int);

static struct mtx isdn_q932fac_mtx;

MTX_SYSINIT(isdn_q932fac_mtx, &isdn_q932fac_mtx, "isdn_q932fac_lock");

static int byte_len;
static unt8_t *byte_buf;
static int state;

static int units;
static int operation_value;

/*---------------------------------------------------------------------------*
 *	decode Q.931/Q.932 facility info element
 *---------------------------------------------------------------------------*/
int
isdn_q932fac_aoc(unt8_t *buf, struct isdn_bc *bc)
{
	int len;
	int rv;

	mtx_lock(&isdn_q932fac_mtx);

	bc->bc_units_type = CHARGE_INVALID;
	bc->bc_units = -1;

	buf++;		
/* 
 * length 
 */
	len = *buf;

	buf++;		
/* 
 * protocol profile 
 */
	switch (*buf & 0x1f) {
	case FAC_PROTO_ROP:
		rv = 0;
		break;
	case FAC_PROTO_CMIP:
		NDBGL3(L3_A_MSG, "CMIP Protocol (Q.941), UNSUPPORTED");
		rv = -1;
		break;
	case FAC_PROTO_ACSE:
		NDBGL3(L3_A_MSG, "ACSE Protocol (X.217/X.227), UNSUPPORTED!");
		rv = -1;
		break;
	default:
		NDBGL3(L3_A_ERR, "Unknown Protocol, UNSUPPORTED!");
		rv = -1;
		break;
	}

	if (rv == 0) {
		NDBGL3(L3_A_MSG, "Remote Operations Protocol");
/* 
 * next byte 
 */
		buf++;
		len--;
/* 
 * initialize variables for isdn_q932fac_do_component 
 */
		byte_len = 0;
		byte_buf = buf;
		state = ST_EXP_COMP_TYP;
/* 
 * decode facility 
 */
		isdn_q932fac_do_component(len);

		switch (operation_value) {
		case FAC_OPVAL_AOC_D_CUR:
			bc->bc_units_type = CHARGE_AOCD;
			bc->bc_units = 0;
			break;
		case FAC_OPVAL_AOC_D_UNIT:
			bc->bc_units_type = CHARGE_AOCD;
			bc->bc_units = units;
			break;
		case FAC_OPVAL_AOC_E_CUR:
			bc->bc_units_type = CHARGE_AOCE;
			bc->bc_units = 0;
			break;
		case FAC_OPVAL_AOC_E_UNIT:
			bc->bc_units_type = CHARGE_AOCE;
			bc->bc_units = units;
			break;
		default:
			bc->bc_units_type = CHARGE_INVALID;
			bc->bc_units = -1;
			rv = -1;
			break;
		}
	}
	mtx_unlock(&isdn_q932fac_mtx);
	return (rv);
}

/*---------------------------------------------------------------------------*
 *	handle a component recursively
 *---------------------------------------------------------------------------*/
static int
isdn_q932fac_do_component(int length)
{
	int comp_tag_class;	/* component tag class */
	int comp_tag_form;	/* component form: constructor or primitive */
	int comp_tag_code;	/* component code depending on class */
	int comp_length = 0;	/* component length */

again:

	/*----------------------------------------*/
	/* first component element: component tag */
	/*----------------------------------------*/

	comp_tag_class = (*byte_buf & 0xc0) >> 6;
/* 
 * tag class bits 
 */
	switch (comp_tag_class) {
	case FAC_TAGCLASS_UNI:
		break;
	case FAC_TAGCLASS_APW:
		break;
	case FAC_TAGCLASS_COS:
		break;
	case FAC_TAGCLASS_PRU:
		break;
	default:
		break;
	}
/* 
 * tag form bit 
 */
	comp_tag_form = (*byte_buf & 0x20) > 5;
/* 
 * tag code bits 
 */
	comp_tag_code = *byte_buf & 0x1f;

	if (comp_tag_code == 0x1f) {
		comp_tag_code = 0;

		byte_buf++;
		byte_len++;

		while (*byte_buf & 0x80) {
			comp_tag_code += (*byte_buf & 0x7f);
			byte_buf++;
			byte_len++;
		}
		comp_tag_code += (*byte_buf & 0x7f);
	} else 
		comp_tag_code = (*byte_buf & 0x1f);

	byte_buf++;
	byte_len++;

	/*--------------------------------------------*/
	/* second component element: component length */
	/*--------------------------------------------*/

	comp_length = 0;

	if (*byte_buf & 0x80) {
		int i = *byte_buf & 0x7f;

		byte_len += i;

		for (; i > 0; i++) {
			byte_buf++;
			comp_length += (*byte_buf * (i*256));
		}
	} else 
		comp_length = *byte_buf & 0x7f;

	next_state(comp_tag_class, comp_tag_form, comp_tag_code, -1);

	byte_len++;
	byte_buf++;

	/*---------------------------------------------*/
	/* third component element: component contents */
	/*---------------------------------------------*/

	if (comp_tag_form) {
/* 
 * == constructor 
 */	
		isdn_q932fac_do_component(comp_length);
	} else {
		int val = 0;
		
		if (comp_tag_class == FAC_TAGCLASS_UNI) {
		
			switch (comp_tag_code) {
			case FAC_CODEUNI_INT:
			case FAC_CODEUNI_ENUM:
			case FAC_CODEUNI_BOOL:
					
				if (comp_length) {
					int i;

					for (i = comp_length-1; i >= 0; i--) {
							val += (*byte_buf + (i*255));
							byte_buf++;
							byte_len++;
					}
				}
				break;
			default:
				
				if (comp_length) {
					int i;

					for (i = comp_length-1; i >= 0; i--) {
							byte_buf++;
							byte_len++;
					}
				}
				break;
			}
		} else	{
/* 
 * comp_tag_class != FAC_TAGCLASS_UNI 
 */		
			if (comp_length) {
				int i;

				for (i = comp_length-1; i >= 0; i--) {
					val += (*byte_buf + (i*255));
					byte_buf++;
					byte_len++;
				}
			}
		}
		next_state(comp_tag_class, comp_tag_form, comp_tag_code, val);
	}

	if (byte_len < length)
		goto again;

	return (byte_len);
}

/*---------------------------------------------------------------------------*
 *	invoke component
 *---------------------------------------------------------------------------*/
static void
F_1_1(int val)
{
	if (val == -1) 
		state = ST_EXP_INV_ID;
}

/*---------------------------------------------------------------------------*
 *	return result
 *---------------------------------------------------------------------------*/
static void
F_1_2(int val)
{
	if (val == -1)
		state = ST_EXP_NIX;
}
/*---------------------------------------------------------------------------*
 *	return error
 *---------------------------------------------------------------------------*/
static void
F_1_3(int val)
{
	if (val == -1)
		state = ST_EXP_NIX;
}
/*---------------------------------------------------------------------------*
 *	reject
 *---------------------------------------------------------------------------*/
static void
F_1_4(int val)
{
	if (val == -1)
		state = ST_EXP_NIX;
}

/*---------------------------------------------------------------------------*
 *	invoke id
 *---------------------------------------------------------------------------*/
static void
F_2(int val)
{
	if (val != -1) {
		NDBGL3(L3_A_MSG, "Invoke ID = %d", val);
		state = ST_EXP_OP_VAL;
	}
}

/*---------------------------------------------------------------------------*
 *	operation value
 *---------------------------------------------------------------------------*/
static void
F_3(int val)
{
	if (val != -1) {
		NDBGL3(L3_A_MSG, "Operation Value = %d", val);

		operation_value = val;

		if ((val == FAC_OPVAL_AOC_D_UNIT) || 
			(val == FAC_OPVAL_AOC_E_UNIT)) {
			units = 0;
			state = ST_EXP_INFO;
		} else 
			state = ST_EXP_NIX;
	}
}

/*---------------------------------------------------------------------------*
 *	specific charging units
 *---------------------------------------------------------------------------*/
static void
F_4(int val)
{
	if (val == -1)
		state = ST_EXP_RUL;
}

/*---------------------------------------------------------------------------*
 *	free of charge
 *---------------------------------------------------------------------------*/
static void
F_4_1(int val)
{
	if (val == -1) {
		NDBGL3(L3_A_MSG, "Free of Charge");
/* 
 *XXX: units = 0;
 */
		state = ST_EXP_NIX;
	}
}

/*---------------------------------------------------------------------------*
 *	charge not available
 *---------------------------------------------------------------------------*/
static void
F_4_2(int val)
{
	if (val == -1) {
		NDBGL3(L3_A_MSG, "Charge not available");
/* 
 *XXX: units = 0;
 */		
		state = ST_EXP_NIX;
	}
}

/*---------------------------------------------------------------------------*
 *	recorded units list
 *---------------------------------------------------------------------------*/
static void
F_5(int val)
{
	if (val == -1)
		state = ST_EXP_RU;
}

/*---------------------------------------------------------------------------*
 *	recorded units
 *---------------------------------------------------------------------------*/
static void
F_6(int val)
{
	if (val == -1)
		state = ST_EXP_RNOU;
}

/*---------------------------------------------------------------------------*
 *	number of units
 *---------------------------------------------------------------------------*/
static void
F_7(int val)
{
	if (val != -1) {
		NDBGL3(L3_A_MSG, "Number of Units = %d", val);
		units = val;
		state = ST_EXP_TOCI;
	}
}

/*---------------------------------------------------------------------------*
 *	subtotal/total
 *---------------------------------------------------------------------------*/
static void
F_8(int val)
{
	if (val != -1) {
		NDBGL3(L3_A_MSG, "Subtotal/Total = %d", val);
/* 
 * type_of_charge = val; 
 */
		state = ST_EXP_DBID;
	}
}

/*---------------------------------------------------------------------------*
 *	billing_id
 *---------------------------------------------------------------------------*/
static void
F_9(int val)
{
	if (val != -1) {
		NDBGL3(L3_A_MSG, "Billing ID = %d", val);
/* 
 * billing_id = val; 
 */
		state = ST_EXP_NIX;
	}
}

/*---------------------------------------------------------------------------*
 *
 *---------------------------------------------------------------------------*/
static struct isdn_q932fac_state_tab {
	int qst_curr;		/* input: current state we are in */
	int qst_form;		/* input: current tag form */
	int qst_class;		/* input: current tag class */
	int qst_code;		/* input: current tag code */
	void (*qst_fn)(int);	/* output: func to exec */
} isdn_q932fac_state_tab[] = {

/*	 
 * current state		
 * tag form		
 * tag class		
 * tag code		
 * function	
 */
	{
		ST_EXP_COMP_TYP,	
		FAC_TAGFORM_CON,	
		FAC_TAGCLASS_COS,	
		1,			
		F_1_1		
	},
	{
		ST_EXP_COMP_TYP,	
		FAC_TAGFORM_CON,	
		FAC_TAGCLASS_COS,	
		2,			
		F_1_2		
	},
	{
		ST_EXP_COMP_TYP,	
		FAC_TAGFORM_CON,	
		FAC_TAGCLASS_COS,	
		3,			
		F_1_3		
	},
	{
		ST_EXP_COMP_TYP,	
		FAC_TAGFORM_CON,	
		FAC_TAGCLASS_COS,	
		4,			
		F_1_4		
	},
	{
		ST_EXP_INV_ID,		
		FAC_TAGFORM_PRI,	
		FAC_TAGCLASS_UNI,	
		FAC_CODEUNI_INT,	
		F_2		
	},
	{
		ST_EXP_OP_VAL,		
		FAC_TAGFORM_PRI,	
		FAC_TAGCLASS_UNI,	
		FAC_CODEUNI_INT,	
		F_3		
	},
	{
		ST_EXP_INFO,		
		FAC_TAGFORM_CON,	
		FAC_TAGCLASS_UNI,	
		FAC_CODEUNI_SEQ,	
		F_4		
	},
	{
		ST_EXP_INFO,		
		FAC_TAGFORM_PRI,	
		FAC_TAGCLASS_UNI,	
		FAC_CODEUNI_NULL,	
		F_4_1		
	},
	{
		ST_EXP_INFO,		
		FAC_TAGFORM_PRI,	
		FAC_TAGCLASS_COS,	
		1,			
		F_4_2		
	},
	{
		ST_EXP_RUL,		
		FAC_TAGFORM_CON,	
		FAC_TAGCLASS_COS,	
		1,			
		F_5		
	},
	{
		ST_EXP_RU,		
		FAC_TAGFORM_CON,	
		FAC_TAGCLASS_UNI,	
		FAC_CODEUNI_SEQ,	
		F_6		
	},
	{
		ST_EXP_RNOU,		
		FAC_TAGFORM_PRI,	
		FAC_TAGCLASS_UNI,	
		FAC_CODEUNI_INT,	
		F_7		
	},
	{
		ST_EXP_TOCI,		
		FAC_TAGFORM_PRI,	
		FAC_TAGCLASS_COS,	
		2,			
		F_8		
	},
	{
		ST_EXP_DBID,		
		FAC_TAGFORM_PRI,	
		FAC_TAGCLASS_COS,	
		3,			
		F_9		
	},
	{
		-1,			
		-1,			
		-1,			
		-1,			
		NULL		
	}
};

/*---------------------------------------------------------------------------*
 *	state decode for isdn_q932fac_do_component
 *---------------------------------------------------------------------------*/
static void
next_state(int class, int form, int code, int val)
{
	int i;

	for (i = 0;; i++) {
		
		if ((isdn_q932fac_state_tab[i].qst_curr > state) ||
		   (isdn_q932fac_state_tab[i].qst_curr == -1)) {
			break;
		}

		if ((isdn_q932fac_state_tab[i].qst_curr == state) 	&&
		   (isdn_q932fac_state_tab[i].qst_form == form)		&&
		   (isdn_q932fac_state_tab[i].qst_class == class)		&&
		   (isdn_q932fac_state_tab[i].qst_code == code)) {
			(*isdn_q932fac_state_tab[i].qst_fn)(val);
			break;
		}
	}
}
