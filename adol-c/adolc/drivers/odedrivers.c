/*---------------------------------------------------------------------------- 
 ADOL-C -- Automatic Differentiation by Overloading in C++
 File:     drivers/odedrivers.c
 Revision: $Id: odedrivers.c,v 1.1 2004/10/13 14:18:12 e_arnold Exp $
 Contents: Easy to use drivers for ordinary differential equations (ODE)
           (with C and C++ callable interfaces including Fortran 
            callable versions).

 Copyright (c) 2004
               Technical University Dresden
               Department of Mathematics
               Institute of Scientific Computing
  
 This file is part of ADOL-C. This software is provided under the terms of
 the Common Public License. Any use, reproduction, or distribution of the
 software constitutes recipient's acceptance of the terms of this license.
 See the accompanying copy of the Common Public License for more details.

 History:
          20040416 kowarz: adapted to configure - make - make install
          19981201 olvo:   newly created from driversc.c
 
----------------------------------------------------------------------------*/
#include "../drivers/odedrivers.h"
#include "../interfaces.h"
#include "../adalloc.h"

#include <math.h>
#include <malloc.h>

BEGIN_C_DECLS

/****************************************************************************/
/*                                                         DRIVERS FOR ODEs */

/*--------------------------------------------------------------------------*/
/*                                                                  forodec */
/* forodec(tag, n, tau, dold, dnew, X[n][d+1])                              */
int forodec(short tag,    /* tape identifier */
	    int n,        /* space dimension */
	    double tau,   /* scaling defaults to 1.0 */
	    int dol,      /* previous degree defaults to zero */
	    int deg,      /* New degree of consistency        */
            double** Y)   /* Taylor series */
{
  /*********************************************************************
    This is assumed to be the autonomous case.
    Here we are just going around computing the vectors 
    y[][j] for  dol < j <= deg
    by successive calls to forward that works on the tape identified
    by tag. This tape (array of file) must obviously have been
    generated by a the execution of an active section between
    trace_on and trace_off with n independent and n dependent variables
    y must have been set up as  pointer to an array of n pointers
    to double arrays containing at least deg+1 components.
    The scaling by tau is sometimes necessary to avoid overflow.
    **********************************************************************/
  
  int rc= 3;
  static int i, j, k, nax, dax;
  static double *y, *z, **Z, taut;
  if ( n > nax || deg > dax )
    {
      if (nax) {free((char*) *Z); free((char*) Z);
               free((char*) z);free((char*) y);}
      Z = myalloc2(n,deg);
      z = myalloc1(n);
      y = myalloc1(n);
      nax = n;
      dax = deg;
    }
  
  for (i=0;i<n;i++)
    {
      y[i] = Y[i][0];
      /*printf("y[%i] = %f\n",i,y[i]);*/
      for (k=0;k<deg;k++)
        {
          Y[i][k] = Y[i][k+1];
          /*printf("Y[%i][%i] = %f\n",i,k,Y[i][k]);*/
        }
    } 

  /******  Here we get  going    ********/
  if (dol == 0)
    {
      j = dol;                          /* j = 0 */
      k = (deg)*(j == deg-1 ) ;         /* keep death values in prepration */
      mindec(rc,zos_forward(tag,n,n,k, y, z));   
                                        /* for  reverse called by jacode   */
      if( rc < 0)
        return rc; 
      taut = tau/(1+j);                 /* only the last time through.     */
      for (i=0;i<n;i++)
	Y[i][j] = taut*z[i];
      dol++;                            /* !!! */
    }
  for (j=dol;j<deg;j++)
    {
      k = (deg)*(j == deg-1 ) ;         /* keep death values in prepration */
      mindec(rc,hos_forward(tag,n,n,j,k, y, Y, z, Z));      
                                        /* for  reverse called by jacode   */
      if( rc < 0)
        return rc; 
      taut = tau/(1+j);             /* only the last time through.     */
      for (i=0;i<n;i++)
	Y[i][j] = taut*Z[i][j-1];
    }
  /******  Done                  ********/

  for (i=0;i<n;i++)
    {
      for (k=deg;k>0;k--)
        {
          Y[i][k] = Y[i][k-1];
          /*printf("Y[%i][%i] = %f\n",i,k,Y[i][k]);*/
        }
      Y[i][0] = y[i];
      /*printf("Y[%i][0] = %f\n",i,Y[i][0]);*/
    }

 return rc;
}

