// -*- mode: C; c-basic-offset: 2 -*-
// Copyright 2011 Oblong Industries
//           2017 Dima Kogan <dima@secretsauce.net>
// License: GNU LGPL <http://www.gnu.org/licenses>.

#pragma once

#include <suitesparse/cholmod.h>
#include <assert.h>
#include <stdbool.h>

typedef void (dogleg_callback_t)(// in
                                 // shape (Nstate,)
                                 const double*   p,
                                 // out
                                 // shape (Nmeasurements,)
                                 double*         x,
                                 // shape (Nstate,Nmeasurements,)
                                 cholmod_sparse* Jt,
                                 // context
                                 void*           cookie);
typedef void (dogleg_callback_dense_t)(// in
                                       // shape (Nstate,)
                                       const double*   p,
                                       // out
                                       // shape (Nmeasurements,)
                                       double*         x,
                                       // shape (Nmeasurements,Nstate,)
                                       double*         J,
                                       // context
                                       void*           cookie);

// like dogleg_callback_dense_t, but returns much smaller arrays because Nstate
// << Nmeasurements
typedef void (dogleg_callback_dense_products_t)(// in
                                                // shape (Nstate,)
                                                const double*   p,
                                                // out
                                                // scalar
                                                double*         norm2x,
                                                // shape (Nstate,)
                                                double*         xtJ,
                                                // shape (Nstate,Nstate)
                                                double*         JtJ,
                                                // context
                                                void*           cookie);

// an operating point of the solver
typedef struct
{
  // The pointers in this structure are all indices into a single "pool" array
  // (see allocOperatingPoint()). I thus don't need to store the pointers at
  // all, and can just index that one array directly, but that would break my
  // ABI

  // This is always available
  double*         p;

  // These may or may not be available at any given time because they may not
  // have been computed yet.
  double*         x;
  double          norm2_x;
  // This is a union. DOGLEG_DENSE_PRODUCTS has JtJ, but no J. DOGLEG_SPARSE and
  // DOGLEG_DENSE has just J
  union
  {
    cholmod_sparse* Jt;
    double*         J_dense; // row-first: grad0, grad1, grad2, ...
    double*         JtJ;     // possibly one packed triangle of a symmetric matrix
  };
  double*         Jt_x;

  // the cached update vectors. It's useful to cache these so that when a step is rejected, we can
  // reuse these when we retry
  double*        updateCauchy;
  union
  {
    cholmod_dense* updateGN_cholmoddense;
    double*        updateGN_dense;
  };
  double         norm2_updateCauchy, norm2_updateGN; // update vector lengths

  // Which elements of this structure contain valid data. The union is there for
  // compatibility with the previous API. I want to preserve the old layout and
  // sizeof(dogleg_operatingPoint_t); I don't care about renaming THESE
  // variables since nobody is using them
  union
  {
    int dummy_bits[3];
    struct {
      bool have_updateCauchy          : 1; // and the norm2
      bool have_updateGN              : 1; // and the norm2 and the factorization
      bool have_factorization         : 1; // and the norm2 and the factorization
      bool have_x                     : 1; // just x; norm2_x is always valid
      bool have_J                     : 1; // Jt or J_dense
      bool have_Jtx                   : 1;
      bool have_JtJ                   : 1;
      bool have_step_to_here          : 1; // and the norm2
      bool didStepToEdgeOfTrustRegion : 1;
    };
  };

  double* step_to_here;
  double  norm2_step_to_here;

} dogleg_operatingPoint_t;

#define DOGLEG_DEBUG_VNLOG_BIT 30
// The newer APIs ( dogleg_...2() ) take a dogleg_parameters2_t argument for
// their settings, while the older ones use a global set of parameters specified
// with dogleg_set_...(). This global-parameters approach doesn't work if we
// have multiple users of libdogleg in the same application
typedef struct
{
  int max_iterations;
  union
  {
    int dogleg_debug; // Don't touch this in new code. Use the bit fields below
    struct
    {
      bool debug       : 1;

      // if packed, use the LAPACK packed storage: represent
      //   [ A B C ]
      //   [ B D E ]
      //   [ C E F ]
      // as [A B C D E F] if upper or
      // as [A B D C E F] if lower
      bool JtJ_packed  : 1;
      bool JtJ_upper   : 1;
      int  dummy       : DOGLEG_DEBUG_VNLOG_BIT-3;
      bool debug_vnlog : 1; // in the same spot as dogleg_debug
    };
  };

  // it is cheap to reject a too-large trust region, so I start with something
  // "large". The solver will quickly move down to something reasonable. Only the
  // user really knows if this is "large" or not, so they should change this via
  // the API
  double trustregion0;

  // These are probably OK to leave alone. Tweaking them can maybe result in
  // slightly faster convergence
  double trustregion_decrease_factor;
  double trustregion_decrease_threshold;
  double trustregion_increase_factor;
  double trustregion_increase_threshold;

  // The termination thresholds. Documented in the header
  double Jt_x_threshold;
  double update_threshold;
  double trustregion_threshold;
} dogleg_parameters2_t;
// I also need to make sure that the "debug_vnlog" bit is DOGLEG_DEBUG_VNLOG. I
// can't use a static-assert, so I do that in test-misc.c

