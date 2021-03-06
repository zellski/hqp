/**
 * @file Hqp_DocpStub.h
 *   client stub for Hqp_Docp interface
 *
 * rf, 10/31/00
 *
 */

/*
    Copyright (C) 1997--2009  Ruediger Franke

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

#ifndef Hqp_DocpStub_H
#define Hqp_DocpStub_H

#include <Hqp.h>

/**
 * Stub for accessing DOCP (discrete-time optimal control problem)
 * interface of Hqp.
 */
class Hqp_DocpStub {

private:
  Hqp_DocpHandle _handle;
  int _k0, _kf;		// store back result of setup_horizon

  /**
   *  Catch errors from model evaluation and signal them with
   *  infinite objective function. This signaling is useful for line search.
   */
  bool _catch_model_errors;

  /**
   * @name Static callback methods 
   */
  //@{
  static void setup_horizon(void *clientdata, int &k0, int &kf);

  static void setup_vars(void *clientdata, int k,
                         VECP x, VECP x_min, VECP x_max, IVECP x_int,
                         VECP u, VECP u_min, VECP u_max, IVECP u_int,
                         VECP c, VECP c_min, VECP c_max);

  static void setup_struct(void *clientdata,
                           int k, const VECP x, const VECP u,
                           MATP fx, MATP fu, IVECP f_lin,
                           VECP f0x, VECP f0u, int &f0_lin,
                           MATP cx, MATP cu, IVECP c_lin,
                           MATP Lxx, MATP Luu, MATP Lxu);

  static void init_simulation(void *clientdata,
                              int k, VECP x, VECP u);

  static void update_vals(void *clientdata,
                          int k, const VECP x, const VECP u,
                          VECP f, Real &f0, VECP c);

  static void update_stage(void *clientdata,
                           int k, const VECP x, const VECP u,
                           VECP f, Real &f0, VECP c,
                           MATP fx, MATP fu, VECP f0x, VECP f0u,
                           MATP cx, MATP cu,
                           const VECP rf, const VECP rc,
                           MATP Lxx, MATP Luu, MATP Lxu);
  //@}

public:

  Hqp_DocpStub();
  virtual ~Hqp_DocpStub();

  /**
   * @name Interface to be implemented by derived classes
   */
  //@{
  /** Optional: setup optimization horizon [k0,kf] */
  virtual void setup_horizon(int &k0, int &kf);

  /** Required: setup variables for one stage k */
  virtual void setup_vars(int k,
			  VECP x, VECP x_min, VECP x_max, IVECP x_int,
			  VECP u, VECP u_min, VECP u_max, IVECP u_int,
			  VECP c, VECP c_min, VECP c_max) = 0;

  /** Optional: setup structure of Jacobians and Hessians for one stage k */
  virtual void setup_struct(int k, const VECP x, const VECP u,
			    MATP fx, MATP fu, IVECP f_lin,
			    VECP f0x, VECP f0u, int &f0_lin,
			    MATP cx, MATP cu, IVECP c_lin,
			    MATP Lxx, MATP Luu, MATP Lxu) {}

  /** Optional: override values used during simulation of initial guess
      for one stage k */
  virtual void init_simulation(int k, VECP x, VECP u) {}

  /** Required: evaluate system equations f, objective f0, and constraints c
      for one stage k */
  virtual void update_vals(int k, const VECP x, const VECP u,
			   VECP f, Real &f0, VECP c) = 0;

  /** Optional: evaluate equations, Jacobians and Hessians for one stage k */
  virtual void update_stage(int k, const VECP x, const VECP u,
			    VECP f, Real &f0, VECP c,
			    MATP fx, MATP fu, VECP f0x, VECP f0u,
			    MATP cx, MATP cu,
			    const VECP rf, const VECP rc,
			    MATP Lxx, MATP Luu, MATP Lxu)
  {
    // call default implementation provided by Hqp_Docp
    Hqp_Docp_update_stage(_handle, k, x, u, f, f0, c,
			  fx, fu, f0x, f0u, cx, cu,
			  rf, rc, Lxx, Luu, Lxu);
  }
  //@}

  /** allocate variables from implementation of setup_vars */
  void	alloc_vars(VECP v, VECP v_min, VECP v_max, IVECP v_int, int n);
};  


#endif