/*--------------------------------------------------------------------------*/
/*                                                                  accodec */
/* accodec(n, tau, d, Z[n][n][d+1], B[n][n][d+1], nz[n][n])                 */
void accodec(int n,              /* space dimension */
             double tau,         /* scaling defaults to 1.0 */
             int deg,            /* highest degree          */
	     double*** A,        /* input tensor of "partial" Jacobians */
             double*** B,        /* output tensor of "total" Jacobians  */
	     short** nonzero )   /* optional sparsity characterization  */
{
/*
    The purpose of this subroutine is to compute the total derivatives
               B[i=0...n-1][j=0...n-1][k=0...deg].
    The matrix obtained for fixed k represents the Jacobian of the
    (k+1)-st Taylor coefficient vector with respect to the base point of
    the ODE, i.e., the 0-th coefficient vector. The input array
	       A[i=0...n-1][j=0...n-1][k=0...deg]
    has exactly the same format, except that it-s k-th matrix slice
    represents a partial derivative in that the indirect dependence
    of the k-th coefficient vector on the base point via the (k-1)-st
    and other lower Taylor coeffcients has not been taken into account.
    The B's are compute from the A's by the chainrule with the parameter
    tau thrown in for scaling. The calculation is performed so that
    A may directly be overwritten by B i.e. their pointers arguments may
    coincide to save storage.
	   Sparsity is used so far only to reduce the operations count 
    but not to save space. In general we expect that for each given pair
    (i,j) the entries A[i][j][k=0...] are nonzero either for all k, or for no k,
    or for k=0 only. 
	   On entry the optional short array nonzero may be used to identify
    all entries of the A[.][.][k] that are potentially nonzero, i.e. 

            nonzero[i][j] <= 0  implies   A[i][j][k] = 0 for all k 
	    nonzero[i][j] = 1   implies   A[i][j][k] = 0 for all k > 0 .

    In other words we only allow the sparsity of the matrices A[.][.][k]
    to be increasing in that A[.][.][1] is possibly sparser than A[.][.][0]
    and all subseqent A[.][.][k] with k > 0 have the same sparsity pattern.
    That is the typical situation since A[.][.][k] is the k-th
    Taylor coefficient in the time expansion of the Jacobian of the
    right hand side. The entries of this square matrix tend to be either
    constant or trancendental functions of time.
	   The matrices B_k = B[.][.][k] are obtained from the A_k = A[.][.][k]
    by the recurrence
                        tau   /        k                  \
	         B_k = ----- |  A_k + SUM A_{j-1} B_{k-j}  |
	                k+1   \       j=1                 /

    Assuming that the diagonal entries A[i][i][0] are structurally nonzero
    we find that the matrices B[.][.][k=1..] can only lose sparsity
    as k increase. Therfore, we can redefine the nonpositive values 
    nonzero[i][j] so that on exit

	    k <= -nonzero[i][j]  implies     B[i][j][k] = 0 

    which is trivially satisfied for all positive values of nonzero[i][j].
    Due to the increasing sparsity of the A_i and the decreasing sparsity
    of the B_i the first product in the sum of the RHS above determines the
    sparsity pattern of the resulting B_k. Hence the optimal values of
    the nonzero[i][j] depend only on the sparsity pattern of A_0. More
    specifically, all positive -nonzero[i][j] represent the length of the
    shortest directed path connecting nodes j and i in the incidence graph 
    of A_0. 
*/    

  int i,j,k,m,p,nzip,nzpj,isum;
  double *Aip, *Bpj, scale, sum;
  for (k=0;k<=deg;k++)   /* Lets calculate B_k */
    {
      scale = tau/(1.0+k);
      if(nonzero)
	{
	  for (i=0;i<n;i++)
	    for(j=0;j<n;j++)
	      if(k   < -nonzero[i][j]) 
		B[i][j][k] = 0.0;
	      else   
		{
		  sum = A[i][j][k];
		  isum = (nonzero[i][j] >  0);
		  for (p=0;p<n;p++)
		    {
		      nzip = nonzero[i][p];
		      nzpj = nonzero[p][j];
		      if(nzpj > 0) nzpj = 0;
		      if(nzip > 0 && k > -nzpj )  /*otherwise all terms vanish*/
			{
			  Aip = A[i][p];
			  Bpj = B[p][j]+k-1;
			  sum += *Aip*(*Bpj);
			  isum =1;
			  if(nzip > 1 )   /* the A[i][p][m>0] may be nonzero*/
			    for(m=k-1; m>-nzpj;m--)
			      sum += *(++Aip)*(*(--Bpj));
			}
		    }
		  if(isum)         /* we found something nonzero after all*/
		    B[i][j][k] = sum*scale;
		  else
		    {B[i][j][k]= 0;
		     nonzero[i][j]--;
		   }
		}
	}
      else
	{
	  for (i=0;i<n;i++)
	    for(j=0;j<n;j++)
	      {
		sum = A[i][j][k];
		for (p=0;p<n;p++)
		  {
		    Aip = A[i][p];
		    Bpj = B[p][j]+k-1;
		    for(m=k; m>0 ;m--)
		      sum += *(Aip++)*(*Bpj--);
		    B[i][j][k] = sum*scale;
		  }
	      }
	}
    }
}

END_C_DECLS
