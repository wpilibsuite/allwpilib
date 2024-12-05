// -*- mode: C; c-basic-offset: 2 -*-
// Copyright 2011 Oblong Industries
//           2017-2018 Dima Kogan <dima@secretsauce.net>
// License: GNU LGPL <http://www.gnu.org/licenses>.

// Apparently I need this in MSVC to get constants
#define _USE_MATH_DEFINES

#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <stdlib.h>
#include "dogleg.h"

#if (CHOLMOD_VERSION > (CHOLMOD_VER_CODE(2,2))) && (CHOLMOD_VERSION < (CHOLMOD_VER_CODE(4,0)))
#include <suitesparse/cholmod_function.h>
#endif


// Any non-vnlog bit mask
#define DOGLEG_DEBUG_OTHER_THAN_VNLOG (~DOGLEG_DEBUG_VNLOG)
#define SAY_NONEWLINE(fmt, ...) fprintf(stderr, "libdogleg at %s:%d: " fmt, __FILE__, __LINE__, ## __VA_ARGS__)
#define SAY(fmt, ...)           do {  SAY_NONEWLINE(fmt, ## __VA_ARGS__); fprintf(stderr, "\n"); } while(0)

// This REQUIRES that a "dogleg_solverContext_t* ctx" be available
#define SAY_IF_VERBOSE(fmt,...) do { if( ctx->parameters->dogleg_debug & DOGLEG_DEBUG_OTHER_THAN_VNLOG ) SAY(fmt, ##__VA_ARGS__); } while(0)

// I do this myself because I want this to be active in all build modes, not just !NDEBUG
#define ASSERT(x) do { if(!(x)) { SAY("ASSERTION FAILED: " #x " is not true"); exit(1); } } while(0)

// used to consolidate the casts
#define P(A, index) ((unsigned int*)((A)->p))[index]
#define I(A, index) ((unsigned int*)((A)->i))[index]
#define X(A, index) ((double*      )((A)->x))[index]


//////////////////////////////////////////////////////////////////////////////////////////
// vnlog debugging stuff
//
// This is used if the user calls dogleg_setDebug(DOGLEG_DEBUG_VNLOG | stuff)
//////////////////////////////////////////////////////////////////////////////////////////
#define VNLOG_DEBUG_STEP_TYPE_LIST(_)           \
  _(STEPTYPE_CAUCHY,       "cauchy")            \
  _(STEPTYPE_GAUSSNEWTON,  "gaussnewton")       \
  _(STEPTYPE_INTERPOLATED, "interpolated")      \
  _(STEPTYPE_FAILED,       "failed")
#define VNLOG_DEBUG_STEP_TYPE_NAME_COMMA(name,shortname) name,
typedef enum { VNLOG_DEBUG_STEP_TYPE_LIST(VNLOG_DEBUG_STEP_TYPE_NAME_COMMA)
               STEPTYPE_UNINITIALIZED } vnlog_debug_step_type_t;
#define VNLOG_DEBUG_FIELDS(_)                                      \
  _(double,                  norm2x_before,         INFINITY)      \
  _(double,                  norm2x_after,          INFINITY)      \
  _(double,                  step_len_cauchy,       INFINITY)      \
  _(double,                  step_len_gauss_newton, INFINITY)      \
  _(double,                  step_len_interpolated, INFINITY)      \
  _(double,                  k_cauchy_to_gn,        INFINITY)      \
  _(double,                  step_len,              INFINITY)      \
  _(vnlog_debug_step_type_t, step_type,             STEPTYPE_UNINITIALIZED) \
  _(double,                  step_direction_change_deg, INFINITY)      \
  _(double,                  expected_improvement,  INFINITY)      \
  _(double,                  observed_improvement,  INFINITY)      \
  _(double,                  rho,                   INFINITY)      \
  _(double,                  trustregion_before,    INFINITY)      \
  _(double,                  trustregion_after,     INFINITY)
static struct vnlog_debug_data_t
{
#define VNLOG_DEBUG_DECLARE_FIELD(type, name, initialvalue) type name;
  VNLOG_DEBUG_FIELDS(VNLOG_DEBUG_DECLARE_FIELD)
} vnlog_debug_data;
static void vnlog_debug_reset(void)
{
#define VNLOG_DEBUG_RESET_FIELD(type, name, initialvalue) vnlog_debug_data.name = initialvalue;
  VNLOG_DEBUG_FIELDS(VNLOG_DEBUG_RESET_FIELD);
}
static void vnlog_debug_emit_legend(void)
{
#define VNLOG_DEBUG_SPACE_FIELD_NAME(type, name, initialvalue) " " #name

  vnlog_debug_reset();
  printf("# iteration step_accepted" VNLOG_DEBUG_FIELDS(VNLOG_DEBUG_SPACE_FIELD_NAME) "\n");
}

static void vnlog_emit_double(double x)
{
  if(x == INFINITY) printf("- ");
  else              printf("%g ", x);
}

static void vnlog_emit_int(int x)
{
  if(x == -1) printf("- ");
  else        printf("%d ", x);
}

static void vnlog_emit_vnlog_debug_step_type_t(vnlog_debug_step_type_t x)
{
#define VNLOG_DEBUG_STEP_TYPE_SWITCH_EMIT(name,shortname) case name: printf(shortname " "); break;
  switch(x)
  {
    VNLOG_DEBUG_STEP_TYPE_LIST(VNLOG_DEBUG_STEP_TYPE_SWITCH_EMIT)
  default: printf("- ");
  }
}
static void vnlog_debug_emit_record(int iteration, int step_accepted)
{
#define VNLOG_DEBUG_EMIT_FIELD(type, name, initialvalue) vnlog_emit_ ## type(vnlog_debug_data.name);

  printf("%d %d ", iteration, step_accepted);
  VNLOG_DEBUG_FIELDS(VNLOG_DEBUG_EMIT_FIELD);
  printf("\n");
  fflush(stdout);
  vnlog_debug_reset();
}

// Default parameters. Used only by the original API, which uses a global set of
// parameters
#define PARAMETERS_DEFAULT                      \
  {                                             \
    .max_iterations                 = 100,      \
    .dogleg_debug                   = 0,        \
    .trustregion0                   = 1.0e3,    \
    .trustregion_decrease_factor    = 0.1,      \
    .trustregion_decrease_threshold = 0.25,     \
    .trustregion_increase_factor    = 2,        \
    .trustregion_increase_threshold = 0.75,     \
    .Jt_x_threshold                 = 1e-8,     \
    .update_threshold               = 1e-8,     \
    .trustregion_threshold          = 1e-8      \
  }

static const dogleg_parameters2_t parameters_default = PARAMETERS_DEFAULT;
static       dogleg_parameters2_t parameters_global  = PARAMETERS_DEFAULT;
void dogleg_getDefaultParameters(dogleg_parameters2_t* parameters)
{
  *parameters = parameters_default;
}

// if I ever see a singular JtJ, I factor JtJ + LAMBDA*I from that point on
#define LAMBDA_INITIAL 1e-10

// these parameters likely should be messed with
void dogleg_setDebug(int debug)
{
  parameters_global.dogleg_debug = debug;
}
void dogleg_setInitialTrustregion(double t)
{
  parameters_global.trustregion0 = t;
}
void dogleg_setThresholds(double Jt_x, double update, double trustregion)
{
  if(Jt_x > 0.0)        parameters_global.Jt_x_threshold        = Jt_x;
  if(update > 0.0)      parameters_global.update_threshold      = update;
  if(trustregion > 0.0) parameters_global.trustregion_threshold = trustregion;
}

// these parameters likely should not be messed with.
void dogleg_setMaxIterations(int n)
{
  parameters_global.max_iterations = n;
}
void dogleg_setTrustregionUpdateParameters(double downFactor, double downThreshold,
                                           double upFactor,   double upThreshold)
{
  parameters_global.trustregion_decrease_factor    = downFactor;
  parameters_global.trustregion_decrease_threshold = downThreshold;
  parameters_global.trustregion_increase_factor    = upFactor;
  parameters_global.trustregion_increase_threshold = upThreshold;
}




//////////////////////////////////////////////////////////////////////////////////////////
// general boring linear algebra stuff
// should really come from BLAS or libminimath
//////////////////////////////////////////////////////////////////////////////////////////
static double norm2(const double* x, unsigned int n)
{
  double result = 0;
  for(unsigned int i=0; i<n; i++)
    result += x[i]*x[i];
  return result;
}
static double inner(const double* x, const double* y, unsigned int n)
{
  double result = 0;
  for(unsigned int i=0; i<n; i++)
    result += x[i]*y[i];
  return result;
}

static double inner_withstride(const double* x, const double* y, unsigned int n, unsigned int stride)
{
  double result = 0;
  for(unsigned int i=0; i<n*stride; i+=stride)
    result += x[i]*y[i];
  return result;
}
// JtJ += outer(j,j). JtJ is packed, upper-triangular (lower-triangular as far
// as LAPACK is concerned)
static void accum_outerproduct_packed( double* JtJ, const double* j, int n )
{
  int iJtJ=0;
  for(int i1=0; i1<n; i1++)
    for(int i0=i1; i0<n; i0++, iJtJ++)
      JtJ[iJtJ] += j[i0]*j[i1];
}
static void vec_copy_scaled(double* dest,
                            const double* v, double scale, int n)
{
  for(int i=0; i<n; i++)
    dest[i] = scale * v[i];
}

static void vec_add(double* dest,
                    const double* v0, const double* v1, int n)
{
  for(int i=0; i<n; i++)
    dest[i] = v0[i] + v1[i];
}

static void vec_accum(double* dest,
                      const double* v, int n)
{
  for(int i=0; i<n; i++)
    dest[i] += v[i];
}
static void vec_negate(double* v, int n)
{
  for(int i=0; i<n; i++)
    v[i] *= -1.0;
}

// It would be nice to use the CHOLMOD implementation of these, but they're
// licensed under the GPL
static void mul_spmatrix_densevector(double* dest,
                                     const cholmod_sparse* A, const double* x)
{
  memset(dest, 0, sizeof(double) * A->nrow);
  for(unsigned int i=0; i<A->ncol; i++)
  {
    for(unsigned int j=P(A, i); j<P(A, i+1); j++)
    {
      int row = I(A, j);
      dest[row] += x[i] * X(A, j);
    }
  }
}
static double norm2_mul_spmatrix_t_densevector(const cholmod_sparse* At, const double* x)
{
  // computes norm2(transpose(At) * x). For each row of A (col of At) I
  // compute that element of A*x, and accumulate it immediately towards the
  // norm
  double result = 0.0;

  for(unsigned int i=0; i<At->ncol; i++)
  {
    double dotproduct = 0.0;
    for(unsigned int j=P(At, i); j<P(At, i+1); j++)
    {
      int row = I(At, j);
      dotproduct += x[row] * X(At, j);
    }
    result += dotproduct * dotproduct;
  }

  return result;
}

