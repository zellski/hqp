/*
 * Hqp.C -- implementation of Hqp API
 *
 * rf, 11/01/00
 */

/*
    Copyright (C) 1994--2009  Ruediger Franke

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; 
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library (file COPYING.LIB);
    if not, write to the Free Software Foundation, Inc.,
    59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <If.h>

#include "Hqp.h"
#include "Hqp_DocpWrapper.h"

/* declare things necessary for automatic initialization of Hqp */
class Hqp_SqpSolver;
extern Hqp_SqpSolver *theSqpSolver;
extern "C" int Hqp_Init(Tcl_Interp *);

//-------------------------------------------------------------------------
extern "C" Hqp_DocpHandle 
Hqp_Docp_create(Hqp_DocpSpec &spec, void *clientdata)
{
  // assert that mandatory functions are provided
  assert(spec.setup_horizon != NULL);
  assert(spec.setup_vars != NULL);
  assert(spec.update_vals != NULL);

  // check that If was initialized
  if (If_Interp() == NULL)
    m_error(E_UNKNOWN, "Hqp_Docp_create: If_Interp not initialized");

  // check for Hqp
  if (!theSqpSolver && Hqp_Init(If_Interp()) != TCL_OK)
    m_error(E_UNKNOWN, "Hqp_Docp_create: failed calling Hqp_Init");

  // create a Hqp_Docp
  return new Hqp_DocpWrapper(spec, clientdata);
}

//-------------------------------------------------------------------------
extern "C" void
Hqp_Docp_destroy(Hqp_DocpHandle handle)
{
  delete handle;
}

//-------------------------------------------------------------------------
extern "C" void
Hqp_Docp_alloc_vars(Hqp_DocpHandle handle,
		    VECP v, VECP v_min, VECP v_max, IVECP v_int, int n)
{
  handle->alloc_vars(v, v_min, v_max, v_int, n);
}

//-------------------------------------------------------------------------
extern "C" void
Hqp_Docp_update_stage(Hqp_DocpHandle handle, int k,
		      const VECP x, const VECP u,
		      VECP f, Real &f0, VECP c,
		      MATP fx, MATP fu,
		      VECP f0x, VECP f0u,
		      MATP cx, MATP cu,
		      const VECP rf, const VECP rc,
		      MATP Lxx, MATP Luu, MATP Lxu)
{
  handle->Hqp_Docp::update_stage(k, x, u, f, f0, c,
				 fx, fu, f0x, f0u, cx, cu,
				 rf, rc, Lxx, Luu, Lxu);
}

//=========================================================================
