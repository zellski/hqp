#define _HO_REV_C_
#define _ADOLC_SRC_
/*
   -----------------------------------------------------------------
   File ho_rev.c of ADOL-C version 1.8.7           as of Mar/10/2000
   -----------------------------------------------------------------
   Contains the routines :
   hos_reverse (higher-order-scalar reverse mode) : define _HOS_
   hov_reverse (higher-order-vector reverse mode) : define _HOV_

   Uses the preprocessor to compile the 2 different object files

   Last changed :
      20000310 olvo   removed trigraphs stuff
        981130 olvo   last check (includes ...)
        980929 olvo   allow reflexive operations for
                          - vector operations: eq_mult_av_a,
                            mult_a_av, div_av_a
        980925 olvo   allow reflexive operations for
                          - cond_assign, cond_assign_s
                            (changed code completely)
        980924 olvo   (1) Lots of small changes (Atemp, At --> Aqo)
                      (2) new macros AQO*
                      (3) deleted all int_* opcodes
        980923 olvo   allow reflexive operations for
                          - min_op
        980922 olvo   (1) allow reflexive operations for
                          - div_d_a, div_a_a
        980921 olvo   (1) changed save-order in sin/cos
                      (2) allow reflexive operations for
                          - pow
                      (3) new macros VEC_COMPUTED_*
        980820 olvo   new comparison strategy 
        980721 olvo   write of taylors in subscript and
                      subscript_m   
        980714 olvo   some error elimination
                      op-code mult_av_a removed  
        980713 olvo   debugging & optimizing
        980710 olvo   sin/cos writes 2 taylors
        980709 olvo   new operation code: neg_sign_a
                                          pos_sign_a
        980706 olvo   new operation code: int_adb_d_one
                                          int_adb_d_zero
        980703 olvo   new operation code: assign_d_one
                                          assign_d_zero
        980626 olvo   vector operations & subscripts
        980623 mitev/olvo revision + stock stuff 
        980622 olvo   debugging & rearrangements
        980616 olvo   griewank's idea II
        980615 olvo   griewank's idea
        980612 mitev  

   -----------------------------------------------------------------
*/

/* 
   -----------------------------------------------------------------
  There are four basic versions of the procedure `reverse', which
  are optimized for the cases of scalar or vector reverse sweeps
  with first or higher derivatives, respectively. In the calling
  sequence this distinction is apparent from the type of the
  parameters `lagrange' and `results'. The former may be left out
  and the integer parameters `depen', `indep', `degre', and `nrows'
  must be set or default according to the following matrix of
  calling cases. 

           no lagrange         double* lagrange     double** lagrange

double*   gradient of scalar   weight vector times    infeasible 
results   valued function      Jacobian product       combination

          ( depen = 1 ,         ( depen > 0 ,         
	    degre = 0 ,           degre = 0 ,              ------
	    nrows = 1 )           nrows = 1 )

double**  Jacobian of vector   weight vector times     weight matrix
results   valued function      Taylor-Jacobians        times Jacobian
           
	  ( 0 < depen           ( depen > 0 ,          ( depen > 0 ,
	      = nrows ,           degre > 0 ,            degre = 0 ,
	    degre = 0 )           nrows = 1 )            nrows > 0 )

double*** full family of         ------------          weigth matrix x
results   Taylor-Jacobians       ------------          Taylor Jacobians
   -----------------------------------------------------------------
*/ 


/****************************************************************************/
/*                                                                   MACROS */
#undef _ADOLC_VECTOR_
#undef _HI_ORDER_

/*--------------------------------------------------------------------------*/
#ifdef _HOS_
#define GENERATED_FILENAME "hos_reverse"         

#define _HI_ORDER_

#define RESULTS(l,indexi,k) results[indexi][k]
#define LAGRANGE(l,indexd)  lagrange[indexd] 

#define HOV_INC(T,degree)  {}

/*--------------------------------------------------------------------------*/
#elif _HOV_
#define GENERATED_FILENAME "hov_reverse"         

#define _ADOLC_VECTOR_
#define _HI_ORDER_

#define RESULTS(l,indexi,k) results[l][indexi][k]
#define LAGRANGE(l,indexd)  lagrange[l][indexd] 

#define IF_HOV_  
#define ENDIF_HOV_

#define HOV_INC(T,degree) T += degree; 

#else
#error Error ! Define [_HOS_ | _HOV_] 
#endif

/*--------------------------------------------------------------------------*/
/*                                                     access to variables  */

#ifdef _FOS_
#define ARES       *Ares
#define AARG       *Aarg
#define AARG1      *Aarg1
#define AARG2      *Aarg2
#define AQO        *Aqo

#define ARES_INC   *Ares
#define AARG_INC   *Aarg
#define AARG1_INC  *Aarg1
#define AARG2_INC  *Aarg2
#define AQO_INC    *Aqo

#define ARES_INC_O  Ares
#define AARG_INC_O  Aarg
#define AARG1_INC_O Aarg1
#define AARG2_INC_O Aarg2
#define AQO_INC_O   Aqo

#define ASSIGN_A(a,b)  a = &b;

#else  /* _FOV_, _HOS_, _HOV_ */
#define ARES       *Ares
#define AARG       *Aarg
#define AARG1      *Aarg1
#define AARG2      *Aarg2
#define AQO        *Aqo

#define ARES_INC   *Ares++
#define AARG_INC   *Aarg++
#define AARG1_INC  *Aarg1++
#define AARG2_INC  *Aarg2++
#define AQO_INC    *Aqo++

#define ARES_INC_O  Ares++
#define AARG_INC_O  Aarg++
#define AARG1_INC_O Aarg1++
#define AARG2_INC_O Aarg2++
#define AQO_INC_O   Aqo++

#define ASSIGN_A(a,b)  a = b;
#endif

#ifdef _HI_ORDER_

#define TRES      *Tres
#define TARG      *Targ
#define TARG1     *Targ1
#define TARG2     *Targ2

#define ASSIGN_T(a,b)  a = b;
#else

#define TRES       T[res]
#define TARG       T[arg]
#define TARG1      T[arg1]
#define TARG2      T[arg2]

#define ASSIGN_T(a,b)
#endif

/*--------------------------------------------------------------------------*/
/*                                                              loop stuff  */
#ifdef _ADOLC_VECTOR_
#define FOR_0_LE_l_LT_p for (l=0; l<p; l++)  
#define FOR_p_GT_l_GE_0 for (l=p-1; l>=0; l--)  
#else
#define FOR_0_LE_l_LT_p 
#define FOR_p_GT_l_GE_0  
#endif
 
#ifdef _HI_ORDER_
#define FOR_0_LE_i_LT_k for (i=0; i<k; i++) 
#define FOR_k_GT_i_GE_0 for (i=k-1; i>=0; i--) 
#else
#define FOR_0_LE_i_LT_k  
#define FOR_k_GT_i_GE_0  
#endif

#ifdef _HOV_
#define FOR_0_LE_l_LT_pk1 for (l=0; l<pk1; l++)  
#elif _FOV_
#define FOR_0_LE_l_LT_pk1 for (l=0; l<p; l++)  
#elif _HOS_
#define FOR_0_LE_l_LT_pk1 for (l=0; l<k1; l++)  
#else
#define FOR_0_LE_l_LT_pk1
#endif

/*--------------------------------------------------------------------------*/
  /*                                                         VEC_COMPUTED_* */
#ifdef _ADOLC_VECTOR
#define VEC_COMPUTED_INIT   computed = 0;
#define VEC_COMPUTED_CHECK  if (computed == 0) { computed = 1;
#define VEC_COMPUTED_END    }
#else
#define VEC_COMPUTED_INIT 
#define VEC_COMPUTED_CHECK 
#define VEC_COMPUTED_END 
#endif

/*--------------------------------------------------------------------------*/
/*                                                             other macros */
#define MAXINC(a,b) if ((a) < (b)) (a) = (b)
#define MINDEC(a,b) if ((a) > (b)) (a) = (b)

/* END Macros */


/****************************************************************************/
/*                                                       NECESSARY INCLUDES */
#include "dvlparms.h" /* Developers Parameters */
#include "usrparms.h" /* Users Parameters */
#include "interfaces.h"
#include "adalloc.h"
#include "oplate.h"
#include "taputil.h"
#include "tayutil.h"
#include "convolut.h"

#include <stdio.h>
#include <math.h>