// transpose(A)*x
static void mul_matrix_t_densevector(double* dest,
                                     const double* A, const double* x,
                                     int Nrows, int Ncols)
{
  memset(dest, 0, sizeof(double) * Ncols);
  for(int i=0; i<Ncols; i++)
    for(int j=0; j<Nrows; j++)
      dest[i] += A[i + j*Ncols]*x[j];
}
static double norm2_mul_matrix_vector(const double* A, const double* x, int Nrows, int Ncols)
{
  // computes norm2(A * x). For each row of A I compute that element of A*x, and
  // accumulate it immediately towards the norm
  double result = 0.0;

  for(int i=0; i<Nrows; i++)
  {
    double dotproduct = inner(x, &A[i*Ncols], Ncols);
    result += dotproduct * dotproduct;
  }

  return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
// routines for gradient testing
//////////////////////////////////////////////////////////////////////////////////////////
#define GRADTEST_DELTA 1e-6
static double getGrad(unsigned int var, int meas, const cholmod_sparse* Jt)
{
  int row     = P(Jt, meas);
  int rownext = P(Jt, meas+1);

  // this could be done more efficiently with bsearch()
  for(int i=row; i<rownext; i++)
  {
    if(I(Jt,i) == var)
      return X(Jt,i);
  }

  // This gradient is not in my sparse matrix, so its value is 0.0
  return 0.0;
}
static double getGrad_dense(unsigned int var, int meas, const double* J, int Nstate)
{
  return J[var + meas*Nstate];
}

static
void _dogleg_testGradient(unsigned int var, const double* p0,
                          unsigned int Nstate, unsigned int Nmeas, unsigned int NJnnz,
                          dogleg_callback_t* f, void* cookie)
{
  int is_sparse = NJnnz > 0;

  double* x0 = static_cast<double*>(malloc(Nmeas  * sizeof(double)));
  double* x  = static_cast<double*>(malloc(Nmeas  * sizeof(double)));
  double* p  = static_cast<double*>(malloc(Nstate * sizeof(double)));
  ASSERT(x0);
  ASSERT(x);
  ASSERT(p);

  memcpy(p, p0, Nstate * sizeof(double));


  cholmod_common _cholmod_common;
  cholmod_sparse* Jt;
  cholmod_sparse* Jt0;
  double* J_dense  = NULL; // setting to NULL to pacify compiler's "uninitialized" warnings
  double* J_dense0 = NULL; // setting to NULL to pacify compiler's "uninitialized" warnings


  // This is a plain text table, that can be easily parsed with "vnlog" tools
  printf("# ivar imeasurement gradient_reported gradient_observed error error_relative\n");

  if( is_sparse )
  {
    if( !cholmod_start(&_cholmod_common) )
    {
      SAY( "Couldn't initialize CHOLMOD");
      return;
    }

    Jt  = cholmod_allocate_sparse(Nstate, Nmeas, NJnnz,
                                  1, // sorted
                                  1, // packed,
                                  0, // NOT symmetric
                                  CHOLMOD_REAL,
                                  &_cholmod_common);
    Jt0 = cholmod_allocate_sparse(Nstate, Nmeas, NJnnz,
                                  1, // sorted
                                  1, // packed,
                                  0, // NOT symmetric
                                  CHOLMOD_REAL,
                                  &_cholmod_common);

    p[var] -= GRADTEST_DELTA/2.0;
    (*f)(p, x0, Jt0, cookie);
    p[var] += GRADTEST_DELTA;
    (*f)(p, x,  Jt,  cookie);
    p[var] -= GRADTEST_DELTA/2.0;
  }
  else
  {
    J_dense  = static_cast<double*>(malloc( Nmeas * Nstate * sizeof(J_dense[0]) ));
    J_dense0 = static_cast<double*>(malloc( Nmeas * Nstate * sizeof(J_dense[0]) ));

    dogleg_callback_dense_t* f_dense = (dogleg_callback_dense_t*)f;
    p[var] -= GRADTEST_DELTA/2.0;
    (*f_dense)(p, x0, J_dense0, cookie);
    p[var] += GRADTEST_DELTA;
    (*f_dense)(p, x,  J_dense,  cookie);
    p[var] -= GRADTEST_DELTA/2.0;
  }


  for(unsigned int i=0; i<Nmeas; i++)
  {
    // estimated gradients at the midpoint between x and x0
    double g_observed = (x[i] - x0[i]) / GRADTEST_DELTA;
    double g_reported;
    if( is_sparse )
      g_reported = (getGrad(var, i, Jt0) + getGrad(var, i, Jt)) / 2.0;
    else
      g_reported = (getGrad_dense(var, i, J_dense0, Nstate) + getGrad_dense(var, i, J_dense, Nstate)) / 2.0;

    double g_sum_abs = fabs(g_reported) + fabs(g_observed);
    double g_abs_err = fabs(g_reported - g_observed);

    printf( "%d %d %.6g %.6g %.6g %.6g\n", var, i,
            g_reported, g_observed, g_abs_err,

            g_sum_abs == 0.0 ? 0.0 : (g_abs_err / ( g_sum_abs / 2.0 )));
  }

  if( is_sparse )
  {
    cholmod_free_sparse(&Jt,  &_cholmod_common);
    cholmod_free_sparse(&Jt0, &_cholmod_common);
    cholmod_finish(&_cholmod_common);
  }
  else
  {
    free(J_dense);
    free(J_dense0);
  }

  free(x0);
  free(x);
  free(p);
}
void dogleg_testGradient(unsigned int var, const double* p0,
                         unsigned int Nstate, unsigned int Nmeas, unsigned int NJnnz,
                         dogleg_callback_t* f, void* cookie)
{
  if( NJnnz == 0 )
  {
    SAY( "I must have NJnnz > 0, instead I have %d", NJnnz);
    return;
  }
  return _dogleg_testGradient(var, p0, Nstate, Nmeas, NJnnz, f, cookie);
}
void dogleg_testGradient_dense(unsigned int var, const double* p0,
                               unsigned int Nstate, unsigned int Nmeas,
                               dogleg_callback_dense_t* f, void* cookie)
{
  return _dogleg_testGradient(var, p0, Nstate, Nmeas, 0, (dogleg_callback_t*)f, cookie);
}


//////////////////////////////////////////////////////////////////////////////////////////
// solver routines
//////////////////////////////////////////////////////////////////////////////////////////

static void computeCauchyUpdate(dogleg_operatingPoint_t* point,
                                const dogleg_solverContext_t* ctx)
{
  // I already have this data, so don't need to recompute
  if(!point->updateCauchy_valid)
  {
    point->updateCauchy_valid = 1;

    // I look at a step in the steepest direction that minimizes my
    // quadratic error function (Cauchy point). If this is past my trust region,
    // I move as far as the trust region allows along the steepest descent
    // direction. My error function is F=norm2(f(p)). dF/dP = 2*ft*J.
    // This is proportional to Jt_x, which is thus the steepest ascent direction.
    //
    // Thus along this direction we have F(k) = norm2(f(p + k Jt_x)). The Cauchy
    // point is where F(k) is at a minimum:
    // dF_dk = 2 f(p + k Jt_x)t  J Jt_x ~ (x + k J Jt_x)t J Jt_x =
    // = xt J Jt x + k xt J Jt J Jt x = norm2(Jt x) + k norm2(J Jt x)
    // dF_dk = 0 -> k= -norm2(Jt x) / norm2(J Jt x)
    // Summary:
    // the steepest direction is parallel to Jt*x. The Cauchy point is at
    // k*Jt*x where k = -norm2(Jt*x)/norm2(J*Jt*x)
    double norm2_Jt_x       = norm2(point->Jt_x, ctx->Nstate);
    double norm2_J_Jt_x     = ctx->is_sparse ?
      norm2_mul_spmatrix_t_densevector(point->Jt, point->Jt_x) :
      norm2_mul_matrix_vector         (point->J_dense, point->Jt_x, ctx->Nmeasurements, ctx->Nstate);
    double k                = -norm2_Jt_x / norm2_J_Jt_x;

    point->updateCauchy_lensq = k*k * norm2_Jt_x;

    vec_copy_scaled(point->updateCauchy,
                    point->Jt_x, k, ctx->Nstate);
    SAY_IF_VERBOSE( "cauchy step size %.6g", sqrt(point->updateCauchy_lensq));
  }

  if( ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG )
    vnlog_debug_data.step_len_cauchy = sqrt(point->updateCauchy_lensq);
}

// LAPACK prototypes for a packed cholesky factorization and a linear solve
// using that factorization, respectively
extern "C" {
int dpptrf_(char* uplo, int* n, double* ap,
            int* info, int uplo_len);
int dpptrs_(char* uplo, int* n, int* nrhs,
            double* ap, double* b, int* ldb, int* info,
            int uplo_len);
}


void dogleg_computeJtJfactorization(dogleg_operatingPoint_t* point, dogleg_solverContext_t* ctx)
{
  // I already have this data, so don't need to recompute
  if(point->updateGN_valid)
    return;

  if( ctx->is_sparse )
  {
    // I'm assuming the pattern of zeros will remain the same throughout, so I
    // analyze only once
    if(ctx->factorization == NULL)
    {
      ctx->factorization = cholmod_analyze(point->Jt, &ctx->common);
      ASSERT(ctx->factorization != NULL);
    }

    while(1)
    {
      if( ctx->lambda == 0.0 )
        ASSERT( cholmod_factorize(point->Jt, ctx->factorization, &ctx->common) );
      else
      {
        double beta[] = { ctx->lambda, 0 };
        ASSERT( cholmod_factorize_p(point->Jt, beta, NULL, 0,
                                    ctx->factorization, &ctx->common) );
      }

      if(ctx->factorization->minor == ctx->factorization->n)
        break;

      // singular JtJ. Raise lambda and go again
      if( ctx->lambda == 0.0) ctx->lambda = LAMBDA_INITIAL;
      else                    ctx->lambda *= 10.0;
      ASSERT( isfinite(ctx->lambda) );

      SAY_IF_VERBOSE( "singular JtJ. Have rank/full rank: %zd/%d. Adding %g I from now on",
                      ctx->factorization->minor, ctx->Nstate, ctx->lambda);
    }
  }
  else
  {
    if(ctx->factorization_dense == NULL)
    {
      // Need to store symmetric JtJ, so I only need one triangle of it
      ctx->factorization_dense = static_cast<double*>(malloc( ctx->Nstate * (ctx->Nstate+1) / 2 *
                                         sizeof( ctx->factorization_dense[0])));
      ASSERT(ctx->factorization_dense);
    }

    while(1)
    {
      // I construct my JtJ. JtJ is packed and stored row-first. I have two
      // equivalent implementations. The one enabled here is maybe a bit faster,
      // but it's definitely clearer
#if 1
      memset(ctx->factorization_dense,
             0,
             ctx->Nstate*(ctx->Nstate+1)/2*sizeof(ctx->factorization_dense[0]));
      for(int i=0; i<ctx->Nmeasurements; i++)
        accum_outerproduct_packed( ctx->factorization_dense, &point->J_dense[ctx->Nstate*i],
                                   ctx->Nstate );
      if( ctx->lambda > 0.0 )
      {
        int iJtJ=0;
        for(int i1=0; i1<ctx->Nstate; i1++)
        {
          ctx->factorization_dense[iJtJ] += ctx->lambda;
          iJtJ                           += ctx->Nstate-i1;
        }
      }
#else
      int iJtJ = 0;
      for(int i1=0; i1<ctx->Nstate; i1++)
      {
        #error this does not work. overwritten in the following loop
        ctx->factorization_dense[iJtJ] += ctx->lambda;

        for(int i0=i1; i0<ctx->Nstate; i0++, iJtJ++)
          ctx->factorization_dense[iJtJ] = inner_withstride( &point->J_dense[i0],
                                                             &point->J_dense[i1],
                                                             ctx->Nmeasurements,
                                                             ctx->Nstate);
      }
#endif



      int info;
      char uplo = 'L';
      int Nstate_copy = ctx->Nstate;
      dpptrf_(&uplo, &Nstate_copy, ctx->factorization_dense,
              &info, 1);
      ASSERT(info >= 0); // we MUST either succeed or see complain of singular
      // JtJ
      if( info == 0 )
        break;

      // singular JtJ. Raise lambda and go again
      if( ctx->lambda == 0.0) ctx->lambda = LAMBDA_INITIAL;
      else                    ctx->lambda *= 10.0;

      SAY_IF_VERBOSE( "singular JtJ. Adding %g I from now on", ctx->lambda);
    }
  }
}

static void computeGaussNewtonUpdate(dogleg_operatingPoint_t* point, dogleg_solverContext_t* ctx)
{
  // I already have this data, so don't need to recompute
  if(!point->updateGN_valid)
  {
    dogleg_computeJtJfactorization(point, ctx);

    // try to factorize the matrix directly. If it's singular, add a small
    // constant to the diagonal. This constant gets larger if we keep being
    // singular
    if( ctx->is_sparse )
    {
      // solve JtJ*updateGN = Jt*x. Gauss-Newton step is then -updateGN
      cholmod_dense Jt_x_dense = {.nrow  = static_cast<size_t>(ctx->Nstate),
                                  .ncol  = 1,
                                  .nzmax = static_cast<size_t>(ctx->Nstate),
                                  .d     = static_cast<size_t>(ctx->Nstate),
                                  .x     = point->Jt_x,
                                  .xtype = CHOLMOD_REAL,
                                  .dtype = CHOLMOD_DOUBLE};

      if(point->updateGN_cholmoddense != NULL)
        cholmod_free_dense(&point->updateGN_cholmoddense, &ctx->common);

      point->updateGN_cholmoddense = cholmod_solve(CHOLMOD_A,
                                                   ctx->factorization,
                                                   &Jt_x_dense,
                                                   &ctx->common);
      vec_negate(static_cast<double*>(point->updateGN_cholmoddense->x),
                 ctx->Nstate); // should be more efficient than this later

      point->updateGN_lensq = norm2(static_cast<double*>(point->updateGN_cholmoddense->x), ctx->Nstate);
    }
    else
    {
      memcpy( point->updateGN_dense, point->Jt_x, ctx->Nstate * sizeof(point->updateGN_dense[0]));
      int info;
      char uplo = 'L';
      int nhrs = 1;
      int Nstate_copy = ctx->Nstate;
      int Nstate_copy2 = ctx->Nstate;
      dpptrs_(&uplo, &Nstate_copy, &nhrs,
              ctx->factorization_dense,
              point->updateGN_dense, &Nstate_copy2, &info, 1);
      vec_negate(point->updateGN_dense,
                 ctx->Nstate); // should be more efficient than this later

      point->updateGN_lensq = norm2(point->updateGN_dense, ctx->Nstate);
    }

    SAY_IF_VERBOSE( "gn step size %.6g", sqrt(point->updateGN_lensq));
    point->updateGN_valid = 1;
  }

  if( ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG )
    vnlog_debug_data.step_len_gauss_newton = sqrt(point->updateGN_lensq);

}

static void computeInterpolatedUpdate(double*                  update_dogleg,
                                      double*                  update_dogleg_lensq,
                                      dogleg_operatingPoint_t* point,
                                      double                   trustregion,
                                      const dogleg_solverContext_t* ctx)
{
  // I interpolate between the Cauchy-point step and the Gauss-Newton step
  // to find a step that takes me to the edge of my trust region.
  //
  // I have something like norm2(a + k*(b-a)) = dsq
  // = norm2(a) + 2*at*(b-a) * k + norm2(b-a)*k^2 = dsq
  // let c = at*(b-a), l2 = norm2(b-a) ->
  // l2 k^2 + 2*c k + norm2(a)-dsq = 0
  //
  // This is a simple quadratic equation:
  // k = (-2*c +- sqrt(4*c*c - 4*l2*(norm2(a)-dsq)))/(2*l2)
  //   = (-c +- sqrt(c*c - l2*(norm2(a)-dsq)))/l2

  // to make 100% sure the discriminant is positive, I choose a to be the
  // cauchy step.  The solution must have k in [0,1], so I much have the
  // +sqrt side, since the other one is negative
  double        dsq    = trustregion*trustregion;
  double        norm2a = point->updateCauchy_lensq;
  const double* a      = point->updateCauchy;
  const double* b      = ctx->is_sparse ? static_cast<double*>(point->updateGN_cholmoddense->x) : point->updateGN_dense;

  double l2    = 0.0;
  double neg_c = 0.0;
  for(int i=0; i<ctx->Nstate; i++)
  {
    double d = a[i] - b[i];

    l2    += d*d;
    neg_c += d*a[i];
  }

  double discriminant = neg_c*neg_c - l2* (norm2a - dsq);
  if(discriminant < 0.0)
  {
    SAY( "negative discriminant: %.6g!", discriminant);
    discriminant = 0.0;
  }
  double k = (neg_c + sqrt(discriminant))/l2;

  *update_dogleg_lensq = 0.0;
  for(int i=0; i<ctx->Nstate; i++)
  {
    update_dogleg[i] = a[i] + k*(b[i] - a[i]);
    *update_dogleg_lensq += update_dogleg[i]*update_dogleg[i];
  }

  SAY_IF_VERBOSE( "k_cauchy_to_gn %.6g, norm %.6g",
                  k,
                  sqrt(*update_dogleg_lensq));
  if(ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG)
  {
    vnlog_debug_data.step_len_interpolated = sqrt(*update_dogleg_lensq);
    vnlog_debug_data.k_cauchy_to_gn        = k;
  }
}

// takes in point->p, and computes all the quantities derived from it, storing
// the result in the other members of the operatingPoint structure. Returns
// true if the gradient-size termination criterion has been met
static int computeCallbackOperatingPoint(dogleg_operatingPoint_t* point, dogleg_solverContext_t* ctx)
{
  if( ctx->is_sparse )
  {
    (*ctx->f)(point->p, point->x, point->Jt, ctx->cookie);

    // compute Jt*x
    mul_spmatrix_densevector(point->Jt_x, point->Jt, point->x);
  }
  else
  {
    (*ctx->f_dense)(point->p, point->x, point->J_dense, ctx->cookie);

    // compute Jt*x
    mul_matrix_t_densevector(point->Jt_x, point->J_dense, point->x,
                             ctx->Nmeasurements, ctx->Nstate);
  }

  // I just got a new operating point, so the current update vectors aren't
  // valid anymore, and should be recomputed, as needed
  point->updateCauchy_valid = 0;
  point->updateGN_valid     = 0;

  // compute the 2-norm of the current error vector
  // At some point this should be changed to use the call from libminimath
  point->norm2_x = norm2(point->x, ctx->Nmeasurements);

  // If the largest absolute gradient element is smaller than the threshold,
  // we can stop iterating. This is equivalent to the inf-norm
  for(int i=0; i<ctx->Nstate; i++)
    if(fabs(point->Jt_x[i]) > ctx->parameters->Jt_x_threshold)
      return 0;
  SAY_IF_VERBOSE( "Jt_x all below the threshold. Done iterating!");

  return 1;
}
static double computeExpectedImprovement(const double* step, const dogleg_operatingPoint_t* point,
                                         const dogleg_solverContext_t* ctx)
{
  // My error function is F=norm2(f(p + step)). F(0) - F(step) =
  // = norm2(x) - norm2(x + J*step) = -2*inner(x,J*step) - norm2(J*step)
  // = -2*inner(Jt_x,step) - norm2(J*step)
  if( ctx->is_sparse )
    return
      - 2.0*inner(point->Jt_x, step, ctx->Nstate)
      - norm2_mul_spmatrix_t_densevector(point->Jt, step);
  else
    return
      - 2.0*inner(point->Jt_x, step, ctx->Nstate)
      - norm2_mul_matrix_vector(point->J_dense, step, ctx->Nmeasurements, ctx->Nstate);
}


// takes a step from the given operating point, using the given trust region
// radius. Returns the expected improvement, based on the step taken and the
// linearized x(p). If we can stop iterating, returns a negative number
static double takeStepFrom(dogleg_operatingPoint_t* pointFrom,
                           double* p_new,
                           double* step,
                           double* step_len_sq,
                           double trustregion,
                           dogleg_solverContext_t* ctx)
{
  SAY_IF_VERBOSE( "taking step with trustregion %.6g", trustregion);
  if(ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG)
  {
    vnlog_debug_data.trustregion_before = trustregion;
    vnlog_debug_data.norm2x_before      = pointFrom->norm2_x;
  }

  computeCauchyUpdate(pointFrom, ctx);

  if(pointFrom->updateCauchy_lensq >= trustregion*trustregion)
  {
    SAY_IF_VERBOSE( "taking cauchy step");
    if(ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG)
    {
      vnlog_debug_data.step_type = STEPTYPE_CAUCHY;
      vnlog_debug_data.step_len  = vnlog_debug_data.step_len_cauchy;
    }
    *step_len_sq = pointFrom->updateCauchy_lensq;

    // cauchy step goes beyond my trust region, so I do a gradient descent
    // to the edge of my trust region and call it good
    vec_copy_scaled(step,
                    pointFrom->updateCauchy,
                    trustregion / sqrt(pointFrom->updateCauchy_lensq),
                    ctx->Nstate);
    pointFrom->didStepToEdgeOfTrustRegion = 1;
  }
  else
  {
    // I'm not yet done. The cauchy point is within the trust region, so I can
    // go further. I look at the full Gauss-Newton step. If this is within the
    // trust region, I use it. Otherwise, I find the point at the edge of my
    // trust region that lies on a straight line between the Cauchy point and
    // the Gauss-Newton solution, and use that. This is the heart of Powell's
    // dog-leg algorithm.
    computeGaussNewtonUpdate(pointFrom, ctx);
    if(pointFrom->updateGN_lensq <= trustregion*trustregion)
    {
      SAY_IF_VERBOSE( "taking GN step");
      if(ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG)
      {
        vnlog_debug_data.step_type = STEPTYPE_GAUSSNEWTON;
        vnlog_debug_data.step_len  = vnlog_debug_data.step_len_gauss_newton;
      }
      *step_len_sq = pointFrom->updateGN_lensq;

      // full Gauss-Newton step lies within my trust region. Take the full step
      memcpy( step,
              ctx->is_sparse ? pointFrom->updateGN_cholmoddense->x : pointFrom->updateGN_dense,
              ctx->Nstate * sizeof(step[0]) );
      pointFrom->didStepToEdgeOfTrustRegion = 0;
    }
    else
    {
      SAY_IF_VERBOSE( "taking interpolated step");

      // full Gauss-Newton step lies outside my trust region, so I interpolate
      // between the Cauchy-point step and the Gauss-Newton step to find a step
      // that takes me to the edge of my trust region.
      computeInterpolatedUpdate(step,
                                step_len_sq,
                                pointFrom, trustregion, ctx);
      pointFrom->didStepToEdgeOfTrustRegion = 1;
      if(ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG)
      {
        vnlog_debug_data.step_type = STEPTYPE_INTERPOLATED;
        vnlog_debug_data.step_len  = vnlog_debug_data.step_len_interpolated;
      }
    }
  }

  // take the step
  vec_add(p_new, pointFrom->p, step, ctx->Nstate);
  double expectedImprovement = computeExpectedImprovement(step, pointFrom, ctx);
  if(ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG)
  {
    vnlog_debug_data.expected_improvement = expectedImprovement;

    if(pointFrom->step_to_here_len_sq != INFINITY)
    {
      double cos_direction_change =
        inner(step, pointFrom->step_to_here, ctx->Nstate) /
        sqrt(*step_len_sq * pointFrom->step_to_here_len_sq);

      // check the numerical overflow cases
      if(cos_direction_change >= 1.0)
        vnlog_debug_data.step_direction_change_deg = 0.0;
      else if(cos_direction_change <= -1.0)
        vnlog_debug_data.step_direction_change_deg = 180.0;
      else
        vnlog_debug_data.step_direction_change_deg = 180.0/M_PI*acos(cos_direction_change);
    }
  }

  // are we done? For each state variable I look at the update step. If all the elements fall below
  // a threshold, I call myself done
  for(int i=0; i<ctx->Nstate; i++)
    if( fabs(step[i]) > ctx->parameters->update_threshold )
      return expectedImprovement;

  SAY_IF_VERBOSE( "update small enough. Done iterating!");

  return -1.0;
}


// I have a candidate step. I adjust the trustregion accordingly, and also
// report whether this step should be accepted (0 == rejected, otherwise
// accepted)
static int evaluateStep_adjustTrustRegion(const dogleg_operatingPoint_t* before,
                                          const dogleg_operatingPoint_t* after,
                                          double* trustregion,
                                          double expectedImprovement,
                                          dogleg_solverContext_t* ctx)
{
  double observedImprovement = before->norm2_x - after->norm2_x;
  double rho = observedImprovement / expectedImprovement;
  SAY_IF_VERBOSE( "observed/expected improvement: %.6g/%.6g. rho = %.6g",
                  observedImprovement, expectedImprovement, rho);
  if(ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG)
  {
    vnlog_debug_data.observed_improvement = observedImprovement;
    vnlog_debug_data.rho                  = rho;
  }


  // adjust the trust region
  if( rho < ctx->parameters->trustregion_decrease_threshold )
  {
    SAY_IF_VERBOSE( "rho too small. decreasing trust region");

    // Our model doesn't fit well. We should reduce the trust region size. If
    // the trust region size was affecting the attempted step, do this by a
    // constant factor. Otherwise, drop the trustregion to attempted step size
    // first
    if( !before->didStepToEdgeOfTrustRegion )
      *trustregion = sqrt(before->updateGN_lensq);

    *trustregion *= ctx->parameters->trustregion_decrease_factor;
  }
  else if (rho > ctx->parameters->trustregion_increase_threshold && before->didStepToEdgeOfTrustRegion)
  {
    SAY_IF_VERBOSE( "rho large enough. increasing trust region");

    *trustregion *= ctx->parameters->trustregion_increase_factor;
  }
  if(ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG)
    vnlog_debug_data.trustregion_after = *trustregion;

  return rho > 0.0;
}

static int runOptimizer(dogleg_solverContext_t* ctx)
{
  double trustregion = ctx->parameters->trustregion0;
  int stepCount = 0;

  if( computeCallbackOperatingPoint(ctx->beforeStep, ctx) )
    return stepCount;

  SAY_IF_VERBOSE( "Initial operating point has norm2_x %.6g", ctx->beforeStep->norm2_x);


  ctx->beforeStep->step_to_here_len_sq = INFINITY;

  while( stepCount<ctx->parameters->max_iterations )
  {
    SAY_IF_VERBOSE( "================= step %d", stepCount );

    while(1)
    {
      SAY_IF_VERBOSE("--------");

      double expectedImprovement =
        takeStepFrom(ctx->beforeStep,
                     ctx->afterStep->p,
                     ctx->afterStep->step_to_here,
                     &ctx->afterStep->step_to_here_len_sq,
                     trustregion, ctx);

      // negative expectedImprovement is used to indicate that we're done
      if(expectedImprovement < 0.0)
      {
        if( ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG )
          vnlog_debug_emit_record(stepCount, 1);
        return stepCount;
      }

      int afterStepZeroGradient = computeCallbackOperatingPoint(ctx->afterStep, ctx);
      SAY_IF_VERBOSE( "Evaluated operating point with norm2_x %.6g", ctx->afterStep->norm2_x);
      if(ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG)
        vnlog_debug_data.norm2x_after = ctx->afterStep->norm2_x;

      if( evaluateStep_adjustTrustRegion(ctx->beforeStep, ctx->afterStep, &trustregion,
                                         expectedImprovement, ctx) )
      {
        SAY_IF_VERBOSE( "accepted step");

        if( ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG )
          vnlog_debug_emit_record(stepCount, 1);
        stepCount++;

        // I accept this step, so the after-step operating point is the before-step operating point
        // of the next iteration. I exchange the before- and after-step structures so that all the
        // pointers are still around and I don't have to re-allocate
        dogleg_operatingPoint_t* tmp;
        tmp             = ctx->afterStep;
        ctx->afterStep  = ctx->beforeStep;
        ctx->beforeStep = tmp;

        if( afterStepZeroGradient )
        {
          SAY_IF_VERBOSE( "Gradient low enough and we just improved. Done iterating!" );

          return stepCount;
        }

        break;
      }

      SAY_IF_VERBOSE( "rejected step");
      if( ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG )
        vnlog_debug_emit_record(stepCount, 0);

      // This step was rejected. check if the new trust region size is small
      // enough to give up
      if(trustregion < ctx->parameters->trustregion_threshold)
      {
        SAY_IF_VERBOSE( "trust region small enough. Giving up. Done iterating!");
        return stepCount;
      }

      // I have rejected this step, so I try again with the new trust region
    }
  }

  if(stepCount == ctx->parameters->max_iterations)
      SAY_IF_VERBOSE( "Exceeded max number of iterations");

  return stepCount;
}

static
dogleg_operatingPoint_t* allocOperatingPoint(int Nstate, int numMeasurements,
                                             unsigned int NJnnz,
                                             cholmod_common* common)
{
  int is_sparse = NJnnz > 0;

  dogleg_operatingPoint_t* point = static_cast<dogleg_operatingPoint_t*>(malloc(sizeof(dogleg_operatingPoint_t)));
  ASSERT(point != NULL);


  int Npool =
    Nstate          +
    numMeasurements +
    Nstate          +
    Nstate          +
    Nstate;
  if(!is_sparse)
    Npool += numMeasurements*Nstate + Nstate;

  double* pool = static_cast<double*>(malloc( Npool * sizeof(double) ));
  ASSERT( pool != NULL );

  point->p            = &pool[0];
  point->x            = &pool[Nstate];
  point->Jt_x         = &pool[Nstate +
                              numMeasurements];
  point->updateCauchy = &pool[Nstate +
                              numMeasurements +
                              Nstate];
  point->step_to_here = &pool[Nstate +
                              numMeasurements +
                              Nstate +
                              Nstate];

  if( is_sparse )
  {
    point->Jt = cholmod_allocate_sparse(Nstate, numMeasurements, NJnnz,
                                        1, // sorted
                                        1, // packed,
                                        0, // NOT symmetric
                                        CHOLMOD_REAL,
                                        common);
    ASSERT(point->Jt != NULL);
    point->updateGN_cholmoddense = NULL; // This will be allocated as it is used
  }
  else
  {
    point->J_dense = &pool[Nstate +
                           numMeasurements +
                           Nstate +
                           Nstate +
                           Nstate];
    point->updateGN_dense = &pool[Nstate +
                                  numMeasurements +
                                  Nstate +
                                  Nstate +
                                  Nstate +
                                  numMeasurements * Nstate];
  }

  // vectors don't have any valid data yet
  point->updateCauchy_valid = 0;
  point->updateGN_valid     = 0;

  return point;
}

static void freeOperatingPoint(dogleg_operatingPoint_t** point, cholmod_common* common)
{
  // MUST match allocOperatingPoint(). It does a single malloc() and stores the
  // pointer into p
  free((*point)->p);

  int is_sparse = common != NULL;

  if( is_sparse )
  {
    cholmod_free_sparse(&(*point)->Jt,   common);

    if((*point)->updateGN_cholmoddense != NULL)
      cholmod_free_dense(&(*point)->updateGN_cholmoddense, common);
  }

  free(*point);
  *point = NULL;
}

static int cholmod_error_callback(const char* s, ...)
{
  SAY_NONEWLINE("");

  va_list ap;
  va_start(ap, s);
  int ret = vfprintf(stderr, s, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  return ret;
}

static void set_cholmod_options(cholmod_common* cc)
{
  // I want to use LGPL parts of CHOLMOD only, so I turn off the supernodal routines. This gave me a
  // 25% performance hit in the solver for a particular set of optical calibration data.
  cc->supernodal = 0;


  // I want all output to go to STDERR, not STDOUT
#if (CHOLMOD_VERSION <= (CHOLMOD_VER_CODE(2,2)))
  cc->print_function = cholmod_error_callback;
#elif (CHOLMOD_VERSION < (CHOLMOD_VER_CODE(4,0)))
  CHOLMOD_FUNCTION_DEFAULTS ;
  CHOLMOD_FUNCTION_PRINTF(cc) = cholmod_error_callback;
#else
  SuiteSparse_config_printf_func_set(cholmod_error_callback);
#endif
}

void dogleg_freeContext(dogleg_solverContext_t** ctx)
{
  freeOperatingPoint(&(*ctx)->beforeStep, (*ctx)->is_sparse ? &(*ctx)->common : NULL);
  freeOperatingPoint(&(*ctx)->afterStep,  (*ctx)->is_sparse ? &(*ctx)->common : NULL);

  if( (*ctx)->is_sparse )
  {
    if((*ctx)->factorization != NULL)
      cholmod_free_factor (&(*ctx)->factorization, &(*ctx)->common);
    cholmod_finish(&(*ctx)->common);
  }
  else
    free((*ctx)->factorization_dense);

  free(*ctx);
  *ctx = NULL;
}

static double _dogleg_optimize(double* p, unsigned int Nstate,
                               unsigned int Nmeas, unsigned int NJnnz,
                               dogleg_callback_t* f,
                               void* cookie,

                               // NULL to use the globals
                               const dogleg_parameters2_t* parameters,
                               dogleg_solverContext_t** returnContext)
{
  int is_sparse = NJnnz > 0;


  dogleg_solverContext_t* ctx = static_cast<dogleg_solverContext_t*>(malloc(sizeof(dogleg_solverContext_t)));
  ctx->f                      = f;
  ctx->cookie                 = cookie;
  ctx->factorization          = NULL;
  ctx->lambda                 = 0.0;
  ctx->Nstate                 = Nstate;
  ctx->Nmeasurements          = Nmeas;
  ctx->parameters             = parameters ? parameters : &parameters_global;

  if( ctx->parameters->dogleg_debug & DOGLEG_DEBUG_VNLOG )
    vnlog_debug_emit_legend();

  if( returnContext != NULL )
    *returnContext = ctx;

  if( is_sparse )
  {
    if( !cholmod_start(&ctx->common) )
    {
      SAY( "Couldn't initialize CHOLMOD");
      return -1.0;
    }
    set_cholmod_options(&ctx->common);
    ctx->is_sparse = 1;
  }
  else
    ctx->is_sparse = 0;

  ctx->beforeStep = allocOperatingPoint(Nstate, Nmeas, NJnnz, &ctx->common);
  ctx->afterStep  = allocOperatingPoint(Nstate, Nmeas, NJnnz, &ctx->common);

  memcpy(ctx->beforeStep->p, p, Nstate * sizeof(double));

  // everything is set up, so run the solver!
  int    numsteps = runOptimizer(ctx);
  double norm2_x  = ctx->beforeStep->norm2_x;

  // runOptimizer places the most recent results into beforeStep in preparation for another
  // iteration
  memcpy(p, ctx->beforeStep->p, Nstate * sizeof(double));

  SAY_IF_VERBOSE( "success! took %d iterations", numsteps);

  if( returnContext == NULL )
    dogleg_freeContext(&ctx);

  return norm2_x;
}

double dogleg_optimize2(double* p, unsigned int Nstate,
                        unsigned int Nmeas, unsigned int NJnnz,
                        dogleg_callback_t* f,
                        void* cookie,
                        const dogleg_parameters2_t* parameters,
                        dogleg_solverContext_t** returnContext)
{
  if( NJnnz == 0 )
  {
    SAY( "I must have NJnnz > 0, instead I have %d", NJnnz);
    return -1.0;
  }

  return _dogleg_optimize(p, Nstate, Nmeas, NJnnz, f,
                          cookie,
                          parameters,
                          returnContext);
}

double dogleg_optimize(double* p, unsigned int Nstate,
                       unsigned int Nmeas, unsigned int NJnnz,
                       dogleg_callback_t* f,
                       void* cookie,
                       dogleg_solverContext_t** returnContext)
{
  return dogleg_optimize2(p,Nstate,Nmeas,NJnnz,f,cookie,
                          NULL, // no parameters; use the globals
                          returnContext);
}


double dogleg_optimize_dense2(double* p, unsigned int Nstate,
                              unsigned int Nmeas,
                              dogleg_callback_dense_t* f, void* cookie,
                              const dogleg_parameters2_t* parameters,
                              dogleg_solverContext_t** returnContext)
{
  return _dogleg_optimize(p, Nstate, Nmeas, 0, (dogleg_callback_t*)f,
                          cookie,
                          parameters,
                          returnContext);
}
double dogleg_optimize_dense(double* p, unsigned int Nstate,
                             unsigned int Nmeas,
                             dogleg_callback_dense_t* f, void* cookie,
                             dogleg_solverContext_t** returnContext)
{
  return dogleg_optimize_dense2(p,Nstate,Nmeas,f,cookie,
                                NULL, // no parameters; use the globals
                                returnContext);
}







// Computes pinv(J) for a subset of measurements: inv(JtJ) *
// Jt[imeasurement0..imeasurement0+N-1]. Returns false if something failed.
// ASSUMES THAT THE CHOLESKY FACTORIZATION HAS ALREADY BEEN COMPUTED.
//
// This function is experimental, and subject to change
static bool pseudoinverse_J_dense(// output
                                  double* out,

                                  // inputs
                                  const dogleg_operatingPoint_t* point,
                                  const dogleg_solverContext_t* ctx,
                                  int i_meas0, int NmeasInChunk)
{
  int info;
  memcpy(out,
         &point->J_dense[i_meas0*ctx->Nstate],
         NmeasInChunk*ctx->Nstate*sizeof(double));
  char uplo = 'L';
  int Nstate_copy = ctx->Nstate;
  int Nstate_copy2 = ctx->Nstate;
  dpptrs_(&uplo, &Nstate_copy, &NmeasInChunk,
          ctx->factorization_dense,
          out,
          &Nstate_copy2, &info, 1);
  return info==0;
}

// Computes pinv(J) for a subset of measurements: inv(JtJ) *
// Jt[imeasurement0..imeasurement0+N-1]. Returns false if something failed.
// ASSUMES THAT THE CHOLESKY FACTORIZATION HAS ALREADY BEEN COMPUTED.
//
// allocates memory, returns NULL on failure. ON SUCCESS, THE CALLER IS
// RESPONSIBLE FOR FREEING THE RETURNED MEMORY
//
// This function is experimental, and subject to change
static cholmod_dense* pseudoinverse_J_sparse(// inputs
                                             const dogleg_operatingPoint_t* point,
                                             dogleg_solverContext_t* ctx,
                                             int i_meas0, int NmeasInChunk,

                                             // Pre-allocated array for the
                                             // right-hand-side. This will be used as a
                                             // workspace. Create this like so:
                                             //
                                             //   cholmod_allocate_dense( Nstate,
                                             //                           NmeasInChunk,
                                             //                           Nstate,
                                             //                           CHOLMOD_REAL,
                                             //                           &ctx->common );

                                             cholmod_dense* Jt_chunk)
{
  // I'm solving JtJ x = b where J is sparse, b is sparse, but x ends up dense.
  // cholmod doesn't have functions for this exact case. so I use the
  // dense-sparse-dense function (cholmod_solve), and densify the input. Instead
  // of sparse-sparse-sparse and the densifying the output (cholmod_spsolve).
  // This feels like it'd be more efficient

  memset( Jt_chunk->x, 0, Jt_chunk->nrow*Jt_chunk->ncol*sizeof(double) );
  int Jt_chunk_ncol_backup = Jt_chunk->ncol;
  for(int i_meas=0; i_meas<NmeasInChunk; i_meas++)
  {
    if( i_meas0 + i_meas >= (int)ctx->Nmeasurements )
    {
      // at the end, we could have one chunk with less that chunk_size
      // columns
      Jt_chunk->ncol = i_meas;
      break;
    }

    for(unsigned int i0=P(point->Jt, i_meas0+i_meas); i0<P(point->Jt, i_meas0+i_meas+1); i0++)
    {
      int irow = I(point->Jt,i0);
      double x = X(point->Jt,i0);
      ((double*)Jt_chunk->x)[irow + i_meas*Jt_chunk->nrow] = x;
    }
  }

  // solve inv(JtJ)Jt[slice]
  cholmod_dense* pinv =
    cholmod_solve(CHOLMOD_A,
                  ctx->factorization,
                  Jt_chunk,
                  &ctx->common);
  Jt_chunk->ncol = Jt_chunk_ncol_backup;
  return pinv;
}


/*
The below is a bunch of code for outlier detection/rejection and for confidence
evaluation. IT'S ALL EXPERIMENTAL AND SUBJECT TO CHANGE

What is an outlier? Suppose I just found an optimum. I define an outlier as an
observation that does two things to the problem if I remove that observation:

1. The cost function would improve significantly. Things would clearly improve
   because the cost function contribution of the removed point itself would get
   removed, but ALSO because the parameters could fit the remaining data better
   without that extra observation; the "outlier" wouldn't pull the solution away
   from the "true" optimum.

2. The confidence of the solution does not significantly decrease. One could
   imagine a set of data that define the problem poorly, and produce a low cost
   function value for some (overfit) set of parameters. And one can imagine an
   extra point being added that defines the problem and increases the confidence
   of the solution. This extra point would suppress the overfitting, so this
   extra point would increase the cost function value. Condition 1 above would
   make this extra point look like an outlier, and this condition is meant to
   detect this case and to classify this point as NOT an outlier

Let's say we just computed an optimum least-squares fit, and we try to determine
if some of the data points look like outliers. The least squares problem I just
solved has cost function

  E = norm2(x)

where x is a length-N vector of measurements. We solved it by optimizing the
vector of parameters p. We're at an optimum, so dE/dp = 0 -> Jt x = 0

We define an outlier as a measurement that would greatly improve the cost
function E if this measurement was removed, and the problem was re-optimized.

Let's say the problem is locally linear (J = dx/dp is constant), and let's say
re-optimizing moves the parameter by dp. The original cost function was

  E0 = norm2(x)

If we move by dp, and take away the outlier's own cost, we get

  E1 = norm2(x + J dp) - norm2( x* + J* dp )

where x* and J* refer to the outlier measurements. Thus "Dima's self+others
factor" is norm2(x) - (norm2(x + J dp) - norm2( x* + J* dp ))

We can choose to look at ONLY the effect on the other variables. That would
produce "Dima's others factor" = norm2(x)-norm2(x*) - (norm2(x + J dp) -
norm2(x* + J* dp ))

This is very similar to Cook's D factor (which Dima apparently reinvented 40
years after the fact!). This factor looks not at the difference of norm2(x), but
at norm2(difference) instead. So "Cook's self+others factor" is proportional to
norm2(x - (x+J dp)). This is the "normal" Cook's D factor. We can also compute
"Cook's others factor": norm2(x - vec(x*) - (x + J dp -(vec(x*) + full(J*) dp)))
= norm2( - J dp + full(J*) dp) = norm2(J dp) + norm2(full(J*) dp) -2 dpt(Jt
full(J*) + full(J*)tJ)dp = norm2(J dp) + norm2(full(J*) dp) -2 dpt(Jt full(J*) +
full(J*)tJ)dp = norm2(J dp) + norm2(J* dp) - 2 norm2(J* dp) = norm2(J dp) -
norm2(J* dp)

This is 4 flavors of a very similar computation. In summary (and ignoring scale
factors):

  Dima's self+others: -norm2(J dp) + 2 x*t J* dp + norm2(J* dp) + norm2(x*)
  Dima's others     : -norm2(J dp) + 2 x*t J* dp + norm2(J* dp)
  Cook's self+others: norm2(J dp)
  Cook's others     : norm2(J dp) - norm2(J* dp)

Let's compute these. dE1/dp = 0 at p+dp ->

  0 = Jt x + JtJ dp - J*t x* - J*tJ* dp
    =        JtJ dp - J*t x* - J*tJ* dp

-> dp = inv(JtJ - J*tJ*) J*t x*

Woodbury identity:

  inv(JtJ - J*t J*) =
  = inv(JtJ) - inv(JtJ) J*t inv(-I + J* inv(JtJ) J*t) J* inv(JtJ)

Let
  A = J* inv(JtJ) J*t
  B = inv(A - I)

So
  AB = BA = I+B

Thus
  inv(JtJ - J*t J*) =
  = inv(JtJ) - inv(JtJ) J*t B J* inv(JtJ)

and

  dp = inv(JtJ - J*tJ*) J*t x* =
     = inv(JtJ)J*t x* - inv(JtJ) J*t B J* inv(JtJ) J*t x*
     = inv(JtJ)J*t x* - inv(JtJ) J*t B A x*
     = inv(JtJ)J*t(I - B A) x*
     = -inv(JtJ)J*t B x*

Then

  norm2(J dp) = x*t ( B J* inv() Jt J inv() J*t B ) x*
              = x*t ( B J* inv() J*t B ) x*
              = x*t ( B A B ) x*
              = x*t ( B + B*B ) x*

  2 x*t J* dp = -2 x*t J* inv(JtJ)J*t B x* =
              = -2 x*t A B x* =
              = x*t (-2AB) x* =
              = x*t (-2I - 2B) x*

  norm2(J* dp) = x*t ( B J* inv() J*tJ* inv() J*t B ) x* =
               = x*t ( B A A B ) x* =
               = x*t ( I + 2B + B*B ) x*

  norm2(x*)    = x*t ( I ) x*

There're two ways to compute the "Dima's self-only" factor. I can simply say
that the measurements' cost of norm2(x*) has been removed, so the factor is x*t
I x* or I can

1. Remove measurements
2. Re-optimize
3. Look to see how the residual of x* changes

This is different because I look at what happens to x*(p) when x* is no longer
in the optimized set.

State moves by dp. x* moves by J* dp. I look at

  dE = norm2(x* + J* dp) - norm2(x*)
     = 2 x*t J* dp + norm2(J* dp) =
     = x*t (-2I - 2B + I + 2B + B*B ) x* =
     = x*t (B*B - I) x*

I expect that removing measurements from the optimization should make their
residuals worse. I.e. I expect dE > 0. Let's check. Is B*B - I positive
definite? Let's say that there's v,l such that

  (B*B-I)v = l v, norm2(v) = 1
  -->
  BBv      = (l+1)v
  vBBv     = l+1

  Let u = Bv ->
  norm2(u) = l+1

  A = J* inv(JtJ) J*t
  B = inv(A - I) ->

  v = (A-I)u
  norm2(v) = 1 = norm2(Au) - 2ut A u + norm2(u) ->
  -> l = 2ut A u - norm2(Au)

  Let w = J*t u
  -> Au = J* inv(JtJ) w
  -> ut A u = wt inv(JtJ) w ->
  l = wt ( 2inv(JtJ) - inv(JtJ)J*tJ* inv(JtJ) ) w

  J*t J* = sum(outer(j*,j*)) = sum(outer(j,j)) - sum(outer(jnot*,jnot*)) =
         = JtJ - Jnot*t Jnot*

  -> l = wt ( 2inv(JtJ) - inv(JtJ)JtJinv(JtJ) + inv(JtJ)Jnot*tJnot* inv(JtJ) ) w
       = wt (  inv(JtJ) + inv(JtJ)Jnot*tJnot* inv(JtJ) ) w

  -> substitute -> l = wt ( C + CDC ) w

  where C >= 0 and D >= 0

  wt C wt >= 0

  wt CDC wt = (Ctw)t D Ctw >= 0 -> l >= 0

So B*B-I >= 0 and dE >= 0: removing a point will never make its own fit better

- if dE >> 0: the other data does NOT support this measurement being correct
- if dE ~~ 0: the other data supports this measurement being correct


Putting all this together I get the expressions for the factors above:

  Dima's self+others:        x*t (-B      ) x*
  Dima's others     :        x*t (-B - I  ) x*
  Dima's self (simple):      x*t ( I      ) x*
  Dima's self (interesting): x*t (B*B - I ) x*
  Cook's self+others:        x*t ( B + B*B) x*
  Cook's others     :        x*t (-B - I  ) x*



One can also do a similar analysis to gauge our confidence in a solution. We can
do this by

1. Solving the optimization problem

2. Querying the solution in a way we care about to produce a new feature
   group of measurements x = f - ref. We can compute J = dx/dp = df/dp. And
   we presumably know something about ref: like its probability
   distribution for instance. Example: we just calibrated a camera; we want
   to know how confident we are about a projection in a particular spot on
   the imager. I compute a projection in that spot: q = project(v). If
   added to the optimization I'd get x = q - ref where 'ref' would be the
   observed pixel coordinate.

3. If this new feature was added to the optimization, I can compute its
   outlierness factor in the same way as before. If we are confident in the
   solution in a particular spot, then we have consensus, and it wouldn't take
   much for these queries to look like outliers: the expected value of the
   outlierness would be high. Conversely, if we aren't well-defined then a wider
   set of points would fit the solution, and things wouldn't look very outliery

Very similar analysis to the above:

Let p,x,J represent the solution. The new feature we're adding is x* with
jacobian J*. The solution would move by dp to get to the new optimum.

Original solution is an optimum: Jt x = 0

If we add x* and move by dp, we get

  E1 = norm2(x + J dp) + norm2( x* + J* dp )

Thus "Dima's self+others factor" is (norm2(x + J dp) + norm2( x* + J* dp )) -
norm2(x) = norm2(Jdp) + norm2( x* + J* dp )

We can choose to look at ONLY the effect on the other variables. That would
produce "Dima's others factor" norm2(x + J dp) - norm2(x) = norm2(Jdp)

This is very similar to Cook's D factor (which Dima apparently reinvented 40
years after the fact!). This factor looks not at the difference of norm2(x), but
at norm2(difference) instead. So "Cook's others factor" is proportional to
norm2(x - (x+J dp)). This is the "normal" Cook's D factor. We can also compute
"Cook's self+others factor": norm2(concat(x + J dp, x* + J* dp) - concat(x,x*))
= norm2(x + J dp-x) + norm2(x* + J* dp - x*) = norm2(J dp) + norm2(J* dp)

This is 4 flavors of a very similar computation. In summary (and ignoring scale
factors):

  Dima's self+others: norm2(J dp) + 2 x*t J* dp + norm2(J* dp) + norm2(x*)
  Dima's others     : norm2(J dp)
  Cook's self+others: norm2(J dp) + norm2(J* dp)
  Cook's others     : norm2(J dp)

The problem including the new point is also at an optimum:

  E1 = norm2(x + J dp) + norm2( x* + J* dp )
  dE1/dp = 0 -> 0 = Jt x + JtJ dp + J*t x* + J*tJ*dp =
                  =        JtJ dp + J*t x* + J*tJ*dp
-> dp = -inv(JtJ + J*tJ*) J*t x*

Woodbury identity:

  -inv(JtJ + J*t J*) =
  = -inv(JtJ) + inv(JtJ) J*t inv(I + J* inv(JtJ) J*t) J* inv(JtJ)

Let
  A = J* inv(JtJ) J*t   (same as before)
  B = inv(A + I)        (NOT the same as before)

So
  AB = BA = I-B

Thus
  -inv(JtJ + J*t J*) =
  = -inv(JtJ) + inv(JtJ) J*t B J* inv(JtJ)

and

  dp = -inv(JtJ + J*tJ*) J*t x* =
     = -inv(JtJ)J*t x* + inv(JtJ) J*t B J* inv(JtJ) J*t x* =
     = -inv(JtJ)J*t x* + inv(JtJ) J*t B A x*
     = -inv(JtJ)J*t(I - B A) x*
     = -inv(JtJ)J*t B x*   (same as before, but with a different B!)

Then

  norm2(J dp) = x*t ( B J* inv() Jt J inv() J*t B ) x*
              = x*t ( B J* inv() J*t B ) x*
              = x*t ( B A B ) x*
              = x*t ( B - B*B ) x*

  2 x*t J* dp = -2 x*t J* inv(JtJ)J*t B x* =
              = -2 x*t A B x* =
              = x*t (-2AB) x* =
              = x*t (-2I + 2B) x*

  norm2(J* dp) = x*t ( B J* inv() J*tJ* inv() J*t B ) x* =
               = x*t ( B A A B ) x* =
               = x*t ( I - 2B + B*B ) x*

  norm2(x*)    = x*t ( I ) x*

How do I compute "Dima's self" factor? The "simple" flavor from above looks at
the new measurement only: norm2(x*). The "interesting" flavor, look at what
happens to the measurements' error when they're added to the optimization set.
State moves by dp. x* moves by J* dp. I look at

  dE = norm2(x* + J*dp) - norm2(x*) =
       2 x*t J* dp + norm2(J* dp) =
       x*t (-2I + 2B + I - 2B + B*B) x* =
       x*t (B*B - I) x*

I expect that adding a point to the optimization would make it fit better: dE <
0. Let's check. Let's say that there's v,l such that

  (B*B-I)v = l v, norm2(v) = 1
  -->
  BBv      = (l+1)v
  vBBv     = l+1

  Let u = Bv ->
  norm2(u) = l+1

  A = J* inv(JtJ) J*t
  B = inv(A + I) ->

  v = (A+I)u
  norm2(v) = 1 = norm2(Au) + 2ut A u + norm2(u) ->
  -> l = -2ut A u - norm2(Au)

  Let w = J*t u
  -> Au = J* inv(JtJ) w
  -> ut A u = wt inv(JtJ) w ->
  l = -2 wt inv(JtJ) w - norm2(Au)

  Since inv(JtJ) > 0 -> l < 0. As expected

So B*B-I is negative definite: adding measurements to the optimization set makes
them fit better

Putting all this together I get the expressions for the factors above:

  Dima's self+others:        x*t (B      ) x*
  Dima's others     :        x*t (B - B*B) x*
  Dima's self (simple):      x*t (I      ) x*
  Dima's self (interesting): x*t (B*B - I) x*
  Cook's self+others:        x*t (I - B  ) x*
  Cook's others     :        x*t (B - B*B) x*

These expressions are all tested and verified in
mrcal/analyses/outlierness-test.py


There're several slightly-different definitions of Cook's D and of a
rule-of-thumb threshold floating around on the internet. Wikipedia says:

  D = norm2(x_io - x_i)^2 / (Nstate * norm2(x_io)/(Nmeasurements - Nstate))
  D_threshold = 1

An article https://www.nature.com/articles/nmeth.3812 says

  D = norm2(x_io - x_i)^2 / ((Nstate+1) * norm2(x_io)/(Nmeasurements - Nstate -1))
  D_threshold = 4/Nmeasurements

I haven't tracked down the reference for this second definition, but it looks
more reasonable, and I use it here.

Here I use the second definition. That definition expands to

  k = xo^2 / ((Nstate+1) * norm2(x)/(Nmeasurements - Nstate -1))
  B = 1.0/(jt inv(JtJ) j - 1)
  f = k * (B + B*B)

I report normalized outlierness factors so that the threshold is 1. Thus I use

  k = Nmeasurements / (4*((Nstate+1) * norm2(x)/(Nmeasurements - Nstate - 1)))

*/


static void accum_outlierness_factor(// output
                                     double* factor,

                                     // inputs
                                     const double* x,

                                     // A is symmetric. I store the upper triangle row-first
                                     const double* A,

                                     // if outliers are grouped into features, the
                                     // feature size is set here
                                     int featureSize,
                                     double k)
{
  // This implements Dima's self+outliers factor.
  //
  // from the derivation in a big comment above I have
  //
  //   f = k (x*t (-B) x* )
  //
  // where B = inv(A - I) and
  //       A = J* inv(JtJ) J*t

  // I only implemented featureSize == 1 and 2 so far.
  if(featureSize <= 1)
  {
    featureSize = 1;

    double denom = 1.0 - *A;
    if( fabs(denom) < 1e-8 )
    {
      *factor = DBL_MAX; // definitely an outlier
      return;
    }
    else
      *factor = x[0]*x[0] / denom;
  }
  else if(featureSize == 2)
  {
    double det = (1.0-A[0])*(1.0-A[2]) - A[1]*A[1];
    if( fabs(det) < 1e-8 )
    {
      *factor = DBL_MAX; // definitely an outlier
      return;
    }
    else
    {
      double B00_det =  A[2] - 1.0;
      double B11_det =  A[0] - 1.0;
      double B01_det = -A[1];

      // inner(x, Bx)
      double xBx =
        (x[0]*x[0]*B00_det       +
         x[0]*x[1]*B01_det * 2.0 +
         x[1]*x[1]*B11_det) / det;

      // norm2(Bx)
       double v1 = x[0]*B00_det + x[1]*B01_det;
       double v2 = x[0]*B01_det + x[1]*B11_det;
       double xBBx = (v1*v1 + v2*v2) / (det*det);

      // // mine self+others
      // *factor = -xBx;

      // // mine others / cook others
      // *factor = -(x[0]*x[0] + x[1]*x[1]) - xBx;

      // cook's self+others
      *factor = xBx + xBBx;

    }
  }
  else
  {
    SAY("featureSize > 2 not implemented yet. Got featureSize=%d", featureSize);
    ASSERT(0);
  }


// #warning This is a hack. The threshold should be 1.0, and the scaling should make sure that is the case. I am leaving it for now
  k /= 8.;


  *factor *= k;
}

static void getOutliernessScale(// out
                                double* scale, // if *scale > 0, I just keep what I have

                                // in
                                int Nmeasurements, int Nstate,
                                int NoutlierFeatures, int featureSize,
                                double norm2_x)
{
  // if *scale > 0, I just keep what I have
  if(*scale > 0.0)
    return;

  int    Nmeasurements_outliers    = NoutlierFeatures*featureSize;
  int    Nmeasurements_nonoutliers = Nmeasurements - Nmeasurements_outliers;

  *scale =
    (double)(Nmeasurements_nonoutliers) /
    (4.*((double)(Nstate+1) * norm2_x/(double)(Nmeasurements_nonoutliers - Nstate - 1)));
}

static bool getOutliernessFactors_dense( // output
                                        double* factors, // Nfeatures factors

                                        // output, input
                                        double* scale, // if <0 then I recompute

                                        // inputs
                                        // if outliers are grouped into features,
                                        // the feature size is set here
                                        int featureSize,
                                        int Nfeatures,
                                        int NoutlierFeatures,
                                        const dogleg_operatingPoint_t* point,
                                        dogleg_solverContext_t* ctx )
{
  // cholmod_spsolve() and cholmod_solve()) work in chunks of 4, so I do this in
  // chunks of 4 too. I pass it rows of J, 4 at a time. Note that if I have
  // measurement features, I don't want these to cross chunk boundaries, so I set
  // up chunk_size=lcm(N,4)
  int chunk_size = 4;
  if(featureSize <= 1)
    featureSize = 1;
  if(featureSize > 1)
  {
    // haven't implemented anything else yet. Don't have lcm() readily available
    ASSERT(featureSize == 2);
    // chunk_size = lcm(chunk_size,featureSize)
  }

  int  Nstate        = ctx->Nstate;
  int  Nmeasurements = ctx->Nmeasurements;
  bool result        = false;

  double* invJtJ_Jt = static_cast<double*>(malloc(Nstate*chunk_size*sizeof(double)));
  if(invJtJ_Jt == NULL)
  {
    SAY("Couldn't allocate invJtJ_Jt!");
    free(invJtJ_Jt);
    return result;
  }

  getOutliernessScale(scale,
                      Nmeasurements, Nstate, NoutlierFeatures, featureSize, point->norm2_x);

  int i_measurement_valid_chunk_start = -1;
  int i_measurement_valid_chunk_last  = -1;
  int i_measurement = 0;
  for( int i_feature=0; i_feature<Nfeatures; i_feature++, i_measurement+=featureSize)
  {
    if( i_measurement > i_measurement_valid_chunk_last )
    {
      bool pinvresult = pseudoinverse_J_dense(invJtJ_Jt, point, ctx,
                                              i_measurement, chunk_size);
      if(!pinvresult)
      {
        SAY("Couldn't compute pinv!");
        free(invJtJ_Jt);
        return result;
      }
      i_measurement_valid_chunk_start = i_measurement;
      i_measurement_valid_chunk_last  = i_measurement+chunk_size-1;
    }

    // from the derivation in a big comment in above I have
    //
    //   f = scale (xot (...) xo )
    //
    // where A = Jo inv(JtJ) Jot
    //
    // A is symmetric. I store the upper triangle
    std::vector<double> A(featureSize*(featureSize+1)/2);
    int iA=0;
    for(int i=0; i<featureSize; i++)
      for(int j=i; j<featureSize; j++, iA++)
      {
        A[iA] = 0.0;

        for(int k=0; k<Nstate; k++)
          A[iA] +=
            invJtJ_Jt     [Nstate*(i_measurement+i -i_measurement_valid_chunk_start) + k] *
            point->J_dense[Nstate* i_measurement+j                                   + k];
      }
    accum_outlierness_factor(&factors[i_feature],
                             &point->x[i_measurement],
                             A.data(), featureSize, *scale);
  }

  result = true;
  free(invJtJ_Jt);
  return result;
}


static bool getOutliernessFactors_sparse( // output
                                         double* factors, // Nfeatures factors

                                         // output, input
                                         double* scale, // if <0 then I recompute

                                         // inputs
                                         // if outliers are grouped into features,
                                         // the feature size is set here
                                         int featureSize,
                                         int Nfeatures,
                                         int NoutlierFeatures,
                                         const dogleg_operatingPoint_t* point,
                                         dogleg_solverContext_t* ctx )
{
  // cholmod_spsolve() and cholmod_solve()) work in chunks of 4, so I do this in
  // chunks of 4 too. I pass it rows of J, 4 at a time. Note that if I have
  // measurement features, I don't want these to cross chunk boundaries, so I set
  // up chunk_size=lcm(N,4)
  int chunk_size = 4;
  if(featureSize <= 1)
    featureSize = 1;
  if(featureSize > 1)
  {
    // haven't implemented anything else yet. Don't have lcm() readily available
    ASSERT(featureSize == 2);
    // chunk_size = lcm(chunk_size,featureSize)
  }

  int  Nstate        = ctx->Nstate;
  int  Nmeasurements = ctx->Nmeasurements;
  bool result        = false;

  cholmod_dense* invJtJ_Jt = NULL;
  cholmod_dense* Jt_chunk =
    cholmod_allocate_dense( Nstate,
                            chunk_size,
                            Nstate,
                            CHOLMOD_REAL,
                            &ctx->common );
  if(!Jt_chunk)
  {
    SAY("Couldn't allocate Jt_chunk!");
    if(Jt_chunk)  cholmod_free_dense(&Jt_chunk,  &ctx->common);
    if(invJtJ_Jt) cholmod_free_dense(&invJtJ_Jt, &ctx->common);
    return result;
  }

  getOutliernessScale(scale,
                      Nmeasurements, Nstate, NoutlierFeatures, featureSize, point->norm2_x);

  int i_measurement_valid_chunk_start = -1;
  int i_measurement_valid_chunk_last  = -1;
  int i_measurement = 0;
  for( int i_feature=0; i_feature<Nfeatures; i_feature++, i_measurement+=featureSize)
  {
    if( i_measurement > i_measurement_valid_chunk_last )
    {
      if(invJtJ_Jt) cholmod_free_dense(&invJtJ_Jt, &ctx->common);
      invJtJ_Jt = pseudoinverse_J_sparse(point, ctx,
                                         i_measurement, chunk_size,
                                         Jt_chunk);
      if(invJtJ_Jt == NULL)
      {
        SAY("Couldn't compute pinv!");
        if(Jt_chunk)  cholmod_free_dense(&Jt_chunk,  &ctx->common);
        if(invJtJ_Jt) cholmod_free_dense(&invJtJ_Jt, &ctx->common);
        return result;
      }

      i_measurement_valid_chunk_start = i_measurement;
      i_measurement_valid_chunk_last  = i_measurement+chunk_size-1;
    }

    // from the derivation in a big comment in above I have
    //
    //   f = scale (xot (...) xo )
    //
    // where A = Jo inv(JtJ) Jot
    //
    // A is symmetric. I store the upper triangle
    std::vector<double> A(featureSize*(featureSize+1)/2);
    int iA=0;
    for(int i=0; i<featureSize; i++)
      for(int j=i; j<featureSize; j++, iA++)
      {
        A[iA] = 0.0;

        for(unsigned int l = P(point->Jt, i_measurement+j);
            l     < P(point->Jt, i_measurement+j+1);
            l++)
        {
          int k = I(point->Jt, l);
          A[iA] +=
            ((double*)invJtJ_Jt->x)[Nstate*(i_measurement+i-i_measurement_valid_chunk_start) + k] *
            X(point->Jt, l);
        }
      }
    accum_outlierness_factor(&factors[i_feature],
                             &point->x[i_measurement],
                             A.data(), featureSize, *scale);
  }

  result = true;
  if(Jt_chunk)  cholmod_free_dense(&Jt_chunk,  &ctx->common);
  if(invJtJ_Jt) cholmod_free_dense(&invJtJ_Jt, &ctx->common);
  return result;
}

bool dogleg_getOutliernessFactors( // output
                                  double* factors, // Nfeatures factors

                                  // output, input
                                  double* scale, // if <0 then I recompute

                                  // inputs
                                  // if outliers are grouped into features, the
                                  // feature size is set here
                                  int featureSize,
                                  int Nfeatures,
                                  int NoutlierFeatures, // how many outliers we already have
                                  dogleg_operatingPoint_t* point,
                                  dogleg_solverContext_t* ctx )
{
  if(featureSize <= 1)
    featureSize = 1;

  dogleg_computeJtJfactorization( point, ctx );
  bool result;
  if(ctx->is_sparse)
    result = getOutliernessFactors_sparse(factors, scale, featureSize, Nfeatures, NoutlierFeatures, point, ctx);
  else
    result = getOutliernessFactors_dense(factors, scale, featureSize, Nfeatures, NoutlierFeatures, point, ctx);

#if 0
  if( result )
  {
    int  Nstate        = ctx->Nstate;
    int  Nmeasurements = ctx->Nmeasurements;



    static FILE* fp = NULL;
    if(fp == NULL)
      fp = fopen("/tmp/check-outlierness.py", "w");
    static int count = -1;
    count++;
    if(count > 5)
      goto done;

    fprintf(fp, "# WARNING: all J here are unscaled with SCALE_....\n");

    if(count == 0)
    {
      fprintf(fp,
              "import numpy as np\n"
              "import numpysane as nps\n"
              "np.set_printoptions(linewidth=100000)\n"
              "\n");
    }

    fprintf(fp, "x%d = np.array((", count);
    for(int j=0;j<Nmeasurements;j++)
      fprintf(fp, "%.20g,", point->x[j]);
    fprintf(fp,"))\n");

    if( ctx->is_sparse )
    {
      fprintf(fp, "J%d = np.zeros((%d,%d))\n", count, Nmeasurements, Nstate);
      for(int imeas=0;imeas<Nmeasurements;imeas++)
      {
        for(int j = P(point->Jt, imeas);
            j     < (int)P(point->Jt, imeas+1);
            j++)
        {
          int irow = I(point->Jt, j);
          fprintf(fp, "J%d[%d,%d] = %.20g\n", count,
                  imeas, irow, X(point->Jt, j));
        }
      }
    }
    else
    {
      fprintf(fp, "J%d = np.array((\n", count);
      for(int j=0;j<Nmeasurements;j++)
      {
        fprintf(fp, "(");
        for(int i=0;i<Nstate;i++)
          fprintf(fp, "%.20g,", point->J_dense[j*Nstate+i]);
        fprintf(fp, "),\n");
      }
      fprintf(fp,"))\n");
    }

    fprintf(fp, "Nmeasurements = %d\n", Nmeasurements);
    fprintf(fp, "Nstate = %d\n", Nstate);
    fprintf(fp, "Nfeatures = %d\n", Nfeatures);
    fprintf(fp, "featureSize = %d\n", featureSize);
    fprintf(fp, "NoutlierFeatures = %d\n", NoutlierFeatures);

    fprintf(fp, "scale_got = %.20f\n", *scale);

    fprintf(fp, "factors_got = np.array((");
    for(int j=0;j<Nfeatures;j++)
      fprintf(fp, "%.20g,", factors[j]);
    fprintf(fp,"))\n");

    fprintf(fp,
            "Nmeasurements_nonoutliers = Nmeasurements - NoutlierFeatures*featureSize\n"
            "scale = Nmeasurements_nonoutliers / (4.*((Nstate+1.) * nps.inner(x%1$d,x%1$d)/(Nmeasurements_nonoutliers - Nstate - 1.)))\n"
            "pinvj = np.linalg.pinv(J%1$d)\n"
            "imeas0 = [featureSize*i for i in xrange(Nfeatures)]\n"
            "jslices = [J%1$d[imeas0[i]:(imeas0[i]+featureSize), :] for i in xrange(Nfeatures)]\n"
            "xslices = [x%1$d[imeas0[i]:(imeas0[i]+featureSize)   ] for i in xrange(Nfeatures)]\n"
            "A       = [nps.matmult(jslices[i],pinvj[:,imeas0[i]:(imeas0[i]+featureSize)]) for i in xrange(Nfeatures)]\n"
            "B       = [np.linalg.inv(A[i] - np.eye(featureSize)) for i in xrange(Nfeatures)]\n"

            "factors_ref = np.array([nps.matmult(xslices[i],"



            "                                    B[i] + nps.matmult(B[i],B[i])," // Cook's self+others
            //"                                    -B[i]," // Dima's self+others

            "                                    nps.transpose(xslices[i])).ravel()"
            "                        for i in xrange(Nfeatures)]).ravel() * scale\n"

            "factor_err = factors_ref-factors_got\n"
            "scale_err = scale-scale_got\n",
            count);

    fprintf(fp, "print 'RMS discrepancy in the factor (should be 0): {}'.format(np.sqrt(np.mean(factor_err*factor_err)))\n");
    fprintf(fp, "print 'Scale error: {}'.format(scale_err)\n");

    if(featureSize <= 1)
    {
      fprintf(fp,
              "factors_ref1 = x%1$d * x%1$d / (1.0 - nps.inner(J%1$d, nps.transpose(pinvj))) * scale\n",
              count

              );
      fprintf(fp, "print 'normdiff1 (should be 0): {}'.format(np.linalg.norm(factors_ref1-factors_got))\n");
      fprintf(fp, "print 'normrefref1: {}'.format(np.linalg.norm(factors_ref1-factors_ref))\n");
    }
    fflush(fp);
  }
 done:
#endif

  return result;
}

double dogleg_getOutliernessTrace_newFeature_sparse(const double*            JqueryFeature,
                                                    int                      istateActive,
                                                    int                      NstateActive,
                                                    int                      featureSize,
                                                    int                      NoutlierFeatures,
                                                    dogleg_operatingPoint_t* point,
                                                    dogleg_solverContext_t*  ctx)
{
  /*
    See the big comment above for a description

    The outlierness I'm using here has the form x*t C x where x =
    project(q)-xref. xref is a random variable of a potential query point with
    mean at project(q) (so that x has mean 0). I conceivably have a distribution
    for xref and I know its variance. For a quadratic form I can compute the
    expected value E(x*t C x) = trace(C Var(x*)). I'm going to assume that x*
    are all independent and identical, so Var(x*) = observation_variance*I, and
    E(x*t C x) = trace(C) observation_variance. I thus let the caller deal with
    the observation variance, and I just return trace(C). The test at
    mrcal/analyses/outlierness-test.py suggests that "Dima's self+others" is the
    metric we want to use, so C = B = inv(J* inv(JtJ) J*t + I)

    k*trace(inv(I + J* inv(JtJ) J*t))

    A = J* inv(JtJ) J*t ->
    B = inv(I + A)      ->
    tr(B) = (2+a00+a11) / ((1+a00)*(1+a11) - a01^2)
  */


  // This is Jt because cholmod thinks in terms of col-first instead of
  // row-first
  std::vector<int> Jt_p(featureSize+1);
  std::vector<int> Jt_i(NstateActive*featureSize);
  for(int i=0; i<=featureSize; i++)
  {
    Jt_p[i] = i*NstateActive;
    if(i==featureSize) break;
    for(int j=0; j<NstateActive; j++)
      Jt_i[j + i*NstateActive] = istateActive + j;
  }
  cholmod_sparse Jt_query_sparse = {.nrow   = static_cast<size_t>(ctx->Nstate),
                                    .ncol   = static_cast<size_t>(featureSize),
                                    .nzmax  = static_cast<size_t>(NstateActive*featureSize),
                                    .p      = Jt_p.data(),
                                    .i      = Jt_i.data(),
                                    .x      = const_cast<double*>(JqueryFeature),
                                    .stype  = 0, // NOT symmetric
                                    .itype  = CHOLMOD_INT,
                                    .xtype  = CHOLMOD_REAL,
                                    .dtype  = CHOLMOD_DOUBLE,
                                    .sorted = 1,
                                    .packed = 1};

  // Really shouldn't need to do this every time. In fact I probably don't need
  // to do it at all, since this will have been done by the solver during the
  // last step
  dogleg_computeJtJfactorization( point, ctx );
  cholmod_sparse* invJtJ_Jp =
    cholmod_spsolve(CHOLMOD_A,
                    ctx->factorization,
                    &Jt_query_sparse,
                    &ctx->common);

  // Now I need trace(matmult(Jquery, invJtJ_Jp))

  // haven't implemented anything else yet
  ASSERT(featureSize == 2);
  double A[4] = {}; // gah. only elements 0,1,3 will be stored.

  for(int i=0; i<featureSize; i++)
  {
    for(unsigned int j=P(invJtJ_Jp, i); j<P(invJtJ_Jp, i+1); j++)
    {
      int row = I(invJtJ_Jp, j);
      if(row >= istateActive)
      {
        if(row >= istateActive+NstateActive)
          break;

        int ic0 = i*featureSize;
        for(int k=i; k<featureSize; k++) // computing one triangle
          A[ic0+k] += X(invJtJ_Jp, j)*JqueryFeature[row-istateActive + k*NstateActive];
      }
    }
  }

  cholmod_free_sparse(&invJtJ_Jp, &ctx->common);

  double invB00 = A[0]+1.0;
  double invB01 = A[1];
  double invB11 = A[3]+1.0;

  double det_invB_recip = 1.0/(invB00*invB11 - invB01*invB01);
  double B00 =  invB11 * det_invB_recip;
  double B11 =  invB00 * det_invB_recip;

  
  double B01 = -invB01 * det_invB_recip;
  double traceB = B00 + B11;
  
  double traceBB = B00*B00 + 2.0*B01*B01 + B11*B11;


#if 0
  static int count = -1;
  count++;
  if(count <= 5)
  {
    int  Nstate        = ctx->Nstate;
    int  Nmeasurements = ctx->Nmeasurements;



    static FILE* fp = NULL;
    if(fp == NULL)
      fp = fopen("/tmp/check-query-outlierness.py", "w");

    fprintf(fp, "# WARNING: all J here are unscaled with SCALE_....\n");

    if(count == 0)
    {
      fprintf(fp,
              "import numpy as np\n"
              "import numpysane as nps\n"
              "np.set_printoptions(linewidth=100000)\n"
              "\n");
    }

    fprintf(fp, "x%d = np.array((", count);
    for(int j=0;j<Nmeasurements;j++)
      fprintf(fp, "%.20g,", point->x[j]);
    fprintf(fp,"))\n");

    if( ctx->is_sparse )
    {
      fprintf(fp, "J%d = np.zeros((%d,%d))\n", count, Nmeasurements, Nstate);
      for(int imeas=0;imeas<Nmeasurements;imeas++)
      {
        for(int j = P(point->Jt, imeas);
            j     < (int)P(point->Jt, imeas+1);
            j++)
        {
          int irow = I(point->Jt, j);
          fprintf(fp, "J%d[%d,%d] = %g\n", count,
                  imeas, irow, X(point->Jt, j));
        }
      }
    }
    else
    {
      fprintf(fp, "J%d = np.array((\n", count);
      for(int j=0;j<Nmeasurements;j++)
      {
        fprintf(fp, "(");
        for(int i=0;i<Nstate;i++)
          fprintf(fp, "%.20g,", point->J_dense[j*Nstate+i]);
        fprintf(fp, "),\n");
      }
      fprintf(fp,"))\n");
    }

    fprintf(fp, "Nmeasurements = %d\n", Nmeasurements);
    fprintf(fp, "Nstate = %d\n", Nstate);
    fprintf(fp, "featureSize = %d\n", featureSize);
    fprintf(fp, "traceB_got = %.20g\n", traceB);

    fprintf(fp, "Jq = np.zeros((featureSize, Nstate), dtype=float)\n");
    for(int i=0; i<NstateActive; i++)
      for(int j=0; j<featureSize; j++)
        fprintf(fp, "Jq[%d,%d] = %.20g\n", j, i + istateActive, JqueryFeature[j*NstateActive + i]);

    fprintf(fp,
            "A = nps.matmult(Jq, np.linalg.solve( nps.matmult(nps.transpose(J%1$d),J%1$d), nps.transpose(Jq)))\n"
            "B = np.linalg.inv( A + np.eye(featureSize) )\n",
            count);

    fprintf(fp, "print 'trace got,ref,rr: {}, {}, {}'.format(traceB_got,np.trace(B),traceB_got-np.trace(B))\n");
    fflush(fp);
  }
#endif


  int Nmeasurements = ctx->Nmeasurements;
  int Nstate        = ctx->Nstate;

  double scale = -1.0;
  getOutliernessScale(&scale,
                      Nmeasurements, Nstate, NoutlierFeatures, featureSize, point->norm2_x);

  // // Dima's self+others
  // return scale * traceB;

  // Cook's self+others
  return scale * (2.0 - traceB);

  // // Dima's others/Cook's others
  // // This one is non-monotonic in outlierness-test
  // return scale * (traceB - traceBB);

}


#define OUTLIER_CONFIDENCE_DROP_THRESHOLD 0.05
bool dogleg_markOutliers(// output, input
                         struct dogleg_outliers_t* markedOutliers,
                         double* scale, // if <0 then I recompute

                         // output, input
                         int* Noutliers,

                         // input
                         double (getConfidence)(int i_feature_exclude),

                         // if outliers are grouped into features, the feature
                         // size is set here
                         int featureSize,
                         int Nfeatures,

                         dogleg_operatingPoint_t* point,
                         dogleg_solverContext_t* ctx)
{
    if(featureSize <= 1)
      featureSize = 1;

    bool markedAny = false;

    double* factors = static_cast<double*>(malloc(Nfeatures * sizeof(double)));
    if(factors == NULL)
    {
        SAY("Error allocating factors");
        free(factors);
        return markedAny;
    }

    if(!dogleg_getOutliernessFactors(factors, scale,
                                     featureSize, Nfeatures,
                                     *Noutliers,
                                     point, ctx))
    {
      free(factors);
      return markedAny;
    }

    // I have my list of POTENTIAL outliers (any that have factor > 1.0). I
    // check to see how much confidence I would lose if I were to throw out any
    // of these measurements, and accept the outlier ONLY if the confidence loss
    // is acceptable
    double confidence0 = getConfidence(-1);
    if( confidence0 < 0.0 )
    {
      free(factors);
      return markedAny;
    }

    SAY_IF_VERBOSE("Initial confidence: %g", confidence0);

    *Noutliers = 0;
    for(int i=0; i<Nfeatures; i++)
    {
        if(markedOutliers[i].marked)
        {
          (*Noutliers)++;
          continue;
        }

        if(factors[i] < 1.0)
            continue;

        // Looking at potential new outlier
        double confidence_excluded = getConfidence(i);
        if( confidence_excluded < 0.0 )
            return false;

        double confidence_drop_relative = 1.0 - confidence_excluded / confidence0;
        if( confidence_drop_relative < OUTLIER_CONFIDENCE_DROP_THRESHOLD )
        {
            // I would lose less than X of my confidence. OK. This is an
            // outlier. Throw it away.
            markedOutliers[i].marked = true;
            markedAny                = true;
            SAY_IF_VERBOSE("Feature %d has outlierness factor %f. Culling produces a confidence: %g. relative loss: %g... YES an outlier; confidence drops little",
                           i, factors[i], confidence_excluded, confidence_drop_relative);
            (*Noutliers)++;
        }
        else
        {
            SAY_IF_VERBOSE("Feature %d has outlierness factor %f. Culling produces a confidence: %g. relative loss: %g... NOT an outlier: confidence drops too much",
                           i, factors[i], confidence_excluded, confidence_drop_relative);
        }
    }

    free(factors);
    return markedAny;
}

// This function is just for debug reporting. It is probably too slow to
// call in general: it computes the confidence for each feature to see the
// confidence change if the feature were to be removed. Normally we do this
// ONLY for potential outliers
void dogleg_reportOutliers( double (getConfidence)(int i_feature_exclude),
                            double* scale, // if <0 then I recompute

                            // if outliers are grouped into features, the
                            // feature size is set here
                            int featureSize,
                            int Nfeatures,
                            int Noutliers, // how many outliers we already have

                            dogleg_operatingPoint_t* point,
                            dogleg_solverContext_t* ctx)
{
    if(featureSize <= 1)
      featureSize = 1;

    double* factors = static_cast<double*>(malloc(Nfeatures * sizeof(double)));
    if(factors == NULL)
    {
        SAY("Error allocating factors");
        free(factors);
    }

    dogleg_getOutliernessFactors(factors, scale,
                                 featureSize, Nfeatures, Noutliers, point, ctx);

    SAY("## Outlier statistics");
    SAY("# i_feature outlier_factor confidence_drop_relative_if_removed");

    double confidence_full = getConfidence(-1);

    for(int i=0; i<Nfeatures; i++)
    {
      double confidence = getConfidence(i);
      double rot_confidence_drop_relative = 1.0 - confidence / confidence_full;

      SAY("%5d %9.3g %9.3g",
          i,
          factors[i],
          rot_confidence_drop_relative);
    }

    free(factors);
}