typedef enum {
  DOGLEG_DENSE          = 0,
  DOGLEG_SPARSE         = 1,
  DOGLEG_DENSE_PRODUCTS = 2 } dogleg_solve_type_t;
static_assert(sizeof(dogleg_solve_type_t) == sizeof(int),
               "solve_type should be as big as an int, to maintain ABI compatibility");

// solver context. This has all the internal state of the solver
typedef struct
{
  cholmod_common  common;

  union
  {
    dogleg_callback_t*                f;
    dogleg_callback_dense_t*          f_dense;
    dogleg_callback_dense_products_t* f_dense_products;
  };
  void*              cookie;

  // between steps, beforeStep contains the operating point of the last step.
  // afterStep is ONLY used while making the step. Externally, use beforeStep
  // unless you really know what you're doing
  dogleg_operatingPoint_t* beforeStep;
  dogleg_operatingPoint_t* afterStep;

  // The result of the last JtJ factorization performed. Note that JtJ is not
  // necessarily factorized at every step, so this is NOT guaranteed to contain
  // the factorization of the most recent JtJ. Use
  // point->have.updateGN_and_factorization
  union
  {
    cholmod_factor* factorization;

    // This is a factorization of JtJ, stored as a packed symmetric matrix
    // returned by dpptrf('L', ...)
    double*         factorization_dense;
  };

  // Have I ever seen a singular JtJ? If so, I add this constant to the diagonal
  // from that point on. This is a simple and fast way to deal with
  // singularities. This constant starts at 0, and is increased every time a
  // singular JtJ is encountered. This is suboptimal but works for me for now.
  double                   lambda;

  // Are we using sparse math (cholmod)?
  dogleg_solve_type_t solve_type;

  int Nstate, Nmeasurements;

  const dogleg_parameters2_t* parameters;

} dogleg_solverContext_t;

