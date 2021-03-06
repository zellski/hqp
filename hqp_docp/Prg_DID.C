/*
 * Prg_DID.C -- class definition
 *
 * rf, 1/29/95
 *
 * rf, 2/10/98
 *   - updated for HQP 1.3
 *
 * rf, 7/11/00
 *   - updated for HQP 1.7
 */

#include <Hqp.h>

extern Hqp_DocpHandle theHqp_DocpHandle;

/** Number of stages */
static int _kmax = 60;

/** Additional sampling for path constraint? */
static bool _cns = true;

//--------------------------------------------------------------------------
static void setup_horizon(void *clientdata, int &k0, int &kf)
{
  k0 = 0;
  kf = _kmax;
}

//--------------------------------------------------------------------------
static void setup_vars(void *clientdata, int k,
		       VECP x, VECP x_min, VECP x_max, IVECP x_int,
		       VECP u, VECP u_min, VECP u_max, IVECP u_int,
		       VECP c, VECP c_min, VECP c_max)
{
  // dimension variables
  Hqp_Docp_alloc_vars(theHqp_DocpHandle, x, x_min, x_max, x_int, 2);
  if (k < _kmax)
    Hqp_Docp_alloc_vars(theHqp_DocpHandle, u, u_min, u_max, u_int, 1);

  // initial values
  if (k == 0) {
    x[0] = 1.0;
    x[1] = 0.0;
  }
  if (k < _kmax)
    u[0] = -2.0;

  // initial state constraints
  if (k == 0) {
    x_min[0] = x_max[0] = x[0];
    x_min[1] = x_max[1] = x[1];
  }
  // path constraint
  else if (k < _kmax) {
    x_max[1] = 0.01;
  }
  // final state constraints
  else {
    x_min[0] = x_max[0] = -1.0;
    x_min[1] = x_max[1] = 0.0;
  }

  // optional additional treatment for path constraint
  if (_cns) {
    if (k < _kmax) {
      Hqp_Docp_alloc_vars(theHqp_DocpHandle, c, c_min, c_max, IVNULL, 1);
      c_max[0] = 0.01;
    }
  }
}

//--------------------------------------------------------------------------
static void update_vals(void *clientdata, int k,
			const VECP x, const VECP u,
			VECP f, Real &f0, VECP c)
{
  double dt = 1.0/_kmax;

  // update constraints and objective for given x and u
  if (k < _kmax) {
    f[0] = x[0] + u[0]*dt;
    f[1] = x[0]*dt + x[1] + u[0]*0.5*dt*dt;

    f0 = u[0] * u[0] * dt;

    if (_cns) {
      c[0] = x[1] + 0.5*dt*x[0];
    }
  }
  else
    f0 = 0.0;
}

//--------------------------------------------------------------------------
static void setup_struct(void *clientdata, int k,
			 const VECP, const VECP,
			 MATP fx, MATP fu, IVECP f_lin,
			 VECP f0x, VECP f0u, int &f0_lin,
			 MATP cx, MATP cu, IVECP c_lin,
			 MATP Lxx, MATP Luu, MATP Lxu)
{
  // Setup sparse structure of problem and initialize linear constraints.
  // Don't setup Hessian blocks in Lxx, Luu, Lxu,
  // i.e. they are assumed dense.

  double dt = 1.0/_kmax;

  f0x[0] = 0.0;
  f0x[1] = 0.0;
  if (k == _kmax) {
    f0_lin = 1;
  }

  if (k < _kmax) {
    fx[0][0] = 1.0;
    fx[0][1] = 0.0;
    fx[1][0] = dt;
    fx[1][1] = 1.0;

    fu[0][0] = dt;
    fu[1][0] = 0.5*dt*dt;

    f_lin[0] = f_lin[1] = 1;

    if (_cns) {
      cx[0][0] = 0.5*dt;
      cx[0][1] = 1.0;
      cu[0][0] = 0.0;
      c_lin[0] = 1;
    }
  }
}

//--------------------------------------------------------------------------
static void update_stage(void *clientdata, int k,
			 const VECP x, const VECP u,
			 VECP f, Real &f0, VECP c,
			 MATP fx, MATP fu,
			 VECP f0x, VECP f0u,
			 MATP cx, MATP cu,
			 const VECP rf, const VECP rc,
			 MATP Lxx, MATP Luu, MATP Lxu)
{
  // update values
  update_vals(clientdata, k, x, u, f, f0, c);

  // Update derivatives of non-linear functions.
  // Don't update Hessian blocks, as this is done by solver.

  double dt = 1.0/_kmax;

  if (k < _kmax) {
    f0u[0] = 2.0*u[0]*dt;
  }
}

//--------------------------------------------------------------------------
Hqp_DocpSpec Prg_DID_Spec()
{
  // register own callback functions to Docp interface
  Hqp_DocpSpec spec;

  // obligatory functions
  spec.setup_horizon = setup_horizon;
  spec.setup_vars = setup_vars;
  spec.update_vals = update_vals;

  // optional callback routines
  spec.setup_struct = setup_struct;
  spec.update_stage = update_stage;

  return spec;
}


//==========================================================================
