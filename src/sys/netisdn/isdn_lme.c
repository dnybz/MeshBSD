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
 *
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

#include <netisdn/isdn_l2.h>
#include <netisdn/isdn_mbuf.h>
#include <netisdn/isdn_l2_fsm.h>


#if I4B_DEBUG
static const char *isdn_lme_error_text[] = {
	"MDL_ERR_A: rx'd unsolicited response - supervisory (F=1)",
	"MDL_ERR_B: rx'd unsolicited response - DM (F=1)",
	"MDL_ERR_C: rx'd unsolicited response - UA (F=1)",
	"MDL_ERR_D: rx'd unsolicited response - UA (F=0)",
	"MDL_ERR_E: rx'd unsolicited response - DM (F=0)",
	"MDL_ERR_F: peer initiated re-establishment - SABME",
	"MDL_ERR_G: unsuccessful transmission N200times - SABME",
	"MDL_ERR_H: unsuccessful transmission N200times - DIS",
	"MDL_ERR_I: unsuccessful transmission N200times - Status ENQ",
	"MDL_ERR_J: other error - N(R) error",
	"MDL_ERR_K: other error - rx'd FRMR response",
	"MDL_ERR_L: other error - rx'd undefined frame",
	"MDL_ERR_M: other error - receipt of I field not permitted",
	"MDL_ERR_N: other error - rx'd frame with wrong size",
	"MDL_ERR_O: other error - N201 error",
	"MDL_ERR_MAX: isdn_lme_error_ind called with wrong parameter!!!"
};
#endif

/*---------------------------------------------------------------------------*
 *	mdl assign indication handler
 *---------------------------------------------------------------------------*/
void
isdn_lme_assign_ind(struct isdn_softc *sc)
{
	NDBGL2(L2_PRIM, "isdnif %d", sc->sc_ifp->if_index);

	if (sc->sc_l2.l2_tei_valid == TEI_VALID) {
		sc->sc_l2.l2_T202_fn = (void(*)(void*))isdn_l2_verify_tei;
		sc->sc_l2.l2_N202 = N202DEF;
		isdn_l2_verify_tei(l2sc);
	} else {
		sc->sc_l2.l2_T202_fn = (void(*)(void*))isdn_l2_assign_tei;
		sc->sc_l2.l2_N202 = N202DEF;
		isdn_l2_assign_tei(l2sc);
	}
}

/*---------------------------------------------------------------------------*
 *	isdn_lme_error_ind handler (Q.921 01/94 pp 156)
 *---------------------------------------------------------------------------*/
void
isdn_lme_error_ind(struct isdn_softc *sc, const char *where, int error)
{

	if (error > MDL_ERR_MAX)
		error = MDL_ERR_MAX;

	NDBGL2(L2_ERROR, "isdnif = %d, location = %s",
	    sc->sc_ifp->if_index, where);
	NDBGL2(L2_ERROR, "error = %s", isdn_lme_error_text[error]);

	switch (error) {
	case MDL_ERR_A:
	case MDL_ERR_B:
		break;
	case MDL_ERR_C:
	case MDL_ERR_D:
		isdn_l2_verify_tei(l2sc);
		break;
	case MDL_ERR_E:
	case MDL_ERR_F:
		break;
	case MDL_ERR_G:
	case MDL_ERR_H:
		isdn_l2_verify_tei(l2sc);
		break;
	case MDL_ERR_I:
	case MDL_ERR_J:
	case MDL_ERR_K:
	case MDL_ERR_L:
	case MDL_ERR_M:
	case MDL_ERR_N:
	case MDL_ERR_O:
		break;
	default:
		break;
	}
}