#ifdef __cplusplus
extern "C" {
#endif
// Fills in the given structure with the default parameter set
void dogleg_getDefaultParameters(dogleg_parameters2_t* parameters);

void dogleg_setMaxIterations(int n);
void dogleg_setTrustregionUpdateParameters(double downFactor, double downThreshold,
                                           double upFactor,   double upThreshold);

// The argument is a bit-mapped integer. Should be a bit-field structure or
// enum, but for API backwards-compatibility, I keep this an integer.
//
// if(debug == 0                 ): no diagnostic output
// if(debug &  DOGLEG_DEBUG_VNLOG): output vnlog diagnostics to stdout
// if(debug & ~DOGLEG_DEBUG_VNLOG): output human-oriented diagnostics to stderr
#define DOGLEG_DEBUG_VNLOG     (1 << DOGLEG_DEBUG_VNLOG_BIT)
void dogleg_setDebug(int debug);


// The following parameters reflect the scaling of the problem being solved, so
// the user is strongly encouraged to tweak these. The defaults are set
// semi-arbitrarily

// The size of the trust region at start. It is cheap to reject a too-large
// trust region, so this should be something "large". Say 10x the length of an
// "expected" step size
void dogleg_setInitialTrustregion(double t);

// termination thresholds. These really depend on the scaling of the input
// problem, so the user should set these appropriately
//
// Jt_x threshold:
// The function being minimized is E = norm2(x) where x is a function of the state p.
// dE/dp = 2*Jt*x where Jt is transpose(dx/dp).
//   if( for every i  fabs(Jt_x[i]) < JT_X_THRESHOLD )
//   { we are done; }
//
// update threshold:
//   if(for every i  fabs(state_update[i]) < UPDATE_THRESHOLD)
//   { we are done; }
//
// trust region threshold:
//   if(trustregion < TRUSTREGION_THRESHOLD)
//   { we are done; }
//
// to leave a particular threshold alone, use a value <= 0 here
void dogleg_setThresholds(double Jt_x, double update, double trustregion);


// The main optimization function. Initial estimate of the solution passed in p,
// final optimized solution returned in p. p has Nstate variables. There are
// Nmeas measurements, the jacobian matrix has NJnnz non-zero entries.
//
// The evaluation function is given in the callback f; this function is passed
// the given cookie
//
// The parameters can be NULL to use the default ones, controlled by
// dogleg_set...() functions. If given, the dogleg_parameters2_t structure will
// be used as is: set it up with dogleg_getDefaultParameters(), and modify as
// needed. This structure needs to exist in memory for the duration of the
// libdogleg solve
//
// If we want to get the full solver state when we're done, a non-NULL
// returnContext pointer can be given. If this is done then THE USER IS
// RESPONSIBLE FOR FREEING ITS MEMORY WITH dogleg_freeContext()
//
// Returns norm2(x) on success or <0 on error
double dogleg_optimize(double* p, unsigned int Nstate,
                       unsigned int Nmeas, unsigned int NJnnz,
                       dogleg_callback_t* f, void* cookie,
                       dogleg_solverContext_t** returnContext);
double dogleg_optimize2(double* p, unsigned int Nstate,
                        unsigned int Nmeas, unsigned int NJnnz,
                        dogleg_callback_t* f, void* cookie,
                        const dogleg_parameters2_t* parameters,
                        dogleg_solverContext_t** returnContext);

// Main optimization function if we're using dense linear algebra. The arguments
// are very similar to the sparse version: dogleg_optimize()
double dogleg_optimize_dense(double* p, unsigned int Nstate,
                             unsigned int Nmeas,
                             dogleg_callback_dense_t* f, void* cookie,
                             dogleg_solverContext_t** returnContext);
double dogleg_optimize_dense2(double* p, unsigned int Nstate,
                              unsigned int Nmeas,
                              dogleg_callback_dense_t* f, void* cookie,
                              const dogleg_parameters2_t* parameters,
                              dogleg_solverContext_t** returnContext);
double dogleg_optimize_dense_products(double* p, unsigned int Nstate,
                                      dogleg_callback_dense_products_t* f, void* cookie,
                                      const dogleg_parameters2_t* parameters,
                                      dogleg_solverContext_t** returnContext);

// Compute the cholesky decomposition of JtJ. This function is only exposed if
// you need to touch libdogleg internals via returnContext. Sometimes after
// computing an optimization you want to do stuff with the factorization of JtJ,
// and this call ensures that the factorization is available. Most people don't
// need this function. If the comment wasn't clear, you don't need this
// function.
bool dogleg_computeJtJfactorization(dogleg_operatingPoint_t* point, dogleg_solverContext_t* ctx);

// Generates a plain text table that contains gradient checks. This table can be
// easily parsed with "vnlog" tools
void dogleg_testGradient(unsigned int var, const double* p0,
                         unsigned int Nstate, unsigned int Nmeas, unsigned int NJnnz,
                         dogleg_callback_t* f, void* cookie);
void dogleg_testGradient_dense(unsigned int var, const double* p0,
                               unsigned int Nstate, unsigned int Nmeas,
                               dogleg_callback_dense_t* f, void* cookie);

// This only kinda works. If J isn't constant, this will report gradient errors.
// Interpret failures with caution
void dogleg_testGradient_dense_products(unsigned int var, const double* p0,
                                        unsigned int Nstate, unsigned int Nmeas,
                                        dogleg_callback_dense_products_t* f, void* cookie,
                                        const dogleg_parameters2_t* parameters);

// If we want to get the full solver state when we're done optimizing, we can
// pass a non-NULL returnContext pointer to dogleg_optimize(). If we do this,
// then the user MUST call dogleg_freeContext() to deallocate the pointer when
// the USER is done
void dogleg_freeContext(dogleg_solverContext_t** ctx);


// Computes outlierness factors. This function is experimental, and subject to
// change.
bool dogleg_getOutliernessFactors( // output
                                  double* factors, // Nfeatures factors

                                  // output, input
                                  double* scale, // if <0 then I recompute

                                  // inputs
                                  // if outliers are grouped into features, the feature size is
                                  // stated here
                                  int featureSize,
                                  int Nfeatures,
                                  int NoutlierFeatures, // how many outliers we already have
                                  dogleg_operatingPoint_t* point,
                                  dogleg_solverContext_t* ctx );




// This stuff is experimental, and subject to change.
struct dogleg_outliers_t
{
    unsigned char marked : 1;
};
bool dogleg_markOutliers(// output, input
                         struct dogleg_outliers_t* markedOutliers,
                         double* scale, // if <0 then I recompute

                         // output, input
                         int* Noutliers, // number of outliers before and after this call

                         // input
                         double (getConfidence)(int i_feature_exclude),

                         // if outliers are grouped into features, the feature size is
                         // stated here
                         int featureSize,
                         int Nfeatures,

                         dogleg_operatingPoint_t* point,
                         dogleg_solverContext_t* ctx);

void dogleg_reportOutliers( double (getConfidence)(int i_feature_exclude),
                            double* scale, // if <0 then I recompute

                            // if outliers are grouped into features, the feature size
                            // is stated here
                            int featureSize,
                            int Nfeatures,
                            int Noutliers, // how many outliers we already have

                            dogleg_operatingPoint_t* point,
                            dogleg_solverContext_t* ctx);

double dogleg_getOutliernessTrace_newFeature_sparse(const double*            JqueryFeature,
                                                    int                      istateActive,
                                                    int                      NstateActive,
                                                    int                      featureSize,
                                                    int                      NoutlierFeatures,
                                                    dogleg_operatingPoint_t* point,
                                                    dogleg_solverContext_t*  ctx);
#ifdef __cplusplus
} // extern "C"
#endif