#ifdef __cplusplus
#include <malloc.h>
extern "C" {
#endif


/****************************************************************************/
/*                                                             NOW THE CODE */

/*--------------------------------------------------------------------------*/
/*                                                   Local Static Variables */
static short tag;

static int rev_location_cnt;
static int dep_cnt;
static int ind_cnt;

#ifdef _HOS_
/***************************************************************************/
/* Higher Order Scalar Reverse Pass.                                       */
/***************************************************************************/
int hos_reverse(short   tnum,        /* tape id */
                int     depen,       /* consistency chk on # of deps */
                int     indep,       /* consistency chk on # of indeps */
                int     degre,       /* highest derivative degre  */
                double  *lagrange,   /* range weight vector       */
                double  **results)   /* matrix of coefficient vectors */   

#elif _HOV_
/***************************************************************************/
/* Higher Order Vector Reverse Pass.                                       */
/***************************************************************************/
int hov_reverse(short   tnum,        /* tape id */
                int     depen,       /* consistency chk on # of deps */
                int     indep,       /* consistency chk on # of indeps */
                int     degre,       /* highest derivative degre */
                int     nrows,       /* # of Jacobian rows calculated */
                double  **lagrange,  /* domain weight vector */
                double  ***results,  /* matrix of coefficient vectors */
                short   **nonzero )  /* structural sparsity  pattern  */

#endif 

{
/****************************************************************************/
/*                                                           ALL VARIABLES  */
  unsigned char operation; /* operation code */
  int tape_stats[11];      /* tape stats */
  int ret_c=3;

  locint size = 0;
  locint res  = 0;
  locint arg  = 0;
  locint arg1 = 0;
  locint arg2 = 0;

  double coval = 0, *d = 0;

  int indexi = 0,  indexd = 0;

  /* loop indices */ 
  int i, j, l, ls;

  /* other necessary variables */
  double x, y, r0, r_0;
  int buffer;
  static int kax, rax, pax;
  int taycheck;
  int numdep,numind;
  double aTmp;

/*--------------------------------------------------------------------------*/
  /* Taylor stuff */
#ifdef _HI_ORDER_
  static revreal **T;
  static revreal *Ttemp;
  static revreal *Ttemp2;
  revreal *Tres, *Targ, *Targ1, *Targ2, *Tqo;
#else
  static revreal *T;
#endif

/*--------------------------------------------------------------------------*/
  /* Adjoint stuff */
#ifdef _FOS_
  static double* A;
  double Atemp;
#else /* _FOV_, _HOS_, _HOV_ */  
  static double** A;
  static double *Atemp;
#endif
  double  *Ares, *Aarg, *Aarg1, *Aarg2, *Aqo;
#ifdef _HI_ORDER_
  static double *Atemp2;
  double *AP1, *AP2;
#endif

/*--------------------------------------------------------------------------*/
#ifdef _HI_ORDER_
  int k = degre + 1;
  int k1 = k + 1;
  revreal comp;
#endif

#ifdef _ADOLC_VECTOR_
  int p = nrows;
  int computed;
#endif 

#ifdef _HOV_
  int pk1 = p*k1;
#endif


#ifdef DEBUG
/****************************************************************************/
/*                                                           DEBUG MESSAGES */
  fprintf(DIAG_OUT,"Call of %s(..) with tag: %d, n: %d, m %d,\n",
                   GENERATED_FILENAME, tnum, indep, depen);      

#ifdef _HI_ORDER_
  fprintf(DIAG_OUT,"                    degree: %d\n",degre);
#endif
#ifdef _ADOLC_VECTOR_
  fprintf(DIAG_OUT,"                    p: %d\n\n",nrows);
#endif
  
#endif


/****************************************************************************/
/*                                                                    INITs */

  /*------------------------------------------------------------------------*/
  /* Set up stuff for the tape */

  tag = tnum;   /*tag is global which indicates which tape to look at */
        
  tapestats(tag,tape_stats);
  ind_cnt          = tape_stats[0];
  dep_cnt          = tape_stats[1];
  rev_location_cnt = tape_stats[2];
  buffer           = tape_stats[4];

  set_buf_size(buffer);

  if ((depen != dep_cnt)||(indep != ind_cnt))
  { fprintf(DIAG_OUT,"ADOL-C error: Reverse sweep on tape %d  aborted!\n",tag);
    fprintf(DIAG_OUT,"Number of dependent and/or independent variables "
             "passed to reverse is\ninconsistent with number "
             "recorded on tape %d \n",tag);
    exit (-1);
  }
  
  indexi = ind_cnt - 1;
  indexd = dep_cnt - 1;


/****************************************************************************/
/*                                                  MEMORY ALLOCATION STUFF */

/*--------------------------------------------------------------------------*/
#ifdef _HOS_                                                         /* HOS */
  if (k compsize kax || rev_location_cnt compsize rax) 
  { if (rax || kax)
    { free((char*) Atemp);
      free((char*) Atemp2);
      free((char*) Ttemp);
      free((char*) *T); free((char*) T);
      free((char*) *A); free((char*) A);
    }
    Atemp  = myalloc1(k1); 
    Atemp2 = myalloc1(k1);
    A      = myalloc2(rev_location_cnt,k1);
    Ttemp  = (revreal *)malloc(k*sizeof(revreal));
    Ttemp2 = (revreal *)malloc(k*sizeof(revreal));
    T      = (revreal**)malloc(rev_location_cnt*sizeof(revreal*));
    Tqo    = (revreal *)malloc(rev_location_cnt*k*sizeof(revreal));  
    if ((T == NULL)||(Tqo == NULL)||(Ttemp == NULL))
    { fprintf(DIAG_OUT,"ADOL-C error: cannot allocate %i + %i + %i bytes!\n",
              k*sizeof(revreal),rev_location_cnt*sizeof(revreal*),
              rev_location_cnt*k*sizeof(revreal));
      exit (-1);
    }
    for(i=0;i<rev_location_cnt;i++)
    { T[i] = Tqo;
      Tqo += k;
    }
    kax = k;
    rax = rev_location_cnt;
  }
  
/*--------------------------------------------------------------------------*/
#elif _HOV_                                                          /* HOV */
  if (k compsize kax || rev_location_cnt compsize  rax || p compsize  pax)
  { if (kax || rax || pax)
    { free((char*) Atemp);
      free((char*) Atemp2);
      free((char*) Ttemp);
      free((char*) *T); free((char*) T);
      free((char*) *A); free((char*) A);
    }
    A      = myalloc2(rev_location_cnt,pk1);
    Atemp  = myalloc1(pk1);
    Atemp2 = myalloc1(pk1);
    Ttemp  = (revreal*) malloc(k*sizeof(revreal));
    Ttemp2 = (revreal*) malloc(k*sizeof(revreal));
    T      = (revreal**)malloc(rev_location_cnt*sizeof(revreal*));
    Tqo    = (revreal*) malloc(rev_location_cnt*k*sizeof(revreal));
    if ((T == NULL)||(Tqo == NULL)||(Ttemp == NULL))
    { fprintf(DIAG_OUT,"ADOL-C error: cannot allocate %i + %i +%i bytes!\n",
              rev_location_cnt*sizeof(revreal*),
              rev_location_cnt*k*sizeof(revreal),k*sizeof(revreal));
      exit (-1);
    }
    for(i=0; i<rev_location_cnt; i++)
    { T[i] = Tqo;
      Tqo += k;
    } 
    kax = k;
    pax = p;
    rax = rev_location_cnt;
  }
#endif


/****************************************************************************/
/*                                                    TAYLOR INITIALIZATION */
  taylor_back2(T,&numdep,&numind,&taycheck);

  if(taycheck != degre)   
  { fprintf(DIAG_OUT,"\n ADOL-C error: reverse fails because it was not"
                   " preceeded\nby a forward sweep with degree>%i,"
                   " keep=%i!\n",degre,degre+1);
    exit(-2);
  };  

  if((numdep != depen)||(numind != indep))
  { fprintf(DIAG_OUT,"\n ADOL-C error: reverse fails on tape %d because the"
                   " number of\nindependent and/or dependent variables"
                   " given to reverse are\ninconsistent with that of the"
                   "  internal taylor array.\n",tag);
    exit(-2);
  }

  
/****************************************************************************/
/*                                                            REVERSE SWEEP */

  /* Initialize the Reverse Sweep */
  init_rev_sweep(tag);

  operation=get_op_r();
  while (operation != start_of_tape) 
  { /* Switch statement to execute the operations in Reverse */
    switch (operation) {


/****************************************************************************/
/*                                                                  MARKERS */

/*--------------------------------------------------------------------------*/
      case end_of_op:                                          /* end_of_op */
        get_op_block_r();
        operation = get_op_r(); 
        /* Skip next operation, it's another end_of_op */
        break;

/*--------------------------------------------------------------------------*/
      case end_of_int:                                        /* end_of_int */
        get_loc_block_r(); /* Get the next int block */
        break;

/*--------------------------------------------------------------------------*/
      case end_of_val:                                        /* end_of_val */
        get_val_block_r(); /* Get the next val block */
        break;

/*--------------------------------------------------------------------------*/
      case start_of_tape:                                  /* start_of_tape */
      case end_of_tape:                                      /* end_of_tape */
	break;


/****************************************************************************/
/*                                                               COMPARISON */

/*--------------------------------------------------------------------------*/
      case eq_zero  :                                            /* eq_zero */
        arg   = get_locint_r();

        ret_c = 0;
        break;

/*--------------------------------------------------------------------------*/
      case neq_zero :                                           /* neq_zero */
      case gt_zero  :                                            /* gt_zero */
      case lt_zero :                                             /* lt_zero */
        arg   = get_locint_r();
        break;

/*--------------------------------------------------------------------------*/
      case ge_zero :                                             /* ge_zero */
      case le_zero :                                             /* le_zero */
        arg   = get_locint_r();

        if (*T[arg] == 0)
          ret_c = 0;
        break;


/****************************************************************************/
/*                                                              ASSIGNMENTS */

/*--------------------------------------------------------------------------*/
      case assign_a:           /* assign an adouble variable an    assign_a */
	                       /* adouble value. (=) */
        res = get_locint_r();
        arg = get_locint_r();

	ASSIGN_A(Aarg, A[arg])
	ASSIGN_A(Ares, A[res])

        FOR_0_LE_l_LT_p
          if  (0 == ARES)
	  { HOV_INC(Aarg, k1)
            HOV_INC(Ares, k1)
	  }
	  else 
	  { MAXINC(AARG,ARES);
	    AARG_INC_O; ARES_INC = 0.0;
	    FOR_0_LE_i_LT_k
	    { /* ! no tempory */
              AARG_INC += ARES;
              ARES_INC = 0.0;
	    }
          }

	get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case assign_d:            /* assign an adouble variable a    assign_d */
	                        /* double value. (=) */
        res   = get_locint_r();
        coval = get_val_r();
	
        ASSIGN_A(Ares, A[res])

        FOR_0_LE_l_LT_pk1
	  ARES_INC = 0.0;

        get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case assign_d_zero:  /* assign an adouble variable a    assign_d_zero */
      case assign_d_one:   /* double value. (=)                assign_d_one */
        res   = get_locint_r();
	
        ASSIGN_A(Ares, A[res])

        FOR_0_LE_l_LT_pk1
	  ARES_INC = 0.0;

        get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case assign_ind:       /* assign an adouble variable an    assign_ind */
	                     /* independent double value (<<=) */
        res = get_locint_r();
	
        ASSIGN_A(Ares, A[res])

        FOR_0_LE_l_LT_p
        {
#ifdef _HOV_
          if (nonzero) /* ??? question: why here? */
	    nonzero[l][indexi] = (int)ARES;
#endif /* _HOV_ */
	  ARES_INC_O;
	  FOR_0_LE_i_LT_k
	    RESULTS(l,indexi,i) = ARES_INC;
        }

        get_taylors(res,k);
	indexi--;
	break;

/*--------------------------------------------------------------------------*/
      case assign_dep:           /* assign a float variable a    assign_dep */
	                         /* dependent adouble value. (>>=) */
        res = get_locint_r();
	
        ASSIGN_A(Ares, A[res])

        FOR_0_LE_l_LT_p
        { *(++Ares) = LAGRANGE(l,indexd);

          if (ARES) 
            *(--Ares) = 1.0;
          else
            --Ares;
	  HOV_INC(Ares, k1)       
        }
	indexd--;
	break;


/****************************************************************************/
/*                                                   OPERATION + ASSIGNMENT */

/*--------------------------------------------------------------------------*/
      case eq_plus_d:            /* Add a floating point to an    eq_plus_d */
	                         /* adouble. (+=) */
        res   = get_locint_r();
        coval = get_val_r();

	get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case eq_plus_a:             /* Add an adouble to another    eq_plus_a */
	                          /* adouble. (+=) */
        res = get_locint_r();
        arg = get_locint_r();

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg]);

        FOR_0_LE_l_LT_p
          if  (0 == ARES)
	  { HOV_INC(Ares, k1)
            HOV_INC(Aarg, k1)
          }
          else
          { MAXINC(AARG,ARES);
            AARG_INC_O; ARES_INC_O;
            FOR_0_LE_i_LT_k
              AARG_INC += ARES_INC;
          }

	get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case eq_min_d:       /* Subtract a floating point from an    eq_min_d */
                           /* adouble. (-=) */
        res   = get_locint_r();
        coval = get_val_r();

	get_taylors(res,k);
	break;
	
/*--------------------------------------------------------------------------*/
      case eq_min_a:        /* Subtract an adouble from another    eq_min_a */
	                    /* adouble. (-=) */
        res = get_locint_r();
        arg = get_locint_r();

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])

        FOR_0_LE_l_LT_p
          if  (0==ARES)
          { HOV_INC(Ares, k1)
            HOV_INC(Aarg, k1)
          }
          else 
          { MAXINC(AARG,ARES);
            AARG_INC_O; ARES_INC_O;
            FOR_0_LE_i_LT_k
              AARG_INC -= ARES_INC;
          }

	get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case eq_mult_d:              /* Multiply an adouble by a    eq_mult_d */
	                           /* flaoting point. (*=) */
        res   = get_locint_r();
        coval = get_val_r();

	ASSIGN_A(Ares, A[res])

        FOR_0_LE_l_LT_p
          if ( 0 == ARES_INC )
            HOV_INC(Ares, k)	  
	  else
            FOR_0_LE_i_LT_k
	      ARES_INC *= coval;         

	get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case eq_mult_a:       /* Multiply one adouble by another    eq_mult_a */
	                    /* (*=) */
        res = get_locint_r();
        arg = get_locint_r();

 	get_taylors(res,k);

	ASSIGN_A(Ares, A[res])
        ASSIGN_A(Aarg, A[arg])
        ASSIGN_A(Aqo,  Atemp)
	ASSIGN_T(Tres, T[res])
	ASSIGN_T(Targ, T[arg])

        FOR_0_LE_l_LT_p
        { if (0 == ARES) 
	  { HOV_INC(Aarg, k1)
            HOV_INC(Ares, k1)
	  }
	  else
	  { MAXINC(ARES,2.0);
            MAXINC(AARG,ARES);
            AARG_INC_O; ARES_INC_O;
            conv(k,Ares,Targ,Atemp);
            if(arg != res)  
            { inconv(k,Ares,Tres,Aarg);
              FOR_0_LE_i_LT_k 
                ARES_INC = AQO_INC;
            }
            else 
              FOR_0_LE_i_LT_k 
                ARES_INC = 2.0 * AQO_INC;
            HOV_INC(Aarg,k)
          }
        }
        break;
        
