/**
 * @file Hqp.h
 *   declaration of external API for HQP (Huge Quadratic Programming)
 *
 * rf, 5/28/94
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

#ifndef Hqp_H
#define Hqp_H

/* include type declarations */
#include <Meschach.h>

/** define HQP_API when compiling a Dynamic Link Library (DLL) */
#ifndef HQP_API
#define HQP_API 
#endif

/**
 * Hold pointers to functions provided by an application using HQP.
 */
class Hqp_DocpSpec {
public:
  /** Obligatory: setup discrete control horizon. */
  void (*setup_horizon)(void *clientdata, int &k0, int &kf);

  /** Obligatory: setup variables per control interval. */
  void (*setup_vars)(void *clientdata, int k,
		     VECP x, VECP x_min, VECP x_max, IVECP x_int,
		     VECP u, VECP u_min, VECP u_max, IVECP u_int,
		     VECP c, VECP c_min, VECP c_max);

  /** Optional: setup sparse structure and mark linear constraints
      per control interval. */
  void (*setup_struct)(void *clientdata, int k,
		       const VECP x, const VECP u,
		       MATP fx, MATP fu, IVECP f_lin,
		       VECP f0x, VECP f0u, int &f0_lin,
		       MATP cx, MATP cu, IVECP c_lin,
		       MATP Lxx, MATP Luu, MATP Lxu);

  /** Optional: initialize variables via initial-value simulation. */
  void (*init_simulation)(void *clientdata, int k, VECP x, VECP u);

  /** Obligatory: update objective and constraints per control interval. */
  void (*update_vals)(void *clientdata, int k,
		      const VECP x, const VECP u,
		      VECP f, Real &f0, VECP c);

  /** Optional: update objective, constraints and derivatives
      per control interval. */
  void (*update_stage)(void *clientdata, int k,
		       const VECP x, const VECP u,
		       VECP f, Real &f0, VECP c,
		       MATP fx, MATP fu,
		       VECP f0x, VECP f0u,
		       MATP cx, MATP cu,
		       const VECP rf, const VECP rc,
		       MATP Lxx, MATP Luu, MATP Lxu);

  /** Constructor initializes all pointers with NULL. */
  Hqp_DocpSpec() {
    setup_horizon = NULL;
    setup_vars = NULL;
    setup_struct = NULL;
    init_simulation = NULL;
    update_vals = NULL;
    update_stage = NULL;
  }
};

/* forward delcaration of Hqp_Docp implementation */
class Hqp_Docp;

/** Handle to address Hqp_Docp implemenation */
typedef Hqp_Docp *Hqp_DocpHandle;

extern "C" {
  /** Create an instance of Hqp_Docp and return handle. */
  HQP_API Hqp_DocpHandle
  Hqp_Docp_create(Hqp_DocpSpec &spec, void *clientdata);

  /** Destroy an instance of Hqp_Docp */
  HQP_API void
  Hqp_Docp_destroy(Hqp_DocpHandle handle);

  /** Service routine for allocating vectors. */
  HQP_API void
  Hqp_Docp_alloc_vars(Hqp_DocpHandle handle,
		      VECP v, VECP v_min, VECP v_max, IVECP v_int, int n);

  /** Default implementation for updating gradients.
   *  It can be used e.g. to check against own derivatives. */
  HQP_API void
  Hqp_Docp_update_stage(Hqp_DocpHandle handle, int k,
			const VECP x, const VECP u,
			VECP f, Real &f0, VECP c,
			MATP fx, MATP fu,
			VECP f0x, VECP f0u,
			MATP cx, MATP cu,
			const VECP rf, const VECP rc,
			MATP Lxx, MATP Luu, MATP Lxu);
}


#endif