/*--------------------------------------------------------------------------*/
      case incr_a:                        /* Increment an adouble    incr_a */
      case decr_a:                        /* Increment an adouble    decr_a */
        res   = get_locint_r();

	get_taylors(res,k);
	break;


/****************************************************************************/
/*                                                        BINARY OPERATIONS */

/*--------------------------------------------------------------------------*/
      case plus_a_a:                 /* : Add two adoubles. (+)    plus a_a */
        res  = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

	ASSIGN_A(Ares,  A[res])
	ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_A(Aarg2, A[arg2])

        FOR_0_LE_l_LT_p
          if  (0 == ARES)
          { HOV_INC(Ares,  k1)
            HOV_INC(Aarg1, k1)
            HOV_INC(Aarg2, k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG1,aTmp);
            MAXINC(AARG2,aTmp);
            AARG2_INC_O; AARG1_INC_O;
            FOR_0_LE_i_LT_k  
            { aTmp = ARES;
              ARES_INC = 0.0;
              AARG1_INC += aTmp;
	      AARG2_INC += aTmp;
            }
          }

   	get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case plus_d_a:             /* Add an adouble and a double    plus_d_a */
	                         /* (+) */
        res   = get_locint_r();
        arg   = get_locint_r();
        coval = get_val_r();

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])

        FOR_0_LE_l_LT_p
          if  (0 == ARES)
          { HOV_INC(Ares, k1)
            HOV_INC(Aarg, k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
            AARG_INC_O;
            FOR_0_LE_i_LT_k
            { aTmp = ARES;
              ARES_INC = 0.0;
              AARG_INC += aTmp;
            }
          }

 	get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case min_a_a:              /* Subtraction of two adoubles    min_a_a */
	                         /* (-) */
        res  = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

	ASSIGN_A(Ares,  A[res])
	ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_A(Aarg2, A[arg2])

        FOR_0_LE_l_LT_p
          if  (0 == ARES)
          { HOV_INC(Ares,  k1)
            HOV_INC(Aarg1, k1)
            HOV_INC(Aarg2, k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG1,aTmp);
            MAXINC(AARG2,aTmp);
            AARG2_INC_O; AARG1_INC_O;
            FOR_0_LE_i_LT_k
            { aTmp = ARES;
              ARES_INC = 0.0;
              AARG1_INC += aTmp;
              AARG2_INC -= aTmp;
            }
          }
 
        get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case min_d_a:                /* Subtract an adouble from a    min_d_a */
	                           /* double (-) */
        res   = get_locint_r();
        arg   = get_locint_r();
        coval = get_val_r();

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])

        FOR_0_LE_l_LT_p
          if (0 == ARES)
          { HOV_INC(Ares, k1)
            HOV_INC(Aarg, k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
            AARG_INC_O;
            FOR_0_LE_i_LT_k 
            { aTmp = ARES;
              ARES_INC = 0.0;
              AARG_INC -= aTmp;
            } 
          }

 	get_taylors(res,k);
	break;
	
/*--------------------------------------------------------------------------*/
      case mult_a_a:               /* Multiply two adoubles (*)    mult_a_a */
        res  = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

      	get_taylors(res,k);

	ASSIGN_A(Ares,  A[res])
	ASSIGN_A(Aarg2, A[arg2])
	ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_T(Targ1, T[arg1])
	ASSIGN_T(Targ2, T[arg2])

        FOR_0_LE_l_LT_p
          if (0 == ARES)
          { HOV_INC(Aarg1, k1)
            HOV_INC(Aarg2, k1)
            HOV_INC(Ares,  k1)
          }
          else
          { comp = (ARES > 2.0) ? ARES : 2.0 ;
            ARES_INC = 0.0;
            MAXINC(AARG1,comp);
            MAXINC(AARG2,comp);
            AARG1_INC_O; AARG2_INC_O;

            copyAndZeroset(k,Ares,Atemp);
            inconv(k,Atemp,Targ1,Aarg2);
            inconv(k,Atemp,Targ2,Aarg1);

            HOV_INC(Ares,  k)
            HOV_INC(Aarg1, k)
            HOV_INC(Aarg2, k)
          }
        break;

/*--------------------------------------------------------------------------*/
      /* olvo 991122: new op_code with recomputation */
      case eq_plus_prod:   /* increment a product of           eq_plus_prod */
                           /* two adoubles (*) */
        res  = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();


	ASSIGN_A(Ares,  A[res])
	ASSIGN_A(Aarg2, A[arg2])
	ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_T(Targ1, T[arg1])
	ASSIGN_T(Targ2, T[arg2])

        /* RECOMPUTATION */
        ASSIGN_T( Tres,  T[res])
        deconv(k,Targ1,Targ2,Tres);

        FOR_0_LE_l_LT_p
          if (0 == ARES)
          { HOV_INC(Aarg1, k1)
            HOV_INC(Aarg2, k1)
            HOV_INC(Ares,  k1)
          }
          else
          { comp = (ARES > 2.0) ? ARES : 2.0 ;
            ARES_INC = comp;
            MAXINC(AARG1,comp);
            MAXINC(AARG2,comp);
            AARG1_INC_O; AARG2_INC_O;

            inconv(k,Ares,Targ1,Aarg2);
            inconv(k,Ares,Targ2,Aarg1);

            HOV_INC(Ares,  k)
            HOV_INC(Aarg1, k)
            HOV_INC(Aarg2, k)
          }
        break;

/*--------------------------------------------------------------------------*/
      /* olvo 991122: new op_code with recomputation */
      case eq_min_prod:   /* decrement a product of             eq_min_prod */
                           /* two adoubles (*) */
        res  = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();


	ASSIGN_A(Ares,  A[res])
	ASSIGN_A(Aarg2, A[arg2])
	ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_T(Targ1, T[arg1])
	ASSIGN_T(Targ2, T[arg2])

        /* RECOMPUTATION */
        ASSIGN_T( Tres,  T[res])
        inconv(k,Targ1,Targ2,Tres);

        FOR_0_LE_l_LT_p
          if (0 == ARES)
          { HOV_INC(Aarg1, k1)
            HOV_INC(Aarg2, k1)
            HOV_INC(Ares,  k1)
          }
          else
          { comp = (ARES > 2.0) ? ARES : 2.0 ;
            ARES_INC = comp;
            MAXINC(AARG1,comp);
            MAXINC(AARG2,comp);
            AARG1_INC_O; AARG2_INC_O;

            deconv(k,Ares,Targ1,Aarg2);
            deconv(k,Ares,Targ2,Aarg1);

            HOV_INC(Ares,  k)
            HOV_INC(Aarg1, k)
            HOV_INC(Aarg2, k)
          }
        break;

/*--------------------------------------------------------------------------*/
      case mult_d_a:         /* Multiply an adouble by a double    mult_d_a */
                             /* (*) */
        res   = get_locint_r();
        arg   = get_locint_r();
        coval = get_val_r();

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])

        FOR_0_LE_l_LT_p
          if (0 == ARES)
          { HOV_INC(Ares, k1)
            HOV_INC(Aarg, k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
            AARG_INC_O;
            FOR_0_LE_i_LT_k  
            { aTmp = ARES;
              ARES_INC = 0.0;
              AARG_INC += coval * aTmp;
            }
          }

 	get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case div_a_a:           /* Divide an adouble by an adouble    div_a_a */
                              /* (/) */
        res  = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

	ASSIGN_A(Ares,  A[res])
	ASSIGN_A(Aarg2, A[arg2])
	ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_T(Tres,  T[res])
	ASSIGN_T(Targ2, T[arg2])

	/* olvo 980922 allows reflexive operation */ 
        if (arg2 == res)
	{ for (i=0; i<k; i++)
            Ttemp2[i] = Tres[i];
          Tres = Ttemp2;
      	  get_taylors(res,k);
        }

VEC_COMPUTED_INIT
        FOR_0_LE_l_LT_p
        { if (0 == ARES)
	  { HOV_INC(Ares,  k1)
	    HOV_INC(Aarg1, k1)
	    HOV_INC(Aarg2, k1)
	  }
	  else
	  { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG1,3.0);
	    MAXINC(AARG1,aTmp);
	    MAXINC(AARG2,3.0);
	    MAXINC(AARG2,aTmp);
	    AARG1_INC_O; AARG2_INC_O;

VEC_COMPUTED_CHECK
            recipr(k,1.0,Targ2,Ttemp);
            conv(k,Ttemp,Tres,Atemp2);
VEC_COMPUTED_END
            copyAndZeroset(k,Ares,Atemp);
            inconv(k,Atemp,Ttemp,Aarg1);
            deconv(k,Atemp,Atemp2,Aarg2);

            HOV_INC(Ares,  k)
            HOV_INC(Aarg1, k)
            HOV_INC(Aarg2, k)
          }
        }

    	if (res != arg2)
          get_taylors(res,k);
        break;

/*--------------------------------------------------------------------------*/
      case div_d_a:             /* Division double - adouble (/)    div_d_a */
        res   = get_locint_r();
        arg   = get_locint_r();
        coval = get_val_r();

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])
	ASSIGN_T(Tres, T[res])
	ASSIGN_T(Targ, T[arg])

	/* olvo 980922 allows reflexive operation */ 
        if (arg == res)
	{ for (i=0; i<k; i++)
            Ttemp2[i] = Tres[i];
          Tres = Ttemp2;
    	  get_taylors(arg,k);
	}

VEC_COMPUTED_INIT
        FOR_0_LE_l_LT_p
        { if (0 == ARES)
          { HOV_INC(Ares, k1)
	    HOV_INC(Aarg, k1)
          }
          else
	  { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
            MAXINC(AARG,3.0);
            AARG_INC_O;

VEC_COMPUTED_CHECK
            recipr(k,1.0,Targ,Ttemp);
            conv(k,Ttemp,Tres,Atemp);
VEC_COMPUTED_END
            deconv0(k,Ares,Atemp,Aarg);

            HOV_INC(Ares, k)
            HOV_INC(Aarg, k)
          }
        }

 	get_taylors(res,k);
        break;


/****************************************************************************/
/*                                                         SIGN  OPERATIONS */

/*--------------------------------------------------------------------------*/
      case pos_sign_a:                                        /* pos_sign_a */
        res   = get_locint_r();
        arg   = get_locint_r();

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])

        FOR_0_LE_l_LT_p
          if  (0 == ARES)
          { HOV_INC(Ares, k1)
            HOV_INC(Aarg, k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
            AARG_INC_O;
            FOR_0_LE_i_LT_k
            { aTmp = ARES;
              ARES_INC = 0.0;
              AARG_INC += aTmp;
            }
          }

 	get_taylors(res,k);
	break;

/*--------------------------------------------------------------------------*/
      case neg_sign_a:                                        /* neg_sign_a */
        res   = get_locint_r();
        arg   = get_locint_r();

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])

        FOR_0_LE_l_LT_p
          if  (0 == ARES)
          { HOV_INC(Ares, k1)
            HOV_INC(Aarg, k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
            AARG_INC_O;
            FOR_0_LE_i_LT_k
            { aTmp = ARES;
              ARES_INC = 0.0;
              AARG_INC -= aTmp;
            }
          }

 	get_taylors(res,k);
	break;


/****************************************************************************/
/*                                                         UNARY OPERATIONS */

/*--------------------------------------------------------------------------*/
      case exp_op:                          /* exponent operation    exp_op */
        res = get_locint_r();
        arg = get_locint_r();

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])
        ASSIGN_T(Tres, T[res])
	ASSIGN_T(Targ, T[arg])

        FOR_0_LE_l_LT_p
        { if (0 == ARES)
	  { HOV_INC(Aarg, k1)
            HOV_INC(Ares, k1)
          }
          else
	  { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
            MAXINC(AARG,4.0);
            AARG_INC_O;

            inconv0(k,Ares,Tres,Aarg);

            HOV_INC(Ares, k)
            HOV_INC(Aarg, k)
          }
        }

        get_taylors(res,k);
        break;

/*--------------------------------------------------------------------------*/
      case sin_op:                              /* sine operation    sin_op */
        res  = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

	ASSIGN_A(Ares,  A[res])
	ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_T(Targ2, T[arg2])

        FOR_0_LE_l_LT_p
        { if (0 == ARES)
          { HOV_INC(Aarg1, k1)
	    HOV_INC(Ares,  k1)
          }
          else
	  { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG1,aTmp);
            MAXINC(AARG1,4.0);
            AARG1_INC_O;

            inconv0(k,Ares,Targ2,Aarg1);

            HOV_INC(Ares,  k)
            HOV_INC(Aarg1, k)
          }
        }

       	get_taylors(res,k);
       	get_taylors(arg2,k); /* olvo 980710 covalue */
	                     /* NOTE: A[arg2] should be 0 already */
	break;

/*--------------------------------------------------------------------------*/
      case cos_op:                            /* cosine operation    cos_op */
        res  = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

	ASSIGN_A(Ares,  A[res])
	ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_T(Targ2, T[arg2])

        FOR_0_LE_l_LT_p
        { if (0 == ARES)
	  { HOV_INC(Aarg1, k1)
            HOV_INC(Ares,  k1)
	  }
          else
	  { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG1,aTmp);
            MAXINC(AARG1,4.0);
            AARG1_INC_O;

            deconv0(k,Ares,Targ2,Aarg1);

            HOV_INC(Ares,  k)
            HOV_INC(Aarg1, k)
          }
        }                

       	get_taylors(res,k);
       	get_taylors(arg2,k); /* olvo 980710 covalue */
	                     /* NOTE: A[arg2] should be 0 already */
	break;

/*--------------------------------------------------------------------------*/
      case atan_op:                                             /* atan_op  */
      case asin_op:                                             /* asin_op  */
      case acos_op:                                             /* acos_op  */
      case asinh_op:                                            /* asinh_op */
      case acosh_op:                                            /* acosh_op */
      case atanh_op:                                            /* atanh_op */
      case erf_op:                                              /* erf_op   */
        res  = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

        get_taylors(res,k);

        ASSIGN_A(Ares,  A[res])
        ASSIGN_A(Aarg1, A[arg1])
        ASSIGN_T(Targ2, T[arg2])

        FOR_0_LE_l_LT_p
        { if (0 == ARES)
          { HOV_INC(Aarg1, k1)
            HOV_INC(Ares,  k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG1,aTmp);
            MAXINC(AARG1,4.0);
            AARG1_INC_O;

            inconv0(k,Ares,Targ2,Aarg1);
                
            HOV_INC(Aarg1, k) 
            HOV_INC(Ares,  k)
          }
        }
	break;

/*--------------------------------------------------------------------------*/
      case log_op:                                                /* log_op */
        res = get_locint_r();
        arg = get_locint_r();

        get_taylors(res,k);

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])
	ASSIGN_T(Targ, T[arg])

VEC_COMPUTED_INIT
        FOR_0_LE_l_LT_p
        { if (0 == ARES)
          { HOV_INC(Aarg, k1)
            HOV_INC(Ares, k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
            MAXINC(AARG,4.0);
            AARG_INC_O;

VEC_COMPUTED_CHECK
            recipr(k,1.0,Targ,Ttemp);
VEC_COMPUTED_END 
            inconv0(k,Ares,Ttemp,Aarg);

            HOV_INC(Ares, k)
            HOV_INC(Aarg, k)
          }
        }
	break;

/*--------------------------------------------------------------------------*/
      case pow_op:                                                /* pow_op */
        res   = get_locint_r();
        arg   = get_locint_r();
        coval = get_val_r();

	ASSIGN_T(Targ, T[arg])
	ASSIGN_T(Tres, T[res])
	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])

	/* olvo 980921 allows reflexive operation */ 
        if (arg == res)
	{ for (i=0; i<k; i++)
            Ttemp2[i] = Tres[i];
          Tres = Ttemp2;
    	  get_taylors(arg,k);
	}

VEC_COMPUTED_INIT
        FOR_0_LE_l_LT_p
          if (0 == ARES)
          { HOV_INC(Aarg, k1)
            HOV_INC(Ares, k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
            MAXINC(AARG,4.0);
            AARG_INC_O;

VEC_COMPUTED_CHECK
            divide(k,Tres,Targ,Ttemp);
            for (i=0;i<k;i++) 
              Ttemp[i] *= coval;
VEC_COMPUTED_END 
            inconv0(k,Ares,Ttemp,Aarg);
           
            HOV_INC(Ares, k)
            HOV_INC(Aarg, k)
          }  
 
  	get_taylors(res,k);
        break;

/*--------------------------------------------------------------------------*/
      case sqrt_op:                                              /* sqrt_op */
        res = get_locint_r();
        arg = get_locint_r();

	ASSIGN_A(Ares, A[res])
	ASSIGN_A(Aarg, A[arg])
	ASSIGN_T(Tres, T[res])

VEC_COMPUTED_INIT
        FOR_0_LE_l_LT_p
          if (0 == ARES)
          { HOV_INC(Aarg, k1)
            HOV_INC(Ares, k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
            MAXINC(AARG,4.0);
            AARG_INC_O;

VEC_COMPUTED_CHECK
            recipr(k,0.5,Tres,Ttemp);
VEC_COMPUTED_END
            inconv0(k,Ares,Ttemp,Aarg);

            HOV_INC(Ares, k)
            HOV_INC(Aarg, k)
          }       
 
  	get_taylors(res,k);
        break;

/*--------------------------------------------------------------------------*/
      case gen_quad:                                            /* gen_quad */
        res   = get_locint_r();
        arg2  = get_locint_r();
        arg1  = get_locint_r();
        coval = get_val_r();
        coval = get_val_r();

	ASSIGN_A(Ares,  A[res])
	ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_T(Targ2, T[arg2])

        FOR_0_LE_l_LT_p
          if (0 == ARES)
          { HOV_INC(Aarg1, k1)
            HOV_INC(Ares,  k1)
          }
          else
          { aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG1,aTmp);
            MAXINC(AARG1,4.0);
            AARG1_INC_O;
              
            inconv0(k,Ares,Targ2,Aarg1);

            HOV_INC(Aarg1, k) 
            HOV_INC(Ares,  k)
          }
  
      	get_taylors(res,k);
      	break;

/*--------------------------------------------------------------------------*/
      case min_op:                                                /* min_op */
        res   = get_locint_r();
        arg2  = get_locint_r();
        arg1  = get_locint_r();
        coval = get_val_r();
  
  	get_taylors(res,k);

        ASSIGN_A(Aarg1, A[arg1])
        ASSIGN_A(Aarg2, A[arg2])
        ASSIGN_A(Ares,  A[res])
        ASSIGN_T(Targ1, T[arg1])
        ASSIGN_T(Targ2, T[arg2])
        ASSIGN_A(AP1,   NULL)
        ASSIGN_A(AP2,   Ares)

        if (Targ1[0] > Targ2[0])
        { FOR_0_LE_l_LT_p 
          { if ((coval) && (*AP2))
              MINDEC(ret_c,2);
            HOV_INC(AP2,k1)
          }
          AP1 = Aarg2;
          arg = 0;
        }
        else 
          if (Targ1[0] < Targ2[0])
          { FOR_0_LE_l_LT_p
            { if ((!coval) && (*AP2))
                MINDEC(ret_c,2);
              HOV_INC(AP2,k1)
            }
            AP1 = Aarg1; 
            arg = 0; 
          } 
          else /* both are equal */
            for (i=1;i<k;i++)
            { if (Targ1[i] > Targ2[i])
              { FOR_0_LE_l_LT_p 
                { if (*AP2)
                    MINDEC(ret_c,1);
                  HOV_INC(AP2,k1)
		}
                AP1 = Aarg2;
                arg = i+1;
              }
              else 
                if (Targ1[i] < Targ2[i])
                { FOR_0_LE_l_LT_p
                  { if (*AP2)
                      MINDEC(ret_c,1);
                    HOV_INC(AP2,k1)
	   	  }
                  AP1 = Aarg1;  
                  arg = i+1;
                }
              if (AP1 != NULL)
                break;
            }

        if (AP1 != NULL)
          FOR_0_LE_l_LT_p
          { if (0 == ARES) 
            { HOV_INC(AP1, k1) 
              HOV_INC(Ares,k1);
            }
            else 
            { aTmp = ARES;
              ARES_INC = 0.0; 
              if (arg)  /* we are at the tie */
                *AP1 = 5.0;
              else
                MAXINC(*AP1,aTmp); 
              AP1++;
              for (i=0;i<k;i++)
	      { aTmp = ARES;
                ARES_INC = 0.0;
                *AP1++ += aTmp;
              } 
            } 
          }
        else /* both are identical */
        { FOR_0_LE_l_LT_p
          { if (0 == ARES) 
            { HOV_INC(Aarg1,k1)
              HOV_INC(Aarg2,k1)
              HOV_INC(Ares, k1)
            }
            else 
            { aTmp = ARES;
              ARES_INC = 0.0;
              MAXINC(AARG1,aTmp);  /*assume sthg like fmin(x,x) */
              MAXINC(AARG2,aTmp);
              AARG1_INC_O; AARG2_INC_O;
              for (i=0;i<k;i++)
              { aTmp = ARES;
                ARES_INC = 0.0;
                AARG1_INC += aTmp/2;
                AARG2_INC += aTmp/2;
              }
            }
          }
          if (arg1 != arg2)
            MINDEC(ret_c,1);
        }
        break;


/*--------------------------------------------------------------------------*/
      case abs_val:                                              /* abs_val */ 
        res   = get_locint_r();
        arg   = get_locint_r();
        coval = get_val_r();
  
  	get_taylors(res,k);

        ASSIGN_A(Ares, A[res])
        ASSIGN_A(Aarg, A[arg])
        ASSIGN_T(Targ, T[arg])

        x = 0.0;
        j = 0;
        for (i=0;i<k;i++)
          if ( (x == 0.0) && (Targ[i] != 0.0) ) 
          { j = i;
            if (Targ[i] < 0.0)
              x = -1.0;
            else
              x = 1.0;
          } 
        FOR_0_LE_l_LT_p 
          if (0 == ARES) 
          { HOV_INC(Aarg, k1)
            HOV_INC(Ares, k1)
          }
          else 
          { if (Targ[0] == 0.0)
            {  ARES_INC = 0.0; AARG_INC = 5.0;
            }
            else 
            { aTmp = ARES;
              ARES_INC = 0.0;
              MAXINC(AARG,aTmp);
              AARG_INC_O;
            }
            if(Targ[0] == 0.0) 
              MINDEC(ret_c,1);
            for (i=0;i<j;i++)
              ARES_INC = 0.0;
            Aarg += j;
            for (i=j;i<k;i++)
            { aTmp = ARES;
              ARES_INC = 0.0;
              if ( (coval) && (x<0) && (aTmp) )
                MINDEC(ret_c,2);
              if ( (!coval) && (x>0) && (aTmp))
                MINDEC(ret_c,2);
              AARG_INC += x * aTmp;
            }
          }
        break;

/*--------------------------------------------------------------------------*/
      case ceil_op:                                              /* ceil_op */
        res   = get_locint_r();
        arg   = get_locint_r();
        coval = get_val_r();

  	get_taylors(res,k);

        coval = (coval != ceil(*T[arg]) );

        ASSIGN_A(Ares, A[res])

        FOR_0_LE_l_LT_p
          if (0 == ARES) 
          { HOV_INC(Aarg,  k1)
            HOV_INC(Ares,  k1)
          } 
          else 
          { ARES_INC = 0.0; AARG_INC = 5.0;
            FOR_0_LE_i_LT_k
            { if ((coval) && (ARES))
                MINDEC(ret_c,2);
              ARES_INC = 0.0;
            }
	    HOV_INC(Aarg, k) 
          }
        break;

/*--------------------------------------------------------------------------*/
      case floor_op:                                            /* floor_op */
        res   = get_locint_r();
        arg   = get_locint_r();
        coval = get_val_r();

  	get_taylors(res,k);

        coval = ( coval != floor(*T[arg]) );

        ASSIGN_A(Ares, A[res])
        ASSIGN_A(Aarg, A[arg])

        FOR_0_LE_l_LT_p
          if (0 == ARES) 
          { HOV_INC(Aarg, k1)
            HOV_INC(Ares, k1)
          }
          else 
          { ARES = 0.0; AARG_INC = 5.0;
            FOR_0_LE_i_LT_k
            { if ( (coval) && (ARES) )
                MINDEC(ret_c,2);
              ARES_INC = 0.0;
            } 
            HOV_INC(Aarg, k) 
          }
        break;    


/****************************************************************************/
/*                                                             CONDITIONALS */

/*--------------------------------------------------------------------------*/
      case cond_assign:                                      /* cond_assign */
        res   = get_locint_r();
        arg2  = get_locint_r();
        arg1  = get_locint_r();
        arg   = get_locint_r();
        coval = get_val_r(); 

	get_taylors(res,k);

        ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_A(Ares,  A[res])
        ASSIGN_A(Aarg2, A[arg2])
        ASSIGN_T(Targ,  T[arg])

	/* olvo 980925 changed code a little bit */
        if (TARG > 0.0)
	{ if (res != arg1)
            FOR_0_LE_l_LT_p
            { if (0 == ARES)
              { HOV_INC(Ares,  k1)
                HOV_INC(Aarg1, k1)
              }
              else 
              { if (coval <= 0.0)
                  MINDEC(ret_c,2);
                MAXINC(AARG1,ARES);
                ARES_INC = 0.0;
                AARG1_INC_O;
                FOR_0_LE_i_LT_k
                { AARG1_INC += ARES;
                  ARES_INC = 0;
                } 
              } 
	    }
          else
            FOR_0_LE_l_LT_p
            { if ((coval <= 0.0) && (ARES)) 
                MINDEC(ret_c,2);
              HOV_INC(Ares,  k1)  
	    }
        }
        else /* TARG <= 0.0 */
	{ if (res != arg2)
            FOR_0_LE_l_LT_p
            { if (0 == ARES)
              { HOV_INC(Ares,  k1)
                HOV_INC(Aarg2, k1)
              }
              else 
              { if (TARG == 0.0) /* we are at the tie */                  
                { MINDEC(ret_c,0);
                  AARG1 = 5.0; AARG2_INC = 5.0;
                } 
                else
                { if (coval <= 0.0)
                  MINDEC(ret_c,2);
                  MAXINC(AARG2,ARES);
                  AARG2_INC_O; 
                } 
                ARES_INC = 0.0;

                FOR_0_LE_i_LT_k
                { AARG2_INC += ARES;
                  ARES_INC = 0;
                } 
              } 
              HOV_INC(Aarg1, k1)
	    }
          else
            FOR_0_LE_l_LT_p
            { if (ARES)
              { if (TARG == 0.0) /* we are at the tie */                  
                { MINDEC(ret_c,0);
                  AARG1 = 5.0; AARG2 = 5.0;
                } 
                else
                  if (coval <= 0.0) 
                    MINDEC(ret_c,2);
              }
              HOV_INC(Ares,  k1)  
              HOV_INC(Aarg1, k1)
              HOV_INC(Aarg2, k1)
	    }
        }
     	break;
 
/*--------------------------------------------------------------------------*/
      case cond_assign_s:                                  /* cond_assign_s */
        res   = get_locint_r();
        arg1  = get_locint_r();
        arg   = get_locint_r(); 
        coval = get_val_r();

	get_taylors(res,k);

	ASSIGN_A(Aarg1, A[arg1])
	ASSIGN_A(Ares,  A[res])
        ASSIGN_T(Targ,  T[arg])

	/* olvo 980925 changed code a little bit */
        if (TARG == 0.0) /* we are at the tie */
        { FOR_0_LE_l_LT_p
          { if  (ARES) 
              AARG1 = 5.0; 
            HOV_INC(Aarg1, k1)
            HOV_INC(Ares,  k1)
          }
          MINDEC(ret_c,0);
        } 
        else
          if (TARG > 0.0)
          { if (res != arg1)
              FOR_0_LE_l_LT_p
              { if  (0 == ARES)
                { HOV_INC(Ares,  k1)
                  HOV_INC(Aarg1, k1)
                }
                else
                { if (coval <= 0.0)
                    MINDEC(ret_c,2);
                  MAXINC(AARG1,ARES);
                  ARES_INC = 0.0;
                  AARG1_INC_O;
                  FOR_0_LE_i_LT_k
                  { (AARG1_INC) += ARES;
                    ARES_INC = 0;
                  } 
	        }
	      }
            else
              FOR_0_LE_l_LT_p
              { if ((coval <= 0.0) && (ARES)) 
                  MINDEC(ret_c,2);
                HOV_INC(Ares,  k1)  
	      }
          } 
	break;


/****************************************************************************/
/*                                                       VECTOR ASSIGNMENTS */

/*--------------------------------------------------------------------------*/
      case assign_av:                                          /* assign_av */
        res  = get_locint_r();
        size = get_locint_r();
        arg  = get_locint_r();

	res += size;
	arg += size;
        for (ls=size; ls>0; ls--)
	{ res--;             /* Location of left-hand-side  */
	  arg--;             /* Location of right-hand-side */

          /* code for assign_a */
	  ASSIGN_A( Aarg, A[arg])
	  ASSIGN_A( Ares, A[res])

          FOR_0_LE_l_LT_p
            if  (0 == ARES)
	    { HOV_INC(Aarg, k1)
              HOV_INC(Ares, k1)
	    }
	    else 
	    { MAXINC(AARG,ARES);
	      AARG_INC_O; ARES_INC = 0.0;
	      FOR_0_LE_i_LT_k
	      { /* ! no tempory */
                AARG_INC += ARES;
                ARES_INC = 0.0;
	      }
            }

	  get_taylors(res,k);
        }
	break;

/*--------------------------------------------------------------------------*/
      case assign_dv:                                          /* assign_dv */
        res  = get_locint_r();
        size = get_locint_r();
        d    = get_val_v_r(size);

        res += size;
        d   += size;
	for (ls=size; ls>0; ls--)
	{ res--;                    /* Location of left-hand-side */
	  coval = *(--d);           /* Value of right-hand-side   */     

	  /* code for assign_d */
          ASSIGN_A( Ares, A[res])
	   
          FOR_0_LE_l_LT_pk1
	    ARES_INC = 0.0;
            
	  get_taylors(res,k);
	}
	break;

/*--------------------------------------------------------------------------*/
      case assign_indvec:                                  /* assign_indvec */
        res  = get_locint_r();
        size = get_locint_r();

        res += size;
	for (ls=size; ls>0; ls--)
	{ res--;            /* Location of the left-hand-side */

	  /* code for assign_ind */
	  ASSIGN_A(Ares, A[res])

          FOR_0_LE_l_LT_p
          {
#ifdef _HOV_
            if (nonzero) /* ??? question: why here? */
	      nonzero[l][indexi] = (int)ARES;
#endif /* _HOV_ */
	    ARES_INC_O;
	    FOR_0_LE_i_LT_k
	      RESULTS(l,indexi,i) = ARES_INC;
          }

          get_taylors(res,k);
          indexi--;
        }
        reset_val_r();
	break;

/*--------------------------------------------------------------------------*/
      case assign_depvec:                                  /* assign_depvec */
        res  = get_locint_r();
        size = get_locint_r();

        res += size;
	for (ls=size; ls>0; ls--)
	{ res--;            /* Location of the left-hand-side */

	  /* code for assign_dep */
	  ASSIGN_A( Ares, A[res])
            
          FOR_0_LE_l_LT_p
          { *(++Ares) = LAGRANGE(l,indexd);
          
            if (ARES) 
              *(--Ares) = 1.0;
            else
              --Ares;
            Ares += k1;       
          }
          indexd--;
	}
	break;


/****************************************************************************/
/*                                            VECTOR OPERATION + ASSIGNMENT */

/*--------------------------------------------------------------------------*/
      case eq_plus_av:                                        /* eq_plus_av */
        res  = get_locint_r();
        size = get_locint_r();
        arg  = get_locint_r();

        res += size;
        arg += size;
	for (ls=size; ls>0; ls--)
	{ res--;            /* Location of left-hand-side  */
	  arg--;            /* Location on right-hand-side */

	  /* code for eq_plus_a */
          ASSIGN_A( Ares, A[res])
	  ASSIGN_A( Aarg, A[arg])

          FOR_0_LE_l_LT_p
            if  (0 == ARES)
	    { HOV_INC(Ares, k1)
              HOV_INC(Aarg, k1)
            }
            else
            { MAXINC(AARG,ARES);
              AARG_INC_O; ARES_INC_O;
              FOR_0_LE_i_LT_k
                AARG_INC += ARES_INC;
            }

	  get_taylors(res,k);
	}
	break;

/*--------------------------------------------------------------------------*/
      case eq_min_av:                                          /* eq_min_av */
        res  = get_locint_r();
        size = get_locint_r();
        arg  = get_locint_r();

        res += size;
        arg += size;
	for (ls=size; ls>0; ls--)
	{ res--;            /* Location of left-hand-side  */
	  arg--;            /* Location on right-hand-side */

	  /* code for eq_min_a */ 
          ASSIGN_A( Ares, A[res])
	  ASSIGN_A( Aarg, A[arg])

          FOR_0_LE_l_LT_p
            if  (0==ARES)
            { HOV_INC(Ares, k1)
              HOV_INC(Aarg, k1)
            }
            else 
            { MAXINC(AARG,ARES);
              AARG_INC_O; ARES_INC_O;
              FOR_0_LE_i_LT_k
                AARG_INC -= ARES_INC;
            }

  	  get_taylors(res,k);
	}
	break;

/*--------------------------------------------------------------------------*/
      case eq_mult_av_d:                                    /* eq_mult_av_d */
        res   = get_locint_r();
        size  = get_locint_r();
        coval = get_val_r();

        res += size;
	for (ls=size; ls>0; ls--)
	{ res--;             /* Location of the left-hand-side  */
	  /* coval = fixed;     value on the right-hand-side */

	  /* code for eq_mult_d*/
	  ASSIGN_A( Ares, A[res])

          FOR_0_LE_l_LT_p
            if ( 0 == ARES_INC )
              HOV_INC(Ares, k)
            else
              FOR_0_LE_i_LT_k
	        ARES_INC *= coval;
 
	  get_taylors(res,k);
	}
	break;

/*--------------------------------------------------------------------------*/
      case eq_mult_av_a:                                    /* eq_mult_av_a */
        res  = get_locint_r();
        size = get_locint_r();
        arg  = get_locint_r();

        /* olvo 980929 new strategy to check for overwrites 
           (changes computation order) */
        if ((arg >= res) && (arg < res+size)) 
	{ /* FIRST compute the case: res==arg */
          /* simplified code for eq_mult_a*/
	  get_taylors(arg,k);
	    
	  ASSIGN_T( Targ, T[arg])
	  ASSIGN_A( Aarg, A[arg])
	  ASSIGN_A( Aqo,  Atemp)
 
          FOR_0_LE_l_LT_p
           if (0 == AARG) 
	   { HOV_INC(Aarg, k1)
	   }
	   else
	   { MAXINC(AARG,2.0);
	     AARG_INC_O;
	     conv(k,Aarg,Targ,Atemp);
	     FOR_0_LE_i_LT_k 
	       AARG_INC = 2.0 * AQO_INC;
	   }
	}

        res += size;
	for (ls=size; ls>0; ls--)
	{ res--;                 /* Location of the left-hand-side  */
	  /* arg   = fixed;         Location on the right-hand-side */
     
          if (res == arg) /* NOW skip this case */
            continue;

	  /* code for eq_mult_a*/
	  get_taylors(res,k);
	    
          ASSIGN_T( Tres, T[res])
	  ASSIGN_T( Targ, T[arg]);
	  ASSIGN_A( Aarg, A[arg])
	  ASSIGN_A( Ares, A[res])
	  ASSIGN_A( Aqo,  Atemp)
 
          FOR_0_LE_l_LT_p
           if (0 == ARES) 
	   { HOV_INC(Aarg, k1)
	     HOV_INC(Ares, k1)
	   }
	   else
	   { MAXINC(ARES,2.0);
	     MAXINC(AARG,ARES);
	     AARG_INC_O; ARES_INC_O;
	     conv(k,Ares,Targ,Atemp);
	     if(arg != res)  
	     { inconv(k,Ares,Tres,Aarg);
	       FOR_0_LE_i_LT_k 
		 ARES_INC = AQO_INC;
	     }
	     else 
	       FOR_0_LE_i_LT_k 
		 ARES_INC = 2.0 * AQO_INC;
	     HOV_INC(Aarg, k)
	   }
	}
	break;


/****************************************************************************/
/*                                                 BINARY VECTOR OPERATIONS */

/*--------------------------------------------------------------------------*/
      case plus_av_av:                                        /* plus_av_av */
        res  = get_locint_r();
        size = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

	res  += size;
	arg1 += size;
	arg2 += size;
	for (ls=size; ls>0; ls--)
	{ arg2--;       /* Location of var 2  */
	  arg1--;       /* Location of var 1  */
	  res--;        /* Location of result */

          /* code for plus_a_a */
	  ASSIGN_A( Ares,  A[res])
	  ASSIGN_A( Aarg1, A[arg1])
	  ASSIGN_A( Aarg2, A[arg2])
            
          FOR_0_LE_l_LT_p
            if  (0 == ARES)
            { HOV_INC(Ares,  k1)
              HOV_INC(Aarg1, k1)
              HOV_INC(Aarg2, k1)
            }
            else
            { aTmp = ARES;
              ARES_INC = 0.0;
              MAXINC(AARG1,aTmp);
              MAXINC(AARG2,aTmp);
              AARG2_INC_O; AARG1_INC_O;
              FOR_0_LE_i_LT_k  
              { aTmp = ARES;
                ARES_INC = 0.0;
                AARG1_INC += aTmp;
	        AARG2_INC += aTmp;
              }
            }

   	  get_taylors(res,k);
        }
	break;

/*--------------------------------------------------------------------------*/
      case sub_av_av:                                          /* sub_av_av */
        res  = get_locint_r();
        size = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

	res  += size;
	arg1 += size;
	arg2 += size;
	for (ls=size; ls>0; ls--)
	{ arg2--;       /* Location of var 2  */
	  arg1--;       /* Location of var 1  */
	  res--;        /* Location of result */

	  /* code for min_a_a */
	  ASSIGN_A( Ares, A[res])
	  ASSIGN_A( Aarg1, A[arg1])
	  ASSIGN_A( Aarg2, A[arg2])

          FOR_0_LE_l_LT_p
            if  (0 == ARES)
            { HOV_INC(Ares,  k1)
              HOV_INC(Aarg1, k1)
              HOV_INC(Aarg2, k1)
            }
            else
            { aTmp = ARES;
              ARES_INC = 0.0;
              MAXINC(AARG1,aTmp);
              MAXINC(AARG2,aTmp);
              AARG2_INC_O; AARG1_INC_O;
              FOR_0_LE_i_LT_k
              { aTmp = ARES;
                ARES_INC = 0.0;
                AARG1_INC += aTmp;
                AARG2_INC -= aTmp;
              }
            }
   	  get_taylors(res,k);
        }
	break;

/*--------------------------------------------------------------------------*/
      case dot_av_av:                                          /* dot_av_av */
        res  = get_locint_r();
        size = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

        get_taylors(res,k);

        /* save Ares to Atemp */
        ASSIGN_A( Aqo,  Atemp)
	ASSIGN_A( Ares, A[res])
        FOR_0_LE_l_LT_pk1
	{ AQO_INC = ARES;
          ARES_INC = 0.0; 
	}

	for (ls=0; ls<size; ls++)
	{ /* code for mult_a_a  */
	  ASSIGN_A( Aarg2, A[arg2])
	  ASSIGN_A( Aarg1, A[arg1])
          ASSIGN_A( Aqo,   Atemp)
	  ASSIGN_T( Targ1, T[arg1])
	  ASSIGN_T( Targ2, T[arg2])

          FOR_0_LE_l_LT_p
            if (0 == AQO)
            { HOV_INC(Aarg1, k1)
              HOV_INC(Aarg2, k1)
              HOV_INC(Aqo,   k1)
            }
            else
            { comp = (AQO > 2.0) ? AQO : 2.0 ;
              MAXINC(AARG1,comp);
              MAXINC(AARG2,comp);
              AARG1_INC_O; AARG2_INC_O; AQO_INC_O;
                  
              inconv(k,Aqo,Targ1,Aarg2);
              inconv(k,Aqo,Targ2,Aarg1);
                  
              HOV_INC(Aqo,   k)
              HOV_INC(Aarg1, k)
              HOV_INC(Aarg2, k)
            }
          arg1++; arg2++;
	} 
        break;

/*--------------------------------------------------------------------------*/
      case mult_a_av:                                          /* mult_a_av */
        res  = get_locint_r();
        size = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

        /* olvo 980929 new strategy to check for overwrites 
           (changes computation order) */
        if ((arg2 >= res) && (arg2 < res+size)) 
	{ /* FIRST compute the case: res==arg2 */
	  /* simplified code for mult_a_a */
   	  get_taylors(arg2,k);

	  ASSIGN_A( Aarg1, A[arg1+res-arg2])
	  ASSIGN_A( Aarg2, A[arg2])
	  ASSIGN_T( Targ2, T[arg2])
	  ASSIGN_T( Targ1, T[arg1+res-arg2]) 

          FOR_0_LE_l_LT_p
            if (0 == AARG2)
            { HOV_INC(Aarg1, k1)
              HOV_INC(Aarg2, k1)
            }
            else
            { MAXINC(AARG1,AARG2);
              AARG1_INC_O; AARG2_INC_O;

              copyAndZeroset(k,Aarg2,Atemp);
              inconv(k,Atemp,Targ1,Aarg2);
              inconv(k,Atemp,Targ2,Aarg1);

              HOV_INC(Aarg1, k)
              HOV_INC(Aarg2, k)
            }         
	}

        res  += size;
        arg1 += size;
	for (ls=size; ls>0; ls--)
	{ arg1--;    /* Location of rght hnd side vectore[l]  */
	  res--;     /* Location of the result */
	    
          if (res == arg2) /* NOW skip this case */
            continue;

	  /* code for mult_a_a */
   	  get_taylors(res,k);

          ASSIGN_A( Ares,  A[res])
	  ASSIGN_A( Aarg1, A[arg1])
	  ASSIGN_A( Aarg2, A[arg2])
	  ASSIGN_T( Targ2, T[arg2])
	  ASSIGN_T( Targ1, T[arg1]) 

          FOR_0_LE_l_LT_p
            if (0 == ARES)
            { HOV_INC(Aarg1, k1)
              HOV_INC(Aarg2, k1)
              HOV_INC(Ares,  k1)
            }
            else
            { comp = (ARES > 2.0) ? ARES : 2.0 ;
              ARES_INC = 0.0;
              MAXINC(AARG1,comp);
              MAXINC(AARG2,comp);
              AARG1_INC_O; AARG2_INC_O;

              copyAndZeroset(k,Ares,Atemp);
              inconv(k,Atemp,Targ1,Aarg2);
              inconv(k,Atemp,Targ2,Aarg1);

              HOV_INC(Ares,  k)
              HOV_INC(Aarg1, k)
              HOV_INC(Aarg2, k)
            }         
	}
	break;

/*--------------------------------------------------------------------------*/
      case mult_d_av:                                          /* mult_d_av */
        res   = get_locint_r();
        size  = get_locint_r();
        arg   = get_locint_r();
        coval = get_val_r();

        res += size;
        arg += size;
	for (ls=size; ls>0; ls--)
	{ arg--;     /* Location on the right-hand-side */
	  res--;     /* location of the result */
	  /* coval = Fixed double value */
	    
          /* code for mult_d_a */
	  ASSIGN_A( Ares, A[res])
	  ASSIGN_A( Aarg, A[arg])

          FOR_0_LE_l_LT_p
            if (0 == ARES)
            { HOV_INC(Ares, k1)
              HOV_INC(Aarg, k1)
            }
            else
            { aTmp = ARES;
              ARES_INC = 0.0;
              MAXINC(AARG,aTmp);
              AARG_INC_O;
              FOR_0_LE_i_LT_k  
              { aTmp = ARES;
                ARES_INC = 0.0;
                AARG_INC += coval * aTmp;
              }
            }

 	  get_taylors(res,k);
	}
	break;

/*--------------------------------------------------------------------------*/
      case div_av_a:                                            /* div_av_a */
        res  = get_locint_r();
        size = get_locint_r();
        arg2 = get_locint_r();
        arg1 = get_locint_r();

       /* olvo 980929 new strategy to check for overwrites 
           (changes computation order) */
        if ((arg2 >= res) && (arg2 < res+size)) 
	{ /* FIRST compute the case: res==arg2 */
	  /* simplified code for div_a_a */
	  ASSIGN_A( Aarg2, A[arg2])
	  ASSIGN_A( Aarg1, A[arg1+res-arg2])
	  ASSIGN_T( Targ2, T[arg2])

	  for (i=0; i<k; i++)
            Ttemp2[i] = Targ2[i];
          Tres = Ttemp2;
        
 	  get_taylors(arg2,k);

VEC_COMPUTED_INIT
          FOR_0_LE_l_LT_p
            if (0 == AARG2)
	    { HOV_INC(Aarg1, k1)
	      HOV_INC(Aarg2, k1)
	    }
	    else
	    { MAXINC(AARG2,3.0);
	      MAXINC(AARG1,AARG2);
	      AARG1_INC_O; AARG2_INC_O;

VEC_COMPUTED_CHECK
              recipr(k,1.0,Targ2,Ttemp);
              conv(k,Ttemp,Tres,Atemp2);
VEC_COMPUTED_END
              copyAndZeroset(k,Aarg2,Atemp);
              inconv(k,Atemp,Ttemp,Aarg1);
              deconv(k,Atemp,Atemp2,Aarg2);

              HOV_INC(Aarg1, k)
              HOV_INC(Aarg2, k)
            }
	}

	res  += size;
        arg1 += size;
	for (ls=size; ls>0; ls--)
	{ arg1--;    /* Location of right-hand-side vector[l] */
	  res--;     /* Location of the result */
	    
          if (res == arg2) /* NOW skip this case */
            continue;

	  /* code for div_a_a */
	  ASSIGN_A( Ares,  A[res])
	  ASSIGN_A( Aarg2, A[arg2])
	  ASSIGN_A( Aarg1, A[arg1])
	  ASSIGN_T( Tres,  T[res])
	  ASSIGN_T( Targ2, T[arg2])

VEC_COMPUTED_INIT
          FOR_0_LE_l_LT_p
            if (0 == ARES)
	    { HOV_INC(Ares,  k1)
	      HOV_INC(Aarg1, k1)
	      HOV_INC(Aarg2, k1)
	    }
	    else
	    { aTmp = ARES;
              ARES_INC = 0.0;
              MAXINC(AARG1,3.0);
	      MAXINC(AARG1,aTmp);
	      MAXINC(AARG2,3.0);
	      MAXINC(AARG2,aTmp);
	      AARG1_INC_O; AARG2_INC_O;

VEC_COMPUTED_CHECK
              recipr(k,1.0,Targ2,Ttemp);
              conv(k,Ttemp,Tres,Atemp2);
VEC_COMPUTED_END
              copyAndZeroset(k,Ares,Atemp);
              inconv(k,Atemp,Ttemp,Aarg1);
              deconv(k,Atemp,Atemp2,Aarg2);

              HOV_INC(Ares,  k)
              HOV_INC(Aarg1, k)
              HOV_INC(Aarg2, k)
            }

 	  get_taylors(res,k);
	}
	break;


/****************************************************************************/
/*                                                               SUBSCRIPTS */

/*--------------------------------------------------------------------------*/
      case subscript:                                          /* subscript */
        res   = get_locint_r();
        arg1  = get_locint_r();
        arg2  = get_locint_r(); 
        coval = get_val_r();

        arg = arg2 + (int)(T[arg1][0]);

	/* olvo 980721 new nl */
        get_taylors(res,k);

	ASSIGN_A( Aarg, A[arg])
	ASSIGN_A( Ares, A[res])

        FOR_0_LE_l_LT_p
          if  (0 == ARES)
	  { HOV_INC(Ares, k1)
	    HOV_INC(Aarg, k1)
	  }
	  else 
	  { if ((int)(coval) != (int)(T[arg1][0]))
              MINDEC(ret_c,2);
	    aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
	    AARG_INC_O;

	    FOR_0_LE_i_LT_k
	    { AARG_INC += ARES;
              if (arg != res)
		ARES_INC = 0;
              else 
                ARES_INC_O;
            }
	  }
	break;

/*--------------------------------------------------------------------------*/
      case subscript_l:                                      /* subscript_l */
        arg   = get_locint_r();
        arg1  = get_locint_r();
        arg2  = get_locint_r(); 
        coval = get_val_r();

        res   = arg2 + (int)(T[arg1][0]);

	get_taylors(res,k);

	ASSIGN_A( Aarg, A[arg])
	ASSIGN_A( Ares, A[res])

	FOR_0_LE_l_LT_p
          if  (0==ARES)
	  { HOV_INC(Ares, k1)
	    HOV_INC(Aarg, k1)
	  }
	  else 
	  { if ((int)(coval) != (int)(T[arg1][0]))
              MINDEC(ret_c,2);
	    aTmp = ARES;
            ARES_INC = 0.0;
            MAXINC(AARG,aTmp);
	    AARG_INC_O;

	    FOR_0_LE_i_LT_k
	    { AARG_INC += ARES;
              if (res != arg)
		ARES_INC = 0;
              else 
                ARES_INC_O;
            }
	  }
	break;
      
/*--------------------------------------------------------------------------*/
      case subscript_ld:                                    /* subscript_ld */
        arg1  = get_locint_r();
        arg2  = get_locint_r(); 
        coval = get_val_r();
        coval = get_val_r();

        arg   = arg2 + (int)(T[arg1][0]);

        get_taylors(arg,k);

        if((int)(coval)!=(int)(T[arg1][0]))
          MINDEC(ret_c,2);

	ASSIGN_A( Aarg, A[arg]);

        FOR_0_LE_l_LT_pk1
	  AARG_INC = 0.0;

	break;
       
/*--------------------------------------------------------------------------*/
      case m_subscript:                                      /* m_subscript */
        res   = get_locint_r();
        size  = get_locint_r();
        arg1  = get_locint_r();
        arg2  = get_locint_r(); 
        coval = get_val_r();

        res += size;
        arg  = arg2 + ((int)(T[arg1][0]) + 1)*size;
        for (ls=size; ls>0; ls--)
        { res--; arg--;
          
	  /* olvo 980721 new nl */ 
          get_taylors(res,k);

          ASSIGN_A( Aarg, A[arg])
	  ASSIGN_A( Ares, A[res])

          FOR_0_LE_l_LT_p
            if  (0 == ARES)
            { HOV_INC(Ares, k1)
              HOV_INC(Aarg, k1)
            }
            else 
            { if ((int)(coval)!=(int)(T[arg1][0]))
                MINDEC(ret_c,2);
              aTmp = ARES;
              ARES_INC = 0.0;
              MAXINC(AARG,aTmp);
              AARG_INC_O;

              FOR_0_LE_i_LT_k
              { AARG_INC += ARES;
                if (arg != res)
                  ARES_INC = 0;
                else
                  ARES_INC_O;
              }
            }
        } 
	break;

/*--------------------------------------------------------------------------*/
      case m_subscript_l:                                  /* m_subscript_l */
        arg   = get_locint_r();
        size  = get_locint_r();
        arg1  = get_locint_r();
        arg2  = get_locint_r(); 
        coval = get_val_r();

        res = arg2 + ((int)(T[arg1][0]) + 1)*size;
        arg += size;
        for (ls=size; ls>0; ls--)
        { arg--; res--;

          get_taylors(res,k);

          ASSIGN_A( Aarg, A[arg])
          ASSIGN_A( Ares, A[res])

          FOR_0_LE_l_LT_p
            if  (0 == ARES)
            { HOV_INC(Ares, k1)
              HOV_INC(Aarg, k1)
            }
            else 
            { if ((int)(coval)!=(int)(T[arg1][0]))
                MINDEC(ret_c,2);
              aTmp = ARES;
              ARES_INC = 0.0;
              MAXINC(AARG,aTmp);
              AARG_INC_O;

              FOR_0_LE_i_LT_k
              { AARG_INC += ARES;
                if (arg != res)
                  ARES_INC = 0;
                else
                  ARES_INC;
              }
            }
        } 
	break;

/*--------------------------------------------------------------------------*/
      case m_subscript_ld:                                /* m_subscript_ld */
        size   = get_locint_r();
        arg    = get_locint_r();
        arg1   = get_locint_r();
        arg2   = get_locint_r(); 
        /* olvo 980702 changed n2l */
        d      = get_val_v_r(size);
        coval  = get_val_r();

        if ((int)(coval) != (int)(T[arg1][0]))
          MINDEC(ret_c,2);

        res = arg2 + ((int)(T[arg1][0]) + 1)*size + arg;
        for (ls=size; ls>0; ls--)
        { res--;
          
          get_taylors(res,k);
          
          ASSIGN_A( Ares, A[res])
        
          FOR_0_LE_l_LT_pk1
            ARES_INC = 0.0;
        } 
	break;


/****************************************************************************/
/*                                                          REMAINING STUFF */

/*--------------------------------------------------------------------------*/
      case take_stock_op:                                  /* take_stock_op */
        res = get_locint_r();
        size = get_locint_r();
        d = get_val_v_r(size);

        res += size;
	for (ls=size;ls>0;ls--)
	{ res--;

          ASSIGN_A( Ares, A[res])

          FOR_0_LE_l_LT_pk1
	    ARES_INC = 0.0;
	}
#ifdef _HOV_
	/* olvo 980804 necessary or not?: reset_val_r(); */
        /* olvo 980623 ??? why that ? */
#endif
	break;
   
/*--------------------------------------------------------------------------*/
      case death_not:                                          /* death_not */
        arg2 = get_locint_r();
        arg1 = get_locint_r();

	for (j=arg1;j<=arg2;j++)
	{ ASSIGN_A(Aarg1, A[j])

	  FOR_0_LE_l_LT_p
            for (i=0; i<k1; i++)
              AARG1_INC = 0.0;
	  
          get_taylors(j,k);
	}
	break;

/*--------------------------------------------------------------------------*/
      default:                                                   /* default */
	  /*             Die here, we screwed up     */ 

        fprintf(DIAG_OUT,"ADOL-C fatal error in " GENERATED_FILENAME " (" 
                __FILE__
                ") : no such operation %d\n", operation);
	exit(-1);
	break;
      } /* endswitch */

      /* Get the next operation */
      operation=get_op_r();
    }

  end_sweep();
  return ret_c;
}


/****************************************************************************/
/*                                                               THAT'S ALL */
#ifdef __cplusplus
}
#endif

#undef _ADOLC_SRC_
#undef _HO_REV_C_




















