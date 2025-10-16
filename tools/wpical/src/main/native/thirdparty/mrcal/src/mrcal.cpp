// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Apparently I need this in MSVC to get constants
#include <vector>
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

extern "C" {
#include <dogleg.h>
}
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

extern "C" {
#include "mrcal.h"
#include "minimath/minimath.h"
#include "cahvore.h"
#include "minimath/minimath-extra.h"
#include "util.h"
#include "scales.h"
}
#define MSG_IF_VERBOSE(...) do { if(verbose) MSG( __VA_ARGS__ ); } while(0)

#define restrict __restrict

// Returns a static string, using "..." as a placeholder for any configuration
// values
#define LENSMODEL_PRINT_CFG_ELEMENT_TEMPLATE(name, type, pybuildvaluecode, PRIcode,SCNcode, bitfield, cookie) \
    "_" #name "=..."
#define LENSMODEL_PRINT_CFG_ELEMENT_FMT(name, type, pybuildvaluecode, PRIcode,SCNcode, bitfield, cookie) \
    "_" #name "=%" PRIcode
#define LENSMODEL_PRINT_CFG_ELEMENT_VAR(name, type, pybuildvaluecode, PRIcode,SCNcode, bitfield, cookie) \
    ,config->name
#define LENSMODEL_SCAN_CFG_ELEMENT_FMT(name, type, pybuildvaluecode, PRIcode,SCNcode, bitfield, cookie) \
    "_" #name "=%" SCNcode
#define LENSMODEL_SCAN_CFG_ELEMENT_VAR(name, type, pybuildvaluecode, PRIcode,SCNcode, bitfield, cookie) \
    ,&config->name
#define LENSMODEL_SCAN_CFG_ELEMENT_PLUS1(name, type, pybuildvaluecode, PRIcode,SCNcode, bitfield, cookie) \
    +1
const char* mrcal_lensmodel_name_unconfigured( const mrcal_lensmodel_t* lensmodel )
{
    switch(lensmodel->type)
    {
#define CASE_STRING_NOCONFIG(s,n) case MRCAL_##s: ;     \
        return #s;
#define _CASE_STRING_WITHCONFIG(s,n,s_CONFIG_LIST) case MRCAL_##s: ;    \
        return #s s_CONFIG_LIST(LENSMODEL_PRINT_CFG_ELEMENT_TEMPLATE, );
#define CASE_STRING_WITHCONFIG(s,n) _CASE_STRING_WITHCONFIG(s,n,MRCAL_ ## s ## _CONFIG_LIST)

        MRCAL_LENSMODEL_NOCONFIG_LIST(                   CASE_STRING_NOCONFIG )
        MRCAL_LENSMODEL_WITHCONFIG_STATIC_NPARAMS_LIST(  CASE_STRING_WITHCONFIG )
        MRCAL_LENSMODEL_WITHCONFIG_DYNAMIC_NPARAMS_LIST( CASE_STRING_WITHCONFIG )

    default:
        return NULL;


#undef CASE_STRING_NOCONFIG
#undef CASE_STRING_WITHCONFIG

    }
    return NULL;
}

// Write the model name WITH the full config into the given buffer. Identical to
// mrcal_lensmodel_name_unconfigured() for configuration-free models
static int LENSMODEL_CAHVORE__snprintf_model
  (char* out, int size,
   const mrcal_LENSMODEL_CAHVORE__config_t* config)
{
    return
        snprintf( out, size, "LENSMODEL_CAHVORE"
                  MRCAL_LENSMODEL_CAHVORE_CONFIG_LIST(LENSMODEL_PRINT_CFG_ELEMENT_FMT, )
                  MRCAL_LENSMODEL_CAHVORE_CONFIG_LIST(LENSMODEL_PRINT_CFG_ELEMENT_VAR, ));
}
static int LENSMODEL_SPLINED_STEREOGRAPHIC__snprintf_model
  (char* out, int size,
   const mrcal_LENSMODEL_SPLINED_STEREOGRAPHIC__config_t* config)
{
    return
        snprintf( out, size, "LENSMODEL_SPLINED_STEREOGRAPHIC"
                  MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC_CONFIG_LIST(LENSMODEL_PRINT_CFG_ELEMENT_FMT, )
                  MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC_CONFIG_LIST(LENSMODEL_PRINT_CFG_ELEMENT_VAR, ));
}
bool mrcal_lensmodel_name( char* out, int size, const mrcal_lensmodel_t* lensmodel )
{
    switch(lensmodel->type)
    {
#define CASE_STRING_NOCONFIG(s,n) case MRCAL_##s: \
        return size > snprintf(out,size, #s);

#define CASE_STRING_WITHCONFIG(s,n) case MRCAL_##s: \
        return size > s##__snprintf_model(out, size, &lensmodel->s##__config);

        MRCAL_LENSMODEL_NOCONFIG_LIST(                   CASE_STRING_NOCONFIG )
        MRCAL_LENSMODEL_WITHCONFIG_STATIC_NPARAMS_LIST(  CASE_STRING_WITHCONFIG )
        MRCAL_LENSMODEL_WITHCONFIG_DYNAMIC_NPARAMS_LIST( CASE_STRING_WITHCONFIG )

    default:
        return NULL;

#undef CASE_STRING_NOCONFIG
#undef CASE_STRING_WITHCONFIG

    }
    return NULL;
}


static bool LENSMODEL_CAHVORE__scan_model_config( mrcal_LENSMODEL_CAHVORE__config_t* config, const char* config_str)
{
    int pos;
    int Nelements = 0 MRCAL_LENSMODEL_CAHVORE_CONFIG_LIST(LENSMODEL_SCAN_CFG_ELEMENT_PLUS1, );
    return
        Nelements ==
        sscanf( config_str,
                MRCAL_LENSMODEL_CAHVORE_CONFIG_LIST(LENSMODEL_SCAN_CFG_ELEMENT_FMT, )"%n"
                MRCAL_LENSMODEL_CAHVORE_CONFIG_LIST(LENSMODEL_SCAN_CFG_ELEMENT_VAR, ),
                &pos) &&
        config_str[pos] == '\0';
}
static bool LENSMODEL_SPLINED_STEREOGRAPHIC__scan_model_config( mrcal_LENSMODEL_SPLINED_STEREOGRAPHIC__config_t* config, const char* config_str)
{
    int pos;
    int Nelements = 0 MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC_CONFIG_LIST(LENSMODEL_SCAN_CFG_ELEMENT_PLUS1, );
    return
        Nelements ==
        sscanf( config_str,
                MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC_CONFIG_LIST(LENSMODEL_SCAN_CFG_ELEMENT_FMT, )"%n"
                MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC_CONFIG_LIST(LENSMODEL_SCAN_CFG_ELEMENT_VAR, ),
                &pos) &&
        config_str[pos] == '\0';
}

const char* const* mrcal_supported_lensmodel_names( void )
{
#define NAMESTRING_NOCONFIG(s,n)                  #s,
#define _NAMESTRING_WITHCONFIG(s,n,s_CONFIG_LIST) #s s_CONFIG_LIST(LENSMODEL_PRINT_CFG_ELEMENT_TEMPLATE, ),
#define NAMESTRING_WITHCONFIG(s,n) _NAMESTRING_WITHCONFIG(s,n,MRCAL_ ## s ## _CONFIG_LIST)

    static const char* names[] = {
        MRCAL_LENSMODEL_NOCONFIG_LIST(                  NAMESTRING_NOCONFIG)
        MRCAL_LENSMODEL_WITHCONFIG_STATIC_NPARAMS_LIST( NAMESTRING_WITHCONFIG)
        MRCAL_LENSMODEL_WITHCONFIG_DYNAMIC_NPARAMS_LIST(NAMESTRING_WITHCONFIG)
        NULL };
    return names;
}

#undef LENSMODEL_PRINT_CFG_ELEMENT_TEMPLATE
#undef LENSMODEL_PRINT_CFG_ELEMENT_FMT
#undef LENSMODEL_PRINT_CFG_ELEMENT_VAR
#undef LENSMODEL_SCAN_CFG_ELEMENT_FMT
#undef LENSMODEL_SCAN_CFG_ELEMENT_VAR
#undef LENSMODEL_SCAN_CFG_ELEMENT_PLUS1

// parses the model name AND the configuration into a mrcal_lensmodel_t structure.
// On error returns false with lensmodel->type set to MRCAL_LENSMODEL_INVALID_...
bool mrcal_lensmodel_from_name( // output
                                mrcal_lensmodel_t* lensmodel,

                                // input
                                const char* name )
{
#define CHECK_AND_RETURN_NOCONFIG(s,n)                                  \
    if( 0 == strcmp( name, #s) )                                        \
    {                                                                   \
        *lensmodel = (mrcal_lensmodel_t){.type = MRCAL_##s};            \
        return true;                                                    \
    }

#define CHECK_AND_RETURN_WITHCONFIG(s,n)                                \
    /* Configured model. I need to extract the config from the string. */ \
    /* The string format is NAME_cfg1=var1_cfg2=var2... */              \
    {                                                                   \
        const int len_s = strlen(#s);                                   \
        if( 0 == strncmp( name, #s, len_s ) )                           \
        {                                                               \
            if(name[len_s] == '\0')                                     \
            {                                                           \
                *lensmodel = (mrcal_lensmodel_t){.type = MRCAL_LENSMODEL_INVALID_MISSINGCONFIG}; \
                return false;                                           \
            }                                                           \
            if(name[len_s] == '_')                                      \
            {                                                           \
                /* found name. Now extract the config */                \
                *lensmodel = (mrcal_lensmodel_t){.type = MRCAL_##s};    \
                mrcal_##s##__config_t* config = &lensmodel->s##__config; \
                                                                        \
                const char* config_str = &name[len_s];                  \
                                                                        \
                if(s##__scan_model_config(config, config_str))          \
                    return true;                                        \
                                                                        \
                *lensmodel = (mrcal_lensmodel_t){.type = MRCAL_LENSMODEL_INVALID_BADCONFIG}; \
                return false;                                           \
            }                                                           \
        }                                                               \
    }

    if(name == NULL)
    {
        *lensmodel = (mrcal_lensmodel_t){.type = MRCAL_LENSMODEL_INVALID_TYPE};
        return false;
    }

    MRCAL_LENSMODEL_NOCONFIG_LIST(                   CHECK_AND_RETURN_NOCONFIG );
    MRCAL_LENSMODEL_WITHCONFIG_STATIC_NPARAMS_LIST(  CHECK_AND_RETURN_WITHCONFIG );
    MRCAL_LENSMODEL_WITHCONFIG_DYNAMIC_NPARAMS_LIST( CHECK_AND_RETURN_WITHCONFIG );

    *lensmodel = (mrcal_lensmodel_t){.type = MRCAL_LENSMODEL_INVALID_TYPE};
    return false;
#undef CHECK_AND_RETURN_NOCONFIG
#undef CHECK_AND_RETURN_WITHCONFIG
}

// parses the model name only. The configuration is ignored. Even if it's
// missing or unparseable. Unknown model names return
// MRCAL_LENSMODEL_INVALID_TYPE
mrcal_lensmodel_type_t mrcal_lensmodel_type_from_name( const char* name )
{
#define CHECK_AND_RETURN_NOCONFIG(s,n)                                  \
    if( 0 == strcmp( name, #s) ) return MRCAL_##s;

#define CHECK_AND_RETURN_WITHCONFIG(s,n)                                \
    /* Configured model. If the name is followed by _ or nothing, I */  \
    /* accept this model */                                             \
    {                                                                   \
        const int len_s = strlen(#s);                                   \
        if( 0 == strncmp( name, #s, len_s) &&                           \
            ( name[len_s] == '\0' ||                                    \
              name[len_s] == '_' ) )                                    \
            return MRCAL_##s;                                           \
    }

    if(name == NULL)
        return MRCAL_LENSMODEL_INVALID_TYPE;

    MRCAL_LENSMODEL_NOCONFIG_LIST(                   CHECK_AND_RETURN_NOCONFIG );
    MRCAL_LENSMODEL_WITHCONFIG_STATIC_NPARAMS_LIST(  CHECK_AND_RETURN_WITHCONFIG );
    MRCAL_LENSMODEL_WITHCONFIG_DYNAMIC_NPARAMS_LIST( CHECK_AND_RETURN_WITHCONFIG );

    return MRCAL_LENSMODEL_INVALID_TYPE;

#undef CHECK_AND_RETURN_NOCONFIG
#undef CHECK_AND_RETURN_WITHCONFIG
}

mrcal_lensmodel_metadata_t mrcal_lensmodel_metadata( const mrcal_lensmodel_t* lensmodel )
{
    switch(lensmodel->type)
    {
    case MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC:
    case MRCAL_LENSMODEL_STEREOGRAPHIC:
    case MRCAL_LENSMODEL_LONLAT:
    case MRCAL_LENSMODEL_LATLON:
        return (mrcal_lensmodel_metadata_t) { .has_core                  = true,
                                              .can_project_behind_camera = true,
                                              .has_gradients             = true,
                                              .noncentral                = false};
    case MRCAL_LENSMODEL_PINHOLE:
    case MRCAL_LENSMODEL_OPENCV4:
    case MRCAL_LENSMODEL_OPENCV5:
    case MRCAL_LENSMODEL_OPENCV8:
    case MRCAL_LENSMODEL_OPENCV12:
    case MRCAL_LENSMODEL_CAHVOR:
        return (mrcal_lensmodel_metadata_t) { .has_core                  = true,
                                              .can_project_behind_camera = false,
                                              .has_gradients             = true,
                                              .noncentral                = false };

    case MRCAL_LENSMODEL_CAHVORE:
        return (mrcal_lensmodel_metadata_t) { .has_core                  = true,
                                              .can_project_behind_camera = false,
                                              .has_gradients             = true,
                                              .noncentral                = true };

    default: ;
    }
    MSG("Unknown lens model %d. Barfing out", lensmodel->type);
    assert(0);
}

static
bool modelHasCore_fxfycxcy( const mrcal_lensmodel_t* lensmodel )
{
    mrcal_lensmodel_metadata_t meta = mrcal_lensmodel_metadata(lensmodel);
    return meta.has_core;
}
static
bool model_supports_projection_behind_camera( const mrcal_lensmodel_t* lensmodel )
{
    mrcal_lensmodel_metadata_t meta = mrcal_lensmodel_metadata(lensmodel);
    return meta.can_project_behind_camera;
}

static int LENSMODEL_SPLINED_STEREOGRAPHIC__lensmodel_num_params(const mrcal_LENSMODEL_SPLINED_STEREOGRAPHIC__config_t* config)
{
    return
        // I have two surfaces: one for x and another for y
        (int)config->Nx * (int)config->Ny * 2 +

        // and I have a core
        4;
}
int mrcal_lensmodel_num_params(const mrcal_lensmodel_t* lensmodel)
{
#define CHECK_CONFIG_NPARAM_NOCONFIG(s,n) \
    _Static_assert(n >= 0, "no-config implies known-at-compile-time param count");

    switch(lensmodel->type)
    {
#define CASE_NUM_STATIC(s,n)                                          \
        case MRCAL_##s: return n;

#define CASE_NUM_DYNAMIC(s,n)                                         \
        case MRCAL_##s: return s##__lensmodel_num_params(&lensmodel->s##__config);

        MRCAL_LENSMODEL_NOCONFIG_LIST(                   CASE_NUM_STATIC )
        MRCAL_LENSMODEL_WITHCONFIG_STATIC_NPARAMS_LIST(  CASE_NUM_STATIC )
        MRCAL_LENSMODEL_WITHCONFIG_DYNAMIC_NPARAMS_LIST( CASE_NUM_DYNAMIC )

    default: ;
    }
    return -1;

#undef CASE_NUM_NOCONFIG
#undef CASE_NUM_WITHCONFIG
}

static
int get_num_distortions_optimization_params(mrcal_problem_selections_t problem_selections,
                                            const mrcal_lensmodel_t* lensmodel)
{
    if( !problem_selections.do_optimize_intrinsics_distortions )
        return 0;

    int N = mrcal_lensmodel_num_params(lensmodel);
    if(modelHasCore_fxfycxcy(lensmodel))
        N -= 4; // ignoring fx,fy,cx,cy
    return N;
}

int mrcal_num_intrinsics_optimization_params(mrcal_problem_selections_t problem_selections,
                                             const mrcal_lensmodel_t* lensmodel)
{
    int N = get_num_distortions_optimization_params(problem_selections, lensmodel);

    if( problem_selections.do_optimize_intrinsics_core &&
        modelHasCore_fxfycxcy(lensmodel) )
        N += 4; // fx,fy,cx,cy
    return N;
}

int mrcal_num_states(int Ncameras_intrinsics, int Ncameras_extrinsics,
                     int Nframes,
                     int Npoints, int Npoints_fixed, int Nobservations_board,
                     mrcal_problem_selections_t problem_selections,
                     const mrcal_lensmodel_t* lensmodel)
{
    return
        mrcal_num_states_intrinsics(Ncameras_intrinsics,
                                    problem_selections,
                                    lensmodel) +
        mrcal_num_states_extrinsics(Ncameras_extrinsics,
                                    problem_selections) +
        mrcal_num_states_frames(Nframes,
                                problem_selections) +
        mrcal_num_states_points(Npoints, Npoints_fixed,
                                problem_selections) +
        mrcal_num_states_calobject_warp( problem_selections,
                                         Nobservations_board);
}

static int num_regularization_terms_percamera(mrcal_problem_selections_t problem_selections,
                                              const mrcal_lensmodel_t* lensmodel)
{
    if(!problem_selections.do_apply_regularization)
        return 0;

    // distortions
    int N = get_num_distortions_optimization_params(problem_selections, lensmodel);
    // optical center
    if(problem_selections.do_optimize_intrinsics_core)
        N += 2;
    return N;
}

int mrcal_measurement_index_boards(int i_observation_board,
                                   int Nobservations_board,
                                   int Nobservations_point,
                                   int calibration_object_width_n,
                                   int calibration_object_height_n)
{
    if(Nobservations_board <= 0)
        return -1;

    // *2 because I have separate x and y measurements
    return
        0 +
        mrcal_num_measurements_boards(i_observation_board,
                                      calibration_object_width_n,
                                      calibration_object_height_n);
}

int mrcal_num_measurements_boards(int Nobservations_board,
                                  int calibration_object_width_n,
                                  int calibration_object_height_n)
{
    if(Nobservations_board <= 0)
        return 0;

    // *2 because I have separate x and y measurements
    return
        Nobservations_board *
        calibration_object_width_n*calibration_object_height_n *
        2;


    return mrcal_measurement_index_boards( Nobservations_board,
                                           0,0,
                                           calibration_object_width_n,
                                           calibration_object_height_n);
}

int mrcal_measurement_index_points(int i_observation_point,
                                   int Nobservations_board,
                                   int Nobservations_point,
                                   int calibration_object_width_n,
                                   int calibration_object_height_n)
{
    if(Nobservations_point <= 0)
        return -1;

    // 2: x,y measurements
    return
        mrcal_num_measurements_boards(Nobservations_board,
                                      calibration_object_width_n,
                                      calibration_object_height_n) +
        i_observation_point * 2;
}

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: need known-range points, at-infinity points"
#endif

int mrcal_num_measurements_points(int Nobservations_point)
{
    // 2: x,y measurements
    return Nobservations_point * 2;
}

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: Add a test for mrcal_measurement_index_points_triangulated()"
#endif
int mrcal_measurement_index_points_triangulated(int i_point_triangulated,
                                                int Nobservations_board,
                                                int Nobservations_point,

                                                // May be NULL if we don't have any of these
                                                const mrcal_observation_point_triangulated_t* observations_point_triangulated,
                                                int Nobservations_point_triangulated,

                                                int calibration_object_width_n,
                                                int calibration_object_height_n)
{
    if(observations_point_triangulated == NULL ||
       Nobservations_point_triangulated <= 0)
        return -1;

    return
        mrcal_num_measurements_boards(Nobservations_board,
                                      calibration_object_width_n,
                                      calibration_object_height_n) +
        mrcal_num_measurements_points(Nobservations_point) +
        mrcal_num_measurements_points_triangulated_initial_Npoints(observations_point_triangulated,
                                                                   Nobservations_point_triangulated,
                                                                   i_point_triangulated);
}

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: python-wrap this function"
#endif
int mrcal_num_measurements_points_triangulated_initial_Npoints(// May be NULL if we don't have any of these
                                                               const mrcal_observation_point_triangulated_t* observations_point_triangulated,
                                                               int Nobservations_point_triangulated,

                                                               // Only consider the leading Npoints. If Npoints < 0: take ALL the points
                                                               int Npoints)
{
    if(observations_point_triangulated == NULL ||
       Nobservations_point_triangulated <= 0)
        return 0;

    // Similar loop as in _mrcal_num_j_nonzero(). If the data layout changes,
    // update this and that

    int Nmeas        = 0;
    int ipoint       = 0;
    int iobservation = 0;

    while( iobservation < Nobservations_point_triangulated &&
           (Npoints < 0 || ipoint < Npoints))
    {
        int Nset = 1;
        while(!observations_point_triangulated[iobservation].last_in_set)
        {
            Nmeas += Nset++;
            iobservation++;
        }

        ipoint++;
        iobservation++;
    }

    return Nmeas;
}

int mrcal_num_measurements_points_triangulated(// May be NULL if we don't have any of these
                                               const mrcal_observation_point_triangulated_t* observations_point_triangulated,
                                               int Nobservations_point_triangulated)
{
    return
        mrcal_num_measurements_points_triangulated_initial_Npoints( observations_point_triangulated,
                                                                    Nobservations_point_triangulated,
                                                                    -1 );
}

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: python-wrap this function"
#endif
bool mrcal_decode_observation_indices_points_triangulated(
    // output
    int* iobservation0,
    int* iobservation1,
    int* iobservation_point0,
    int* Nobservations_this_point,
    int* Nmeasurements_this_point,
    int* ipoint,

    // input
    const int imeasurement,

    const mrcal_observation_point_triangulated_t* observations_point_triangulated,
    int Nobservations_point_triangulated)
{
    if(observations_point_triangulated == NULL ||
       Nobservations_point_triangulated <= 0)
        return false;

    // Similar loop as in
    // mrcal_num_measurements_points_triangulated_initial_Npoints(). If the data
    // layout changes, update this and that

    int Nmeas        = 0;
    int iobservation = 0;
    *ipoint          = 0;

    while( iobservation < Nobservations_point_triangulated )
    {
        int Nset = 1;
        while(!observations_point_triangulated[iobservation].last_in_set)
        {
            Nset++;
            iobservation++;
        }

        // This point has Nset observations. Each pair of observations produces
        // a measurement, so:
        const int Nmeas_thispoint = Nset*(Nset-1) / 2;

        // Done with this point. It is described by Nmeas_thispoint
        // measurements, with the first one at Nmeas. The last observation is at
        // iobservation

        if(imeasurement < Nmeas+Nmeas_thispoint)
        {
            // The measurement I care about observes THIS point. I find the
            // specific observations.
            //
            // I simplify the notation: "m" is "imeasurement", "o" is
            // "iobservation".

            const int No = Nset;
            const int Nm = Nmeas_thispoint;
            const int m  = imeasurement - Nmeas;

            // Within this point o is in range(No) and m is in range(Nm). A pair
            // (o0,o1) describes one measurement. Both o0 and o1 are in
            // range(No) and o1>o0. A sample table of measurement indices m for
            // No = 5:
            //
            //          o1
            //       0 1 2 3 4
            //       ---------
            //     0|  0 1 2 3
            // o0  1|    4 5 6
            //     2|      7 8
            //     3|        9
            //     4|
            //
            // For a given o0, the maximum m =
            //
            //   m_max = (No-1) + (No-2) + (No-3) ... - 1
            //
            // for a total of o0+1 (No...) terms so
            //
            //   m_max = ((No-1) + (No-o0-1))/2 * (o0+1) - 1
            //         = (2*No - 2 - o0)/2 * (o0+1) - 1
            //
            //   m_min = m_max(o0-1) + 1
            //         = (2*No - 2 - o0 + 1)/2 * o0
            //         = (2*No - 1 - o0) * o0 / 2
            //         = (-o0^2 + (2*No - 1)*o0 ) / 2
            //
            // -> (-1/2) o0^2 + (2*No - 1)/2 o0 - m_min = 0
            // -> o0 = (2*No - 1)/2 +- sqrt( (2*No - 1)^2/4 - 2*m_min)
            //       = (2*No - 1)/2 - sqrt( (2*No - 1)^2/4 - 2*m_min)
            //       = (2*No - 1 - sqrt( (2*No - 1)^2 - 8*m_min))/2
            //
            // o0(m) = floor(o0(m))
            //
            // Now that I have o0 I compute
            //
            //   o1 = m - m_min(o0) + o0 + 1
            //      = m - (-o0^2 + (2*No - 1)*o0 ) / 2 + o0 + 1
            //      = m + (o0^2 + (3- 2*No)*o0 + 2) / 2
            //      = m + o0*(o0 + 3 - 2*No)/2 + 1
            const double x = 2.*(double)No - 1.;
            *iobservation0 = (int)((x - sqrt( x*x - 8.*(double)m))/2.);
            *iobservation1 = m + (*iobservation0)*((*iobservation0) + 3 - 2*No)/2 + 1;

            // Reference the observations from the first
            *iobservation_point0 = iobservation-Nset+1;
            *iobservation0 += *iobservation_point0;
            *iobservation1 += *iobservation_point0;

            *Nobservations_this_point = No;
            *Nmeasurements_this_point = Nm;

            return true;
        }

        Nmeas += Nmeas_thispoint;

        iobservation++;
        (*ipoint)++;
    }

    return false;
}


int mrcal_measurement_index_regularization(
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: this argument order is weird. Put then triangulated stuff at the end?"
#endif
                                           // May be NULL if we don't have any of these
                                           const mrcal_observation_point_triangulated_t* observations_point_triangulated,
                                           int Nobservations_point_triangulated,

                                           int calibration_object_width_n,
                                           int calibration_object_height_n,
                                           int Ncameras_intrinsics, int Ncameras_extrinsics,
                                           int Nframes,
                                           int Npoints, int Npoints_fixed, int Nobservations_board, int Nobservations_point,
                                           mrcal_problem_selections_t problem_selections,
                                           const mrcal_lensmodel_t* lensmodel)
{

    if(mrcal_num_measurements_regularization(Ncameras_intrinsics, Ncameras_extrinsics,
                                             Nframes,
                                             Npoints, Npoints_fixed, Nobservations_board,
                                             problem_selections,
                                             lensmodel) <= 0)
        return -1;

    return
        mrcal_num_measurements_boards(Nobservations_board,
                                      calibration_object_width_n,
                                      calibration_object_height_n) +
        mrcal_num_measurements_points(Nobservations_point) +
        mrcal_num_measurements_points_triangulated(observations_point_triangulated,
                                                   Nobservations_point_triangulated);
}

int mrcal_num_measurements_regularization(int Ncameras_intrinsics, int Ncameras_extrinsics,
                                          int Nframes,
                                          int Npoints, int Npoints_fixed, int Nobservations_board,
                                          mrcal_problem_selections_t problem_selections,
                                          const mrcal_lensmodel_t* lensmodel)
{
    return
        Ncameras_intrinsics *
        num_regularization_terms_percamera(problem_selections, lensmodel) +

        ((problem_selections.do_apply_regularization_unity_cam01 &&
          problem_selections.do_optimize_extrinsics &&
          Ncameras_extrinsics > 0) ? 1 : 0);
}

int mrcal_num_measurements(int Nobservations_board,
                           int Nobservations_point,

                           // May be NULL if we don't have any of these
                           const mrcal_observation_point_triangulated_t* observations_point_triangulated,
                           int Nobservations_point_triangulated,

                           int calibration_object_width_n,
                           int calibration_object_height_n,
                           int Ncameras_intrinsics, int Ncameras_extrinsics,
                           int Nframes,
                           int Npoints, int Npoints_fixed,
                           mrcal_problem_selections_t problem_selections,
                           const mrcal_lensmodel_t* lensmodel)
{
    return
        mrcal_num_measurements_boards( Nobservations_board,
                                       calibration_object_width_n,
                                       calibration_object_height_n) +
        mrcal_num_measurements_points(Nobservations_point) +
        mrcal_num_measurements_points_triangulated(observations_point_triangulated,
                                                   Nobservations_point_triangulated) +
        mrcal_num_measurements_regularization(Ncameras_intrinsics, Ncameras_extrinsics,
                                              Nframes,
                                              Npoints, Npoints_fixed, Nobservations_board,
                                              problem_selections,
                                              lensmodel);
}

static bool has_calobject_warp(mrcal_problem_selections_t problem_selections,
                               int Nobservations_board)
{
    return problem_selections.do_optimize_calobject_warp && Nobservations_board>0;
}

int _mrcal_num_j_nonzero(int Nobservations_board,
                         int Nobservations_point,

                         // May be NULL if we don't have any of these
                         const mrcal_observation_point_triangulated_t* observations_point_triangulated,
                         int Nobservations_point_triangulated,

                         int calibration_object_width_n,
                         int calibration_object_height_n,
                         int Ncameras_intrinsics, int Ncameras_extrinsics,
                         int Nframes,
                         int Npoints, int Npoints_fixed,
                         const mrcal_observation_board_t* observations_board,
                         const mrcal_observation_point_t* observations_point,
                         mrcal_problem_selections_t problem_selections,
                         const mrcal_lensmodel_t* lensmodel)
{
    // each observation depends on all the parameters for THAT frame and for
    // THAT camera. Camera0 doesn't have extrinsics, so I need to loop through
    // all my observations

    // Each projected point has an x and y measurement, and each one depends on
    // some number of the intrinsic parameters. Parametric models are simple:
    // each one depends on ALL of the intrinsics. Splined models are sparse,
    // however, and there's only a partial dependence
    int Nintrinsics_per_measurement;
    if(lensmodel->type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC)
    {
        int run_len =
            lensmodel->LENSMODEL_SPLINED_STEREOGRAPHIC__config.order + 1;
        Nintrinsics_per_measurement =
            (problem_selections.do_optimize_intrinsics_core        ? 4                 : 0)  +
            (problem_selections.do_optimize_intrinsics_distortions ? (run_len*run_len) : 0);
    }
    else
        Nintrinsics_per_measurement =
            mrcal_num_intrinsics_optimization_params(problem_selections, lensmodel);

    // x depends on fx,cx but NOT on fy, cy. And similarly for y.
    if( problem_selections.do_optimize_intrinsics_core &&
        modelHasCore_fxfycxcy(lensmodel) )
        Nintrinsics_per_measurement -= 2;

    int N = Nobservations_board * ( (problem_selections.do_optimize_frames         ? 6 : 0) +
                                    (problem_selections.do_optimize_extrinsics     ? 6 : 0) +
                                    (has_calobject_warp(problem_selections,Nobservations_board) ? MRCAL_NSTATE_CALOBJECT_WARP : 0) +
                                  Nintrinsics_per_measurement );

    // initial estimate counts extrinsics for the reference camera, which need
    // to be subtracted off
    if(problem_selections.do_optimize_extrinsics)
        for(int i=0; i<Nobservations_board; i++)
            if(observations_board[i].icam.extrinsics < 0)
                N -= 6;
    // *2 because I have separate x and y measurements
    N *= 2*calibration_object_width_n*calibration_object_height_n;

    // Now the point observations
    for(int i=0; i<Nobservations_point; i++)
    {
        N += 2*Nintrinsics_per_measurement;
        if( problem_selections.do_optimize_frames &&
            observations_point[i].i_point < Npoints-Npoints_fixed )
            N += 2*3;
        if( problem_selections.do_optimize_extrinsics &&
            observations_point[i].icam.extrinsics >= 0 )
            N += 2*6;
    }

    // And the triangulated point observations
    if(observations_point_triangulated != NULL &&
       Nobservations_point_triangulated > 0)
    {
        // Similar loop as in
        // mrcal_num_measurements_points_triangulated_initial_Npoints(). If the
        // data layout changes, update this and that
        for(int i0=0; i0<Nobservations_point_triangulated; i0++)
        {
            if(observations_point_triangulated[i0].last_in_set)
                continue;

            int Nvars0 = Nintrinsics_per_measurement;
            if( problem_selections.do_optimize_extrinsics &&
                observations_point_triangulated[i0].icam.extrinsics >= 0 )
                Nvars0 += 6;

            int i1 = i0;

            do
            {
                i1++;

                int Nvars1 = Nintrinsics_per_measurement;
                if( problem_selections.do_optimize_extrinsics &&
                    observations_point_triangulated[i1].icam.extrinsics >= 0 )
                    Nvars1 += 6;

                N += Nvars0 + Nvars1;

            } while(!observations_point_triangulated[i1].last_in_set);
        }
    }

    // Regularization
    if(lensmodel->type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC)
    {
        if(problem_selections.do_apply_regularization)
        {
            // Each regularization term depends on
            // - two values for distortions
            // - one value for the center pixel
            N +=
                Ncameras_intrinsics *
                2 *
                num_regularization_terms_percamera(problem_selections,
                                                   lensmodel);
            // I multiplied by 2, so I double-counted the center pixel
            // contributions. Subtract those off
            if(problem_selections.do_optimize_intrinsics_core)
                N -= Ncameras_intrinsics*2;
        }
    }
    else
        N +=
            Ncameras_intrinsics *
            num_regularization_terms_percamera(problem_selections,
                                               lensmodel);

    if(problem_selections.do_apply_regularization_unity_cam01 &&
       problem_selections.do_optimize_extrinsics &&
       Ncameras_extrinsics > 0)
    {
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: I assume that camera0 is at the reference"
#endif
        N += 3; // translation only
    }

    return N;
}

// Used in the spline-based projection function.
//
// See bsplines.py for the derivation of the spline expressions and for
// justification of the 2D scheme
//
// Here we sample two interpolated surfaces at once: one each for the x and y
// focal-length scales
//
// The sampling function assumes evenly spaced knots.
// a,b,c,d are sequential control points
// x is in [0,1] between b and c. Function looks like this:
//   double A = fA(x);
//   double B = fB(x);
//   double C = fC(x);
//   double D = fD(x);
//   return A*a + B*b + C*c + D*d;
// I need to sample many such 1D segments, so I compute A,B,C,D separately,
// and apply them together
static
void get_sample_coeffs__cubic(double* ABCD, double* ABCDgrad, double x)
{
    double x2 = x*x;
    double x3 = x2*x;
    ABCD[0] =  (-x3 + 3*x2 - 3*x + 1)/6;
    ABCD[1] = (3 * x3/2 - 3*x2 + 2)/3;
    ABCD[2] = (-3 * x3 + 3*x2 + 3*x + 1)/6;
    ABCD[3] = x3 / 6;

    ABCDgrad[0] =  -x2/2 + x - 1./2.;
    ABCDgrad[1] = 3*x2/2 - 2*x;
    ABCDgrad[2] = -3*x2/2 + x + 1./2.;
    ABCDgrad[3] = x2 / 2;
}
static
void interp__cubic(double* out, const double* ABCDx, const double* ABCDy,
                   // control points
                   const double* c,
                   int stridey)
{
    double cinterp[4][2];
    const int stridex = 2;
    for(int iy=0; iy<4; iy++)
        for(int k=0;k<2;k++)
            cinterp[iy][k] =
                ABCDx[0] * c[iy*stridey + 0*stridex + k] +
                ABCDx[1] * c[iy*stridey + 1*stridex + k] +
                ABCDx[2] * c[iy*stridey + 2*stridex + k] +
                ABCDx[3] * c[iy*stridey + 3*stridex + k];
    for(int k=0;k<2;k++)
        out[k] =
            ABCDy[0] * cinterp[0][k] +
            ABCDy[1] * cinterp[1][k] +
            ABCDy[2] * cinterp[2][k] +
            ABCDy[3] * cinterp[3][k];
}
static
void sample_bspline_surface_cubic(double* out,
                                  double* dout_dx,
                                  double* dout_dy,
                                  double* ABCDx_ABCDy,

                                  double x, double y,
                                  // control points
                                  const double* c,
                                  int stridey

                                  // stridex is 2: the control points from the
                                  // two surfaces are next to each other. Better
                                  // cache locality maybe
                                  )
{
    double* ABCDx = &ABCDx_ABCDy[0];
    double* ABCDy = &ABCDx_ABCDy[4];

    // 4 samples along one dimension, and then one sample along the other
    // dimension, using the 4 samples as the control points. Order doesn't
    // matter. See bsplines.py
    //
    // I do this twice: one for each focal length surface
    double ABCDgradx[4];
    double ABCDgrady[4];
    get_sample_coeffs__cubic(ABCDx, ABCDgradx, x);
    get_sample_coeffs__cubic(ABCDy, ABCDgrady, y);

    // the intrinsics gradient is flatten(ABCDx[0..3] * ABCDy[0..3]) for both x
    // and y. By returning ABCD[xy] and not the cartesian products, I make
    // smaller temporary data arrays
    interp__cubic(out,     ABCDx,     ABCDy,     c, stridey);
    interp__cubic(dout_dx, ABCDgradx, ABCDy,     c, stridey);
    interp__cubic(dout_dy, ABCDx,     ABCDgrady, c, stridey);
}
// The sampling function assumes evenly spaced knots.
// a,b,c are sequential control points
// x is in [-1/2,1/2] around b. Function looks like this:
//   double A = fA(x);
//   double B = fB(x);
//   double C = fC(x);
//   return A*a + B*b + C*c;
// I need to sample many such 1D segments, so I compute A,B,C separately,
// and apply them together
static
void get_sample_coeffs__quadratic(double* ABC, double* ABCgrad, double x)
{
    double x2 = x*x;
    ABC[0] = (4*x2 - 4*x + 1)/8;
    ABC[1] = (3 - 4*x2)/4;
    ABC[2] = (4*x2 + 4*x + 1)/8;

    ABCgrad[0] = x - 1./2.;
    ABCgrad[1] = -2.*x;
    ABCgrad[2] = x + 1./2.;
}
static
void interp__quadratic(double* out, const double* ABCx, const double* ABCy,
                       // control points
                       const double* c,
                       int stridey)
{
    double cinterp[3][2];
    const int stridex = 2;
    for(int iy=0; iy<3; iy++)
        for(int k=0;k<2;k++)
            cinterp[iy][k] =
                ABCx[0] * c[iy*stridey + 0*stridex + k] +
                ABCx[1] * c[iy*stridey + 1*stridex + k] +
                ABCx[2] * c[iy*stridey + 2*stridex + k];
    for(int k=0;k<2;k++)
        out[k] =
            ABCy[0] * cinterp[0][k] +
            ABCy[1] * cinterp[1][k] +
            ABCy[2] * cinterp[2][k];
}
static
void sample_bspline_surface_quadratic(double* out,
                                      double* dout_dx,
                                      double* dout_dy,
                                      double* ABCx_ABCy,

                                      double x, double y,
                                      // control points
                                      const double* c,
                                      int stridey

                                      // stridex is 2: the control points from the
                                      // two surfaces are next to each other. Better
                                      // cache locality maybe
                                      )
{
    double* ABCx = &ABCx_ABCy[0];
    double* ABCy = &ABCx_ABCy[3];

    // 3 samples along one dimension, and then one sample along the other
    // dimension, using the 3 samples as the control points. Order doesn't
    // matter. See bsplines.py
    //
    // I do this twice: one for each focal length surface
    double ABCgradx[3];
    double ABCgrady[3];
    get_sample_coeffs__quadratic(ABCx, ABCgradx, x);
    get_sample_coeffs__quadratic(ABCy, ABCgrady, y);

    // the intrinsics gradient is flatten(ABCx[0..3] * ABCy[0..3]) for both x
    // and y. By returning ABC[xy] and not the cartesian products, I make
    // smaller temporary data arrays
    interp__quadratic(out,     ABCx,     ABCy,     c, stridey);
    interp__quadratic(dout_dx, ABCgradx, ABCy,     c, stridey);
    interp__quadratic(dout_dy, ABCx,     ABCgrady, c, stridey);
}

typedef struct
{
    double _d_rj_rf[3*3];
    double _d_rj_rc[3*3];
    double _d_tj_tf[3*3];
    double _d_tj_rc[3*3];

    // _d_tj_tc is always identity
    // _d_tj_rf is always 0
    // _d_rj_tf is always 0
    // _d_rj_tc is always 0

} geometric_gradients_t;

static
void project_cahvor( // outputs
                     mrcal_point2_t* restrict q,
                     mrcal_point2_t* restrict dq_dfxy,
                     double*         restrict dq_dintrinsics_nocore,
                     mrcal_point3_t* restrict dq_drcamera,
                     mrcal_point3_t* restrict dq_dtcamera,
                     mrcal_point3_t* restrict dq_drframe,
                     mrcal_point3_t* restrict dq_dtframe,

                     // inputs
                     const mrcal_point3_t* restrict p,
                     const mrcal_point3_t* restrict dp_drc,
                     const mrcal_point3_t* restrict dp_dtc,
                     const mrcal_point3_t* restrict dp_drf,
                     const mrcal_point3_t* restrict dp_dtf,

                     const double* restrict intrinsics,
                     bool camera_at_identity,
                     const mrcal_lensmodel_t* lensmodel)
{
    int NdistortionParams = mrcal_lensmodel_num_params(lensmodel) - 4;

    // I perturb p, and then apply the focal length, center pixel stuff
    // normally
    mrcal_point3_t p_distorted;

    bool need_any_extrinsics_gradients =
        ( dq_drcamera != NULL ) ||
        ( dq_dtcamera != NULL ) ||
        ( dq_drframe  != NULL ) ||
        ( dq_dtframe  != NULL );

    // distortion parameter layout:
    //   alpha
    //   beta
    //   r0
    //   r1
    //   r2
    double alpha = intrinsics[4 + 0];
    double beta  = intrinsics[4 + 1];
    double r0    = intrinsics[4 + 2];
    double r1    = intrinsics[4 + 3];
    double r2    = intrinsics[4 + 4];

    double s_al = sin(alpha);
    double c_al = cos(alpha);
    double s_be = sin(beta);
    double c_be = cos(beta);

    // I parametrize the optical axis such that
    // - o(alpha=0, beta=0) = (0,0,1) i.e. the optical axis is at the center
    //   if both parameters are 0
    // - The gradients are cartesian. I.e. do/dalpha and do/dbeta are both
    //   NOT 0 at (alpha=0,beta=0). This would happen at the poles (gimbal
    //   lock), and that would make my solver unhappy
    double o     []         = {  s_al*c_be, s_be,  c_al*c_be };
    double do_dalpha[]      = {  c_al*c_be,    0, -s_al*c_be };
    double do_dbeta[]       = { -s_al*s_be, c_be, -c_al*s_be };

    double norm2p        = norm2_vec(3, p->xyz);
    double omega         = dot_vec(3, p->xyz, o);
    double domega_dalpha = dot_vec(3, p->xyz, do_dalpha);
    double domega_dbeta  = dot_vec(3, p->xyz, do_dbeta);

    double omega_recip = 1.0 / omega;
    double tau         = norm2p * omega_recip*omega_recip - 1.0;
    double s__dtau_dalphabeta__domega_dalphabeta = -2.0*norm2p * omega_recip*omega_recip*omega_recip;
    double dmu_dtau = r1 + 2.0*tau*r2;
    double dmu_dxyz[3];
    for(int i=0; i<3; i++)
        dmu_dxyz[i] = dmu_dtau *
            (2.0 * p->xyz[i] * omega_recip*omega_recip + s__dtau_dalphabeta__domega_dalphabeta * o[i]);
    double mu = r0 + tau*r1 + tau*tau*r2;
    double s__dmu_dalphabeta__domega_dalphabeta = dmu_dtau * s__dtau_dalphabeta__domega_dalphabeta;

    double  dpdistorted_dpcam[3*3] = {};
    std::vector<double>  dpdistorted_ddistortion(3*NdistortionParams);

    for(int i=0; i<3; i++)
    {
        double dmu_ddist[5] = { s__dmu_dalphabeta__domega_dalphabeta * domega_dalpha,
                                s__dmu_dalphabeta__domega_dalphabeta * domega_dbeta,
                                1.0,
                                tau,
                                tau * tau };

        dpdistorted_ddistortion[i*NdistortionParams + 0] = p->xyz[i] * dmu_ddist[0];
        dpdistorted_ddistortion[i*NdistortionParams + 1] = p->xyz[i] * dmu_ddist[1];
        dpdistorted_ddistortion[i*NdistortionParams + 2] = p->xyz[i] * dmu_ddist[2];
        dpdistorted_ddistortion[i*NdistortionParams + 3] = p->xyz[i] * dmu_ddist[3];
        dpdistorted_ddistortion[i*NdistortionParams + 4] = p->xyz[i] * dmu_ddist[4];

        dpdistorted_ddistortion[i*NdistortionParams + 0] -= dmu_ddist[0] * omega*o[i];
        dpdistorted_ddistortion[i*NdistortionParams + 1] -= dmu_ddist[1] * omega*o[i];
        dpdistorted_ddistortion[i*NdistortionParams + 2] -= dmu_ddist[2] * omega*o[i];
        dpdistorted_ddistortion[i*NdistortionParams + 3] -= dmu_ddist[3] * omega*o[i];
        dpdistorted_ddistortion[i*NdistortionParams + 4] -= dmu_ddist[4] * omega*o[i];

        dpdistorted_ddistortion[i*NdistortionParams + 0] -= mu * domega_dalpha*o[i];
        dpdistorted_ddistortion[i*NdistortionParams + 1] -= mu * domega_dbeta *o[i];

        dpdistorted_ddistortion[i*NdistortionParams + 0] -= mu * omega * do_dalpha[i];
        dpdistorted_ddistortion[i*NdistortionParams + 1] -= mu * omega * do_dbeta [i];

        dpdistorted_dpcam[3*i + i] = mu+1.0;
        for(int j=0; j<3; j++)
        {
            dpdistorted_dpcam[3*i + j] += (p->xyz[i] - omega*o[i]) * dmu_dxyz[j];
            dpdistorted_dpcam[3*i + j] -= mu*o[i]*o[j];
        }

        p_distorted.xyz[i] = p->xyz[i] + mu * (p->xyz[i] - omega*o[i]);
    }

    // q = fxy pxy/pz + cxy
    // dqx/dp = d( fx px/pz + cx ) = fx/pz^2 (pz [1 0 0] - px [0 0 1])
    // dqy/dp = d( fy py/pz + cy ) = fy/pz^2 (pz [0 1 0] - py [0 0 1])
    const double fx = intrinsics[0];
    const double fy = intrinsics[1];
    const double cx = intrinsics[2];
    const double cy = intrinsics[3];
    double pz_recip = 1. / p_distorted.z;
    q->x = p_distorted.x*pz_recip * fx + cx;
    q->y = p_distorted.y*pz_recip * fy + cy;

    if(need_any_extrinsics_gradients)
    {
        double dq_dp[2][3] =
            { { fx * pz_recip,             0, -fx*p_distorted.x*pz_recip*pz_recip},
              { 0,             fy * pz_recip, -fy*p_distorted.y*pz_recip*pz_recip} };
        // This is for the DISTORTED p.
        // dq/deee = dq/dpdistorted dpdistorted/dpundistorted dpundistorted/deee

        double dq_dpundistorted[6];
        mul_genN3_gen33_vout(2, (double*)dq_dp, dpdistorted_dpcam, dq_dpundistorted);

        // dq/deee = dq/dp dp/deee
        if(camera_at_identity)
        {
            if( dq_drcamera != NULL ) memset(dq_drcamera, 0, 6*sizeof(double));
            if( dq_dtcamera != NULL ) memset(dq_dtcamera, 0, 6*sizeof(double));
            if( dq_drframe  != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dpundistorted, (double*)dp_drf, (double*)dq_drframe);
            if( dq_dtframe  != NULL ) memcpy(dq_dtframe, dq_dpundistorted, 6*sizeof(double));
        }
        else
        {
            if( dq_drcamera != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dpundistorted, (double*)dp_drc, (double*)dq_drcamera);
            if( dq_dtcamera != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dpundistorted, (double*)dp_dtc, (double*)dq_dtcamera);
            if( dq_drframe  != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dpundistorted, (double*)dp_drf, (double*)dq_drframe );
            if( dq_dtframe  != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dpundistorted, (double*)dp_dtf, (double*)dq_dtframe );
        }
    }

    if( dq_dintrinsics_nocore != NULL )
    {
        for(int i=0; i<NdistortionParams; i++)
        {
            const double dx = dpdistorted_ddistortion[i + 0*NdistortionParams];
            const double dy = dpdistorted_ddistortion[i + 1*NdistortionParams];
            const double dz = dpdistorted_ddistortion[i + 2*NdistortionParams];
            dq_dintrinsics_nocore[0*NdistortionParams + i] = fx * pz_recip * (dx - p_distorted.x*pz_recip*dz);
            dq_dintrinsics_nocore[1*NdistortionParams + i] = fy * pz_recip * (dy - p_distorted.y*pz_recip*dz);
        }
    }

    if( dq_dfxy )
    {
        // I have the projection, and I now need to propagate the gradients
        // xy = fxy * distort(xy)/distort(z) + cxy
        dq_dfxy->x = (q->x - cx)/fx; // dqx/dfx
        dq_dfxy->y = (q->y - cy)/fy; // dqy/dfy
    }
}

static
bool project_cahvore( // outputs
                      mrcal_point2_t* restrict q,
                      mrcal_point2_t* restrict dq_dfxy,
                      double*         restrict dq_dintrinsics_nocore,
                      mrcal_point3_t* restrict dq_drcamera,
                      mrcal_point3_t* restrict dq_dtcamera,
                      mrcal_point3_t* restrict dq_drframe,
                      mrcal_point3_t* restrict dq_dtframe,

                      // inputs
                      const mrcal_point3_t* restrict p,
                      const mrcal_point3_t* restrict dp_drc,
                      const mrcal_point3_t* restrict dp_dtc,
                      const mrcal_point3_t* restrict dp_drf,
                      const mrcal_point3_t* restrict dp_dtf,

                      const double* restrict intrinsics,
                      bool camera_at_identity,
                      const mrcal_lensmodel_t* lensmodel)
{
    int NdistortionParams = mrcal_lensmodel_num_params(lensmodel) - 4; // This is 8: alpha,beta,R,E

    mrcal_point3_t p_distorted;
    double dpdistorted_ddistortion[8*3]; // 8 intrinsics parameters, nvec(p)=3
    double dpdistorted_dpcam[3*3];

    bool need_any_extrinsics_gradients =
        ( dq_drcamera != NULL ) ||
        ( dq_dtcamera != NULL ) ||
        ( dq_drframe  != NULL ) ||
        ( dq_dtframe  != NULL );

    if(!project_cahvore_internals( // outputs
                                   &p_distorted,
                                   dq_dintrinsics_nocore         ? dpdistorted_ddistortion : NULL,
                                   need_any_extrinsics_gradients ? dpdistorted_dpcam       : NULL,

                                   // inputs
                                   p,
                                   &intrinsics[4],
                                   lensmodel->LENSMODEL_CAHVORE__config.linearity))
        return false;

    ////////////// exactly like in project_cahvor() above. Consolidate.

    // q = fxy pxy/pz + cxy
    // dqx/dp = d( fx px/pz + cx ) = fx/pz^2 (pz [1 0 0] - px [0 0 1])
    // dqy/dp = d( fy py/pz + cy ) = fy/pz^2 (pz [0 1 0] - py [0 0 1])
    const double fx = intrinsics[0];
    const double fy = intrinsics[1];
    const double cx = intrinsics[2];
    const double cy = intrinsics[3];
    double pz_recip = 1. / p_distorted.z;
    q->x = p_distorted.x*pz_recip * fx + cx;
    q->y = p_distorted.y*pz_recip * fy + cy;

    if(need_any_extrinsics_gradients)
    {
        double dq_dp[2][3] =
            { { fx * pz_recip,             0, -fx*p_distorted.x*pz_recip*pz_recip},
              { 0,             fy * pz_recip, -fy*p_distorted.y*pz_recip*pz_recip} };
        // This is for the DISTORTED p.
        // dq/deee = dq/dpdistorted dpdistorted/dpundistorted dpundistorted/deee

        double dq_dpundistorted[6];
        mul_genN3_gen33_vout(2, (double*)dq_dp, dpdistorted_dpcam, dq_dpundistorted);

        // dq/deee = dq/dp dp/deee
        if(camera_at_identity)
        {
            if( dq_drcamera != NULL ) memset(dq_drcamera, 0, 6*sizeof(double));
            if( dq_dtcamera != NULL ) memset(dq_dtcamera, 0, 6*sizeof(double));
            if( dq_drframe  != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dpundistorted, (double*)dp_drf, (double*)dq_drframe);
            if( dq_dtframe  != NULL ) memcpy(dq_dtframe, dq_dpundistorted, 6*sizeof(double));
        }
        else
        {
            if( dq_drcamera != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dpundistorted, (double*)dp_drc, (double*)dq_drcamera);
            if( dq_dtcamera != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dpundistorted, (double*)dp_dtc, (double*)dq_dtcamera);
            if( dq_drframe  != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dpundistorted, (double*)dp_drf, (double*)dq_drframe );
            if( dq_dtframe  != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dpundistorted, (double*)dp_dtf, (double*)dq_dtframe );
        }
    }

    if( dq_dintrinsics_nocore != NULL )
    {
        for(int i=0; i<NdistortionParams; i++)
        {
            const double dx = dpdistorted_ddistortion[i + 0*NdistortionParams];
            const double dy = dpdistorted_ddistortion[i + 1*NdistortionParams];
            const double dz = dpdistorted_ddistortion[i + 2*NdistortionParams];
            dq_dintrinsics_nocore[0*NdistortionParams + i] = fx * pz_recip * (dx - p_distorted.x*pz_recip*dz);
            dq_dintrinsics_nocore[1*NdistortionParams + i] = fy * pz_recip * (dy - p_distorted.y*pz_recip*dz);
        }
    }

    if( dq_dfxy )
    {
        // I have the projection, and I now need to propagate the gradients
        // xy = fxy * distort(xy)/distort(z) + cxy
        dq_dfxy->x = (q->x - cx)/fx; // dqx/dfx
        dq_dfxy->y = (q->y - cy)/fy; // dqy/dfy
    }
    return true;
}

// These are all internals for project(). It was getting unwieldy otherwise
static
void _project_point_parametric( // outputs
                               mrcal_point2_t* q,
                               mrcal_point2_t* dq_dfxy, double* dq_dintrinsics_nocore,
                               mrcal_point3_t* restrict dq_drcamera,
                               mrcal_point3_t* restrict dq_dtcamera,
                               mrcal_point3_t* restrict dq_drframe,
                               mrcal_point3_t* restrict dq_dtframe,

                               // inputs
                               const mrcal_point3_t* p,
                               const mrcal_point3_t* dp_drc,
                               const mrcal_point3_t* dp_dtc,
                               const mrcal_point3_t* dp_drf,
                               const mrcal_point3_t* dp_dtf,

                               const double* restrict intrinsics,
                               bool camera_at_identity,
                               const mrcal_lensmodel_t* lensmodel)
{
    // u = distort(p, distortions)
    // q = uxy/uz * fxy + cxy
    if(!( lensmodel->type == MRCAL_LENSMODEL_PINHOLE ||
          lensmodel->type == MRCAL_LENSMODEL_STEREOGRAPHIC ||
          lensmodel->type == MRCAL_LENSMODEL_LONLAT ||
          lensmodel->type == MRCAL_LENSMODEL_LATLON ||
          MRCAL_LENSMODEL_IS_OPENCV(lensmodel->type) ))
    {
        MSG("Unhandled lens model: %d (%s)",
            lensmodel->type, mrcal_lensmodel_name_unconfigured(lensmodel));
        assert(0);
    }

    mrcal_point3_t dq_dp[2];
    if( lensmodel->type == MRCAL_LENSMODEL_PINHOLE )
        mrcal_project_pinhole(q, dq_dp,
                              p, 1, intrinsics);
    else if(lensmodel->type == MRCAL_LENSMODEL_STEREOGRAPHIC)
        mrcal_project_stereographic(q, dq_dp,
                                    p, 1, intrinsics);
    else if(lensmodel->type == MRCAL_LENSMODEL_LONLAT)
        mrcal_project_lonlat(q, dq_dp,
                             p, 1, intrinsics);
    else if(lensmodel->type == MRCAL_LENSMODEL_LATLON)
        mrcal_project_latlon(q, dq_dp,
                             p, 1, intrinsics);
    else
    {
        int Nintrinsics = mrcal_lensmodel_num_params(lensmodel);
        _mrcal_project_internal_opencv( q, dq_dp,
                                        dq_dintrinsics_nocore,
                                        p, 1, intrinsics, Nintrinsics);
    }

    // dq/deee = dq/dp dp/deee
    if(camera_at_identity)
    {
        if( dq_drcamera != NULL ) memset(dq_drcamera, 0, 6*sizeof(double));
        if( dq_dtcamera != NULL ) memset(dq_dtcamera, 0, 6*sizeof(double));
        if( dq_drframe  != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dp, (double*)dp_drf, (double*)dq_drframe);
        if( dq_dtframe  != NULL ) memcpy(dq_dtframe, (double*)dq_dp, 6*sizeof(double));
    }
    else
    {
        if( dq_drcamera != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dp, (double*)dp_drc, (double*)dq_drcamera);
        if( dq_dtcamera != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dp, (double*)dp_dtc, (double*)dq_dtcamera);
        if( dq_drframe  != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dp, (double*)dp_drf, (double*)dq_drframe );
        if( dq_dtframe  != NULL ) mul_genN3_gen33_vout(2, (double*)dq_dp, (double*)dp_dtf, (double*)dq_dtframe );
    }

    // I have the projection, and I now need to propagate the gradients
    if( dq_dfxy )
    {
        const double fx = intrinsics[0];
        const double fy = intrinsics[1];
        const double cx = intrinsics[2];
        const double cy = intrinsics[3];

        // I have the projection, and I now need to propagate the gradients
        // xy = fxy * distort(xy)/distort(z) + cxy
        dq_dfxy->x = (q->x - cx)/fx; // dqx/dfx
        dq_dfxy->y = (q->y - cy)/fy; // dqy/dfy
    }
}

// Compute a pinhole projection using a constant fxy, cxy
void mrcal_project_pinhole( // output
                            mrcal_point2_t* q,
                            mrcal_point3_t* dq_dv, // May be NULL. Each point
                                                   // gets a block of 2 mrcal_point3_t
                                                   // objects

                            // input
                            const mrcal_point3_t* v,
                            int N,
                            const double* fxycxy)
{
    const double fx = fxycxy[0];
    const double fy = fxycxy[1];
    const double cx = fxycxy[2];
    const double cy = fxycxy[3];

    // q = fxy pxy/pz + cxy
    // dqx/dp = d( fx px/pz + cx ) = fx/pz^2 (pz [1 0 0] - px [0 0 1])
    // dqy/dp = d( fy py/pz + cy ) = fy/pz^2 (pz [0 1 0] - py [0 0 1])
    for(int i=0; i<N; i++)
    {
        double pz_recip = 1. / v[i].z;
        q->x = v[i].x*pz_recip * fx + cx;
        q->y = v[i].y*pz_recip * fy + cy;

        if(dq_dv)
        {
            dq_dv[2*i + 0].x = fx * pz_recip;
            dq_dv[2*i + 0].y = 0;
            dq_dv[2*i + 0].z = -fx*v[i].x*pz_recip*pz_recip;

            dq_dv[2*i + 1].x = 0;
            dq_dv[2*i + 1].y = fy * pz_recip;
            dq_dv[2*i + 1].z = -fy*v[i].y*pz_recip*pz_recip;
        }
    }
}

// Compute a pinhole unprojection using a constant fxy, cxy
void mrcal_unproject_pinhole( // output
                              mrcal_point3_t* v,
                              mrcal_point2_t* dv_dq, // May be NULL. Each point
                                                     // gets a block of 3
                                                     // mrcal_point2_t objects

                              // input
                              const mrcal_point2_t* q,
                              int N,
                              const double* fxycxy)
{
    const double fx = fxycxy[0];
    const double fy = fxycxy[1];
    const double cx = fxycxy[2];
    const double cy = fxycxy[3];

    double fx_recip = 1./fx;
    double fy_recip = 1./fy;
    for(int i=0; i<N; i++)
    {
        v[i].x = (q[i].x - cx) / fx;
        v[i].y = (q[i].y - cy) / fy;
        v[i].z = 1.0;

        if(dv_dq)
        {
            dv_dq[3*i + 0] = (mrcal_point2_t){.x = fx_recip};
            dv_dq[3*i + 1] = (mrcal_point2_t){.y = fy_recip};
            dv_dq[3*i + 2] = (mrcal_point2_t){};
        }
    }
}

// Compute a stereographic projection using a constant fxy, cxy. This is the
// same as the pinhole projection for long lenses, but supports views behind the
// camera
void mrcal_project_stereographic( // output
                                 mrcal_point2_t* q,
                                 mrcal_point3_t* dq_dv, // May be NULL. Each point
                                                        // gets a block of 2 mrcal_point3_t
                                                        // objects

                                 // input
                                 const mrcal_point3_t* v,
                                 int N,
                                 const double* fxycxy)
{
    const double fx = fxycxy[0];
    const double fy = fxycxy[1];
    const double cx = fxycxy[2];
    const double cy = fxycxy[3];

    // stereographic projection:
    //   (from https://en.wikipedia.org/wiki/Fisheye_lens)
    //   u = xy_unit * tan(th/2) * 2
    //
    // th is the angle between the observation and the projection
    // center
    //
    // sin(th)   = mag_xy/mag_xyz
    // cos(th)   = z/mag_xyz
    // tan(th/2) = sin(th) / (1 + cos(th))

    // tan(th/2) = mag_xy/mag_xyz / (1 + z/mag_xyz) =
    //           = mag_xy / (mag_xyz + z)
    // u = xy_unit * tan(th/2) * 2 =
    //   = xy/mag_xy * mag_xy/(mag_xyz + z) * 2 =
    //   = xy / (mag_xyz + z) * 2
    for(int i=0; i<N; i++)
    {
        double mag_xyz = sqrt( v[i].x*v[i].x +
                               v[i].y*v[i].y +
                               v[i].z*v[i].z );
        double scale = 2.0 / (mag_xyz + v[i].z);

        if(dq_dv)
        {
            // this is more or less already derived in _project_point_splined()
            //
            // dqx/dv = fx ( scale dx + x dscale ) =
            //        = fx ( [1 0 0] scale - 2 x / ()^2 * ( [x y z]/(sqrt) + [0 0 1]) )
            //        = fx ( [scale 0 0] - x scale^2/2 * ( [x y z]/mag_xyz + [0 0 1]) )
            // Let A = -scale^2/2
            //     B = A/mag_xyz
            // dqx_dv = fx ( [scale 0 0] - x scale^2/2 * [x y z]/mag_xyz - x scale^2/2 [0 0 1] )
            //        = fx ( [scale 0 0] + B x * [x y z] + x A [0 0 1] )
            double A = -scale*scale / 2.;
            double B = A / mag_xyz;
            dq_dv[2*i + 0] = (mrcal_point3_t){.x = fx * (v[i].x * (B*v[i].x) + scale),
                                              .y = fx * (v[i].x * (B*v[i].y)),
                                              .z = fx * (v[i].x * (B*v[i].z + A))};
            dq_dv[2*i + 1] = (mrcal_point3_t){.x = fy * (v[i].y * (B*v[i].x)),
                                              .y = fy * (v[i].y * (B*v[i].y) + scale),
                                              .z = fy * (v[i].y * (B*v[i].z + A))};
        }
        q[i] = (mrcal_point2_t){.x = v[i].x * scale * fx + cx,
                                .y = v[i].y * scale * fy + cy};
    }
}

// Compute a stereographic unprojection using a constant fxy, cxy
void mrcal_unproject_stereographic( // output
                                   mrcal_point3_t* v,
                                   mrcal_point2_t* dv_dq, // May be NULL. Each point
                                                          // gets a block of 3
                                                          // mrcal_point2_t objects

                                   // input
                                   const mrcal_point2_t* q,
                                   int N,
                                   const double* fxycxy)
{
    const double fx = fxycxy[0];
    const double fy = fxycxy[1];
    const double cx = fxycxy[2];
    const double cy = fxycxy[3];

    // stereographic projection:
    //   (from https://en.wikipedia.org/wiki/Fisheye_lens)
    //   u = xy_unit * tan(th/2) * 2
    //
    // I compute the normalized (focal-length = 1) projection, and
    // use that to look-up the x and y focal length scalings
    //
    // th is the angle between the observation and the projection
    // center
    //
    // sin(th)   = mag_xy/mag_xyz
    // cos(th)   = z/mag_xyz
    // tan(th/2) = sin(th) / (1 + cos(th))
    //
    // tan(th/2) = mag_xy/mag_xyz / (1 + z/mag_xyz) =
    //           = mag_xy / (mag_xyz + z)
    // u = xy_unit * tan(th/2) * 2 =
    //   = xy/mag_xy * mag_xy/(mag_xyz + z) * 2 =
    //   = xy / (mag_xyz + z) * 2
    //
    // How do I compute the inverse?
    //
    // So q = u f + c
    // -> u = (q-c)/f
    // mag(u) = tan(th/2)*2
    //
    // So I can compute th. az comes from the direction of u. This is enough to
    // compute everything. th is in [0,pi].
    //
    //     [ sin(th) cos(az) ]   [ cos(az)   ]
    // v = [ sin(th) sin(az) ] ~ [ sin(az)   ]
    //     [ cos(th)         ]   [ 1/tan(th) ]
    //
    // mag(u) = tan(th/2)*2 -> mag(u)/2 = tan(th/2) ->
    // tan(th) = mag(u) / (1 - (mag(u)/2)^2)
    // 1/tan(th) = (1 - 1/4*mag(u)^2) / mag(u)
    //
    // This has a singularity at u=0 (imager center). But I can scale v to avoid
    // this. So
    //
    //     [ cos(az) mag(u)   ]
    // v = [ sin(az) mag(u)   ]
    //     [ 1 - 1/4*mag(u)^2 ]
    //
    // I can simplify this even more. az = atan2(u.y,u.x). cos(az) = u.x/mag(u) ->
    //
    //     [ u.x              ]
    // v = [ u.y              ]
    //     [ 1 - 1/4 mag(u)^2 ]
    //
    // Test script to confirm that the project/unproject expressions are
    // correct. unproj(proj(v))/v should be a constant
    //
    //     import numpy      as np
    //     import numpysane  as nps
    //     f = 2000
    //     c = 1000
    //     def proj(v):
    //         m = nps.mag(v)
    //         scale = 2.0 / (m + v[..., 2])
    //         u = v[..., :2] * nps.dummy(scale, -1)
    //         return u * f + c
    //     def unproj(q):
    //         u = (q-c)/f
    //         muxy = nps.mag(u[..., :2])
    //         m    = nps.mag(u)
    //         return nps.mv(nps.cat( u[..., 0],
    //                                u[..., 1],
    //                                1 - 1./4.* m*m),
    //                       0, -1)
    //     v = np.array(((1., 2., 3.),
    //                   (3., -2., -4.)))
    //     print( unproj(proj(v)) / v)
    double fx_recip = 1./fx;
    double fy_recip = 1./fy;
    for(int i=0; i<N; i++)
    {
        mrcal_point2_t u = {.x = (q[i].x - cx) * fx_recip,
                            .y = (q[i].y - cy) * fy_recip};

        double norm2u = u.x*u.x + u.y*u.y;
        if(dv_dq)
        {
            dv_dq[3*i + 0] = (mrcal_point2_t){.x = 1.0*fx_recip};
            dv_dq[3*i + 1] = (mrcal_point2_t){.y = 1.0*fy_recip};
            dv_dq[3*i + 2] = (mrcal_point2_t){.x = -u.x/2.0*fx_recip,
                                              .y = -u.y/2.0*fy_recip};
        }
        v[i] = (mrcal_point3_t){ .x = u.x,
                                 .y = u.y,
                                 .z = 1. - 1./4. * norm2u };
    }
}


void mrcal_project_lonlat( // output
                           mrcal_point2_t* q,
                           mrcal_point3_t* dq_dv, // May be NULL. Each point
                                                  // gets a block of 2 mrcal_point3_t
                                                  // objects

                           // input
                           const mrcal_point3_t* v,
                           int N,
                           const double* fxycxy)
{
    const double fx = fxycxy[0];
    const double fy = fxycxy[1];
    const double cx = fxycxy[2];
    const double cy = fxycxy[3];

    // equirectangular projection:
    //   q   = (lon, lat)
    //   lon = arctan2(vx, vz)
    //   lat = arcsin(vy / mag(v))
    //
    // At the optical axis we have vx ~ vy ~ 0 and vz ~1, so
    //   lon ~ vx
    //   lat ~ vy

    // qx ~ arctan( vx/vz ) ->
    // dqx/dv = 1/(1 + (vx/vz)^2) 1/vz^2 ( dvx/dv vz - vx dvz/dv ) =
    //        = [vz 0 -vx] / (vx^2 + vz^2)
    //
    // qy ~ arcsin( vy / mag(v) ) ->
    // dqy/dv = 1 / sqrt( 1 - vy^2/norm2(v) ) 1/norm2(v) (dvy/dv mag(v)  - dmag(v)/dv vy)
    //        = 1 / sqrt( norm2(v) - vy^2 ) 1/mag(v)  ( [0 mag(v)   0] - v/mag(v) vy)
    //        = 1 / sqrt( norm2(v) - vy^2 ) ( [0 1 0] - v/norm2(v) vy)
    //        = 1 / sqrt( vx^2 + vz^2 ) ( [0 1 0] - v/norm2(v) vy)
    for(int i=0; i<N; i++)
    {
        double norm2_xyz_recip = 1. / (v[i].x*v[i].x +
                                       v[i].y*v[i].y +
                                       v[i].z*v[i].z );
        double mag_xyz_recip = sqrt(norm2_xyz_recip);
        double norm2_xz_recip = 1. / (v[i].x*v[i].x +
                                      v[i].z*v[i].z );
        double mag_xz_recip = sqrt(norm2_xz_recip);


        if(dq_dv)
        {
            dq_dv[2*i + 0] = (mrcal_point3_t){.x =  fx*norm2_xz_recip * v[i].z,
                                              .z = -fx*norm2_xz_recip * v[i].x };
            dq_dv[2*i + 1] = (mrcal_point3_t){.x = -fy*mag_xz_recip   * (v[i].y*v[i].x * norm2_xyz_recip),
                                              .y = -fy*mag_xz_recip   * (v[i].y*v[i].y * norm2_xyz_recip - 1.),
                                              .z = -fy*mag_xz_recip   * (v[i].y*v[i].z * norm2_xyz_recip) };
        }
        q[i] = (mrcal_point2_t){.x = atan2(v[i].x, v[i].z)          * fx + cx,
                                .y = asin( v[i].y * mag_xyz_recip ) * fy + cy};
    }
}


void mrcal_unproject_lonlat( // output
                             mrcal_point3_t* v,
                             mrcal_point2_t* dv_dq, // May be NULL. Each point
                                                    // gets a block of 3 mrcal_point2_t
                                                    // objects

                             // input
                             const mrcal_point2_t* q,
                             int N,
                             const double* fxycxy)
{
    const double fx = fxycxy[0];
    const double fy = fxycxy[1];
    const double cx = fxycxy[2];
    const double cy = fxycxy[3];

    // equirectangular projection:
    //   q   = (lon, lat)
    //   lon = arctan2(vx, vz)
    //   lat = arcsin(vy / mag(v))
    //
    // Let's say v is normalized. Then:
    //
    // vx/vz = tan(lon)
    // vy    = sin(lat)
    //
    // -> vx = vz tan(lon)
    // -> 1-sin^2(lat) = vz^2 (1 + tan^2(lon)) =
    //    cos^2(lat)   = (vz/cos(lon))^2
    //
    // -> vx = cos(lat) sin(lon)
    //    vy = sin(lat)
    //    vz = cos(lat) cos(lon)
    //
    // mag(v) is arbitrary, and I can simplify:
    //
    // -> v_unnormalized_x = sin(lon)
    //    v_unnormalized_y = tan(lat)
    //    v_unnormalized_z = cos(lon)
    //
    // If the computational cost of tan(lat) is smaller than of
    // sin(lat),cos(lat) and 2 multiplications, then this is a better
    // representation. A quick web search tells me that the cost of sincos ~ the
    // cost of either sin or cos. And that tan is more costly. So I use the
    // normalized form
    //
    // dv/dlon = [ cos(lat) cos(lon)   0          -cos(lat) sin(lon)]
    // dv/dlat = [-sin(lat) sin(lon)   cos(lat)   -sin(lat) cos(lon)]
    double fx_recip = 1./fx;
    double fy_recip = 1./fy;
    for(int i=0; i<N; i++)
    {
        double lon = (q[i].x - cx) * fx_recip;
        double lat = (q[i].y - cy) * fy_recip;

        double slat = sin(lat);
        double clat = cos(lat);
        double slon = sin(lon);
        double clon = cos(lon);
        if(dv_dq)
        {
            dv_dq[3*i + 0] = (mrcal_point2_t){.x =  fx_recip * clat * clon,
                                              .y = -fy_recip * slat * slon};
            dv_dq[3*i + 1] = (mrcal_point2_t){.y =  fy_recip * clat };
            dv_dq[3*i + 2] = (mrcal_point2_t){.x = -fx_recip * clat * slon,
                                              .y = -fy_recip * slat * clon };
        }
        v[i] = (mrcal_point3_t){.x = clat * slon,
                                .y = slat,
                                .z = clat * clon};
    }
}

void mrcal_project_latlon( // output
                           mrcal_point2_t* q,
                           mrcal_point3_t* dq_dv, // May be NULL. Each point
                                                  // gets a block of 2 mrcal_point3_t
                                                  // objects

                           // input
                           const mrcal_point3_t* v,
                           int N,
                           const double* fxycxy)
{
    const double fx = fxycxy[0];
    const double fy = fxycxy[1];
    const double cx = fxycxy[2];
    const double cy = fxycxy[3];

    // copy of mrcal_project_lonlat(), with swapped x/y
    for(int i=0; i<N; i++)
    {
        double norm2_xyz_recip = 1. / (v[i].x*v[i].x +
                                       v[i].y*v[i].y +
                                       v[i].z*v[i].z );
        double mag_xyz_recip = sqrt(norm2_xyz_recip);
        double norm2_yz_recip = 1. / (v[i].y*v[i].y +
                                      v[i].z*v[i].z );
        double mag_yz_recip = sqrt(norm2_yz_recip);


        if(dq_dv)
        {
            dq_dv[2*i + 0] = (mrcal_point3_t){.x = -fx*mag_yz_recip   * (v[i].x*v[i].x * norm2_xyz_recip - 1.),
                                              .y = -fx*mag_yz_recip   * (v[i].x*v[i].y * norm2_xyz_recip),
                                              .z = -fx*mag_yz_recip   * (v[i].x*v[i].z * norm2_xyz_recip) };
            dq_dv[2*i + 1] = (mrcal_point3_t){.y =  fy*norm2_yz_recip * v[i].z,
                                              .z = -fy*norm2_yz_recip * v[i].y };
        }
        q[i] = (mrcal_point2_t){.x = asin( v[i].x * mag_xyz_recip ) * fx + cx,
                                .y = atan2(v[i].y, v[i].z)          * fy + cy};
    }
}


void mrcal_unproject_latlon( // output
                             mrcal_point3_t* v,
                             mrcal_point2_t* dv_dq, // May be NULL. Each point
                                                    // gets a block of 3 mrcal_point2_t
                                                    // objects

                             // input
                             const mrcal_point2_t* q,
                             int N,
                             const double* fxycxy)
{
    const double fx = fxycxy[0];
    const double fy = fxycxy[1];
    const double cx = fxycxy[2];
    const double cy = fxycxy[3];

    // copy of mrcal_unproject_lonlat(), with swapped x/y
    double fx_recip = 1./fx;
    double fy_recip = 1./fy;
    for(int i=0; i<N; i++)
    {
        double lat = (q[i].x - cx) * fx_recip;
        double lon = (q[i].y - cy) * fy_recip;

        double slat = sin(lat);
        double clat = cos(lat);
        double slon = sin(lon);
        double clon = cos(lon);
        if(dv_dq)
        {
            dv_dq[3*i + 0] = (mrcal_point2_t){.x =  fx_recip * clat };
            dv_dq[3*i + 1] = (mrcal_point2_t){.x = -fx_recip * slat * slon,
                                              .y =  fy_recip * clat * clon };
            dv_dq[3*i + 2] = (mrcal_point2_t){.x = -fx_recip * slat * clon,
                                              .y = -fy_recip * clat * slon };
        }
        v[i] = (mrcal_point3_t){.x = slat,
                                .y = clat * slon,
                                .z = clat * clon};
    }
}


static void _mrcal_precompute_lensmodel_data_MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC
  ( // output
    mrcal_LENSMODEL_SPLINED_STEREOGRAPHIC__precomputed_t* precomputed,

    //input
    const mrcal_LENSMODEL_SPLINED_STEREOGRAPHIC__config_t* config )
{
    // I have N control points describing a given field-of-view. I
    // want to space out the control points evenly. I'm using
    // B-splines, so I need extra control points out past my edge.
    // With cubic splines I need a whole extra interval past the
    // edge. With quadratic splines I need half an interval (see
    // stuff in analyses/splines/).
    //
    // (u_width_needed + Nknots_margin*u_interval_size)/(Nknots - 1) = u_interval_size
    // ---> u_width_needed/(Nknots-1) = u_interval_size * (1 - Nknots_margin/(Nknots - 1))
    // ---> u_width_needed  = u_interval_size * (Nknots - 1 - Nknots_margin)
    // ---> u_interval_size = u_width_needed  / (Nknots - 1 - Nknots_margin)
    int Nknots_margin;
    if(config->order == 2)
    {
        Nknots_margin = 1;
        if(config->Nx < 3 || config->Ny < 3)
        {
            MSG("Quadratic splines: absolute minimum Nx, Ny is 3. Got Nx=%d Ny=%d. Barfing out",
                config->Nx, config->Ny);
            assert(0);
        }
    }
    else if(config->order == 3)
    {
        Nknots_margin = 2;
        if(config->Nx < 4 || config->Ny < 4)
        {
            MSG("Cubic splines: absolute minimum Nx, Ny is 4. Got Nx=%d Ny=%d. Barfing out",
                config->Nx, config->Ny);
            assert(0);
        }
    }
    else
    {
        MSG("I only support spline order 2 and 3");
        assert(0);
    }

    double th_edge_x = (double)config->fov_x_deg/2. * M_PI / 180.;
    double u_edge_x  = tan(th_edge_x / 2.) * 2;
    precomputed->segments_per_u = (config->Nx - 1 - Nknots_margin) / (u_edge_x*2.);
}

// NOT A PART OF THE EXTERNAL API. This is exported for the mrcal python wrapper
// only
void _mrcal_precompute_lensmodel_data(mrcal_projection_precomputed_t* precomputed,
                                      const mrcal_lensmodel_t* lensmodel)
{
    // currently only this model has anything
    if(lensmodel->type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC)
        _mrcal_precompute_lensmodel_data_MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC
            ( &precomputed->LENSMODEL_SPLINED_STEREOGRAPHIC__precomputed,
              &lensmodel->LENSMODEL_SPLINED_STEREOGRAPHIC__config );
    precomputed->ready = true;
}

bool mrcal_knots_for_splined_models( // buffers must hold at least
                                     // config->Nx and config->Ny values
                                     // respectively
                                     double* ux, double* uy,
                                     const mrcal_lensmodel_t* lensmodel)
{
    if(lensmodel->type != MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC)
    {
        MSG("This function works only with the MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC model. '%s' passed in",
            mrcal_lensmodel_name_unconfigured(lensmodel));
        return false;
    }

    mrcal_projection_precomputed_t precomputed_all;
    _mrcal_precompute_lensmodel_data(&precomputed_all, lensmodel);

    const mrcal_LENSMODEL_SPLINED_STEREOGRAPHIC__config_t* config =
        &lensmodel->LENSMODEL_SPLINED_STEREOGRAPHIC__config;
    const mrcal_LENSMODEL_SPLINED_STEREOGRAPHIC__precomputed_t* precomputed =
        &precomputed_all.LENSMODEL_SPLINED_STEREOGRAPHIC__precomputed;

    // The logic I'm reversing is
    //     double ix = u.x*segments_per_u + (double)(Nx-1)/2.;
    for(int i=0; i<config->Nx; i++)
        ux[i] =
            ((double)i - (double)(config->Nx-1)/2.) /
            precomputed->segments_per_u;
    for(int i=0; i<config->Ny; i++)
        uy[i] =
            ((double)i - (double)(config->Ny-1)/2.) /
            precomputed->segments_per_u;
    return true;
}

static int get_Ngradients(const mrcal_lensmodel_t* lensmodel,
                          int Nintrinsics)
{
    int N = 0;
    bool has_core                   = modelHasCore_fxfycxcy(lensmodel);
    bool has_dense_intrinsics_grad  = (lensmodel->type != MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC);
    bool has_sparse_intrinsics_grad = (lensmodel->type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC);
    int runlen = (lensmodel->type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC) ?
        (lensmodel->LENSMODEL_SPLINED_STEREOGRAPHIC__config.order + 1) :
        0;
    if(has_core)
        // qx(fx) and qy(fy)
        N += 2;
    if(has_dense_intrinsics_grad)
        // each of (qx,qy) depends on all the non-core intrinsics
        N += 2 * (Nintrinsics-4);
    if(has_sparse_intrinsics_grad)
    {
        // spline coefficients
        N += 2*runlen;
    }

    return N;
}

static
void propagate_extrinsics__splined( // output
                                    mrcal_point3_t* dq_deee,
                                    // input
                                    const mrcal_point3_t* dp_deee,
                                    const double* duxy_dp,
                                    const double* ddeltau_dux,
                                    const double* ddeltau_duy,
                                    const double  fx,
                                    const double  fy)
{
    mrcal_point3_t du_deee[2];
    mul_genN3_gen33_vout(2, (double*)duxy_dp, (double*)dp_deee, (double*)du_deee);

    for(int i=0; i<3; i++)
    {
        dq_deee[0].xyz[i] =
            fx *
            ( du_deee[0].xyz[i] * (1. + ddeltau_dux[0]) +
              ddeltau_duy[0] * du_deee[1].xyz[i]);
        dq_deee[1].xyz[i] =
            fy *
            ( du_deee[1].xyz[i] * (1. + ddeltau_duy[1]) +
              ddeltau_dux[1] * du_deee[0].xyz[i]);
    }
}
static
void propagate_extrinsics_cam0__splined( // output
                                         mrcal_point3_t* dq_deee,
                                         // input
                                         const double* dux_dp,
                                         const double* duy_dp,
                                         const double* ddeltau_dux,
                                         const double* ddeltau_duy,
                                         const double  fx,
                                         const double  fy)
{
    for(int i=0; i<3; i++)
    {
        dq_deee[0].xyz[i] =
            fx *
            ( dux_dp[i] * (1. + ddeltau_dux[0]) +
              ddeltau_duy[0] * duy_dp[i]);
        dq_deee[1].xyz[i] =
            fy *
            ( duy_dp[i] * (1. + ddeltau_duy[1]) +
              ddeltau_dux[1] * dux_dp[i]);
    }
}
static
void _project_point_splined( // outputs
                            mrcal_point2_t* q,
                            mrcal_point2_t* dq_dfxy,

                            double* grad_ABCDx_ABCDy,
                            int* ivar0,

                            // Gradient outputs. May be NULL
                            mrcal_point3_t* restrict dq_drcamera,
                            mrcal_point3_t* restrict dq_dtcamera,
                            mrcal_point3_t* restrict dq_drframe,
                            mrcal_point3_t* restrict dq_dtframe,

                            // inputs
                            const mrcal_point3_t* restrict p,
                            const mrcal_point3_t* restrict dp_drc,
                            const mrcal_point3_t* restrict dp_dtc,
                            const mrcal_point3_t* restrict dp_drf,
                            const mrcal_point3_t* restrict dp_dtf,

                            const double* restrict intrinsics,
                            bool camera_at_identity,
                            int spline_order,
                            uint16_t Nx, uint16_t Ny,
                            double segments_per_u)
{
    // projections out-of-bounds will yield SOME value (function remains
    // continuous as we move out-of-bounds), but it wont be particularly
    // meaningful


    // stereographic projection:
    //   (from https://en.wikipedia.org/wiki/Fisheye_lens)
    //   u = xy_unit * tan(th/2) * 2
    //
    // I compute the normalized (focal-length = 1) projection, and
    // use that to look-up deltau

    // th is the angle between the observation and the projection
    // center
    //
    // sin(th)   = mag_pxy/mag_p
    // cos(th)   = z/mag_p
    // tan(th/2) = sin(th) / (1 + cos(th))

    // tan(th/2) = mag_pxy/mag_p / (1 + z/mag_p) =
    //           = mag_pxy / (mag_p + z)
    // u = xy_unit * tan(th/2) * 2 =
    //   = xy/mag_pxy * mag_pxy/(mag_p + z) * 2 =
    //   = xy / (mag_p + z) * 2
    //
    // The stereographic projection is used to query the spline surface, and it
    // is also the projection baseline. I do
    //
    //   q = (u + deltau(u)) * f + c
    //
    // If the spline surface is at 0 (deltau == 0) then this is a pure
    // stereographic projection
    double mag_p = sqrt( p->x*p->x +
                         p->y*p->y +
                         p->z*p->z );
    double scale = 2.0 / (mag_p + p->z);

    mrcal_point2_t u = {.x = p->x * scale,
                        .y = p->y * scale};
    // du/dp = d/dp ( xy * scale )
    //       = pxy dscale/dp + [I; 0] scale
    // dscale/dp = d (2.0 / (mag_p + p->z))/dp =
    //           = -2/()^2 ( [0,0,1] + dmag/dp)
    //           = -2/()^2 ( [0,0,1] + 2pt/2mag)
    //           = -2 scale^2/4 ( [0,0,1] + pt/mag)
    //           = -scale^2/2 * ( [0,0,1] + pt/mag )
    //           = A*[0,0,1] + B*pt
    double A = -scale*scale / 2.;
    double B = A / mag_p;
    double du_dp[2][3] = { { p->x * (B * p->x)      + scale,
                             p->x * (B * p->y),
                             p->x * (B * p->z + A) },
                           { p->y * (B * p->x),
                             p->y * (B * p->y)      + scale,
                             p->y * (B * p->z + A) } };

    double ix = u.x*segments_per_u + (double)(Nx-1)/2.;
    double iy = u.y*segments_per_u + (double)(Ny-1)/2.;

    mrcal_point2_t deltau;
    double ddeltau_dux[2];
    double ddeltau_duy[2];
    const double fx = intrinsics[0];
    const double fy = intrinsics[1];
    const double cx = intrinsics[2];
    const double cy = intrinsics[3];

    if( spline_order == 3 )
    {
        int ix0 = (int)ix;
        int iy0 = (int)iy;

        // If out-of-bounds, clamp to the nearest valid spline segment. The
        // projection will fly off to infinity quickly (we're extrapolating a
        // polynomial), but at least it'll stay continuous
        if(     ix0 < 1)    ix0 = 1;
        else if(ix0 > Nx-3) ix0 = Nx-3;
        if(     iy0 < 1)    iy0 = 1;
        else if(iy0 > Ny-3) iy0 = Ny-3;

        *ivar0 =
            4 + // skip the core
            2*( (iy0-1)*Nx +
                (ix0-1) );

        sample_bspline_surface_cubic(deltau.xy,
                                     ddeltau_dux, ddeltau_duy,
                                     grad_ABCDx_ABCDy,
                                     ix - ix0, iy - iy0,

                                     // control points
                                     &intrinsics[*ivar0],
                                     2*Nx);
    }
    else if( spline_order == 2 )
    {
        int ix0 = (int)(ix + 0.5);
        int iy0 = (int)(iy + 0.5);

        // If out-of-bounds, clamp to the nearest valid spline segment. The
        // projection will fly off to infinity quickly (we're extrapolating a
        // polynomial), but at least it'll stay continuous
        if(     ix0 < 1)    ix0 = 1;
        else if(ix0 > Nx-2) ix0 = Nx-2;
        if(     iy0 < 1)    iy0 = 1;
        else if(iy0 > Ny-2) iy0 = Ny-2;

        *ivar0 =
            4 + // skip the core
            2*( (iy0-1)*Nx +
                (ix0-1) );

        sample_bspline_surface_quadratic(deltau.xy,
                                         ddeltau_dux, ddeltau_duy,
                                         grad_ABCDx_ABCDy,
                                         ix - ix0, iy - iy0,

                                         // control points
                                         &intrinsics[*ivar0],
                                         2*Nx);
    }
    else
    {
        MSG("I only support spline order==2 or 3. Somehow got %d. This is a bug. Barfing",
            spline_order);
        assert(0);
    }

    // u = stereographic(p)
    // q = (u + deltau(u)) * f + c
    //
    // Extrinsics:
    //   dqx/deee = fx (dux/deee + ddeltaux/du du/deee)
    //            = fx ( dux/deee (1 + ddeltaux/dux) + ddeltaux/duy duy/deee)
    //   dqy/deee = fy ( duy/deee (1 + ddeltauy/duy) + ddeltauy/dux dux/deee)
    q->x = (u.x + deltau.x) * fx + cx;
    q->y = (u.y + deltau.y) * fy + cy;

    if( dq_dfxy )
    {
        // I have the projection, and I now need to propagate the gradients
        // xy = fxy * distort(xy)/distort(z) + cxy
        dq_dfxy->x = u.x + deltau.x;
        dq_dfxy->y = u.y + deltau.y;
    }

    // convert ddeltau_dixy to ddeltau_duxy
    for(int i=0; i<2; i++)
    {
        ddeltau_dux[i] *= segments_per_u;
        ddeltau_duy[i] *= segments_per_u;
    }

    if(camera_at_identity)
    {
        if( dq_drcamera != NULL ) memset(dq_drcamera->xyz, 0, 6*sizeof(double));
        if( dq_dtcamera != NULL ) memset(dq_dtcamera->xyz, 0, 6*sizeof(double));
        if( dq_drframe  != NULL )
            propagate_extrinsics__splined( dq_drframe,  dp_drf,
                                           (const double*)du_dp,
                                           ddeltau_dux, ddeltau_duy,
                                           fx,fy);
        if( dq_dtframe  != NULL )
            propagate_extrinsics_cam0__splined( dq_dtframe,
                                                du_dp[0], du_dp[1],
                                                ddeltau_dux, ddeltau_duy,
                                                fx, fy);
    }
    else
    {
        if( dq_drcamera != NULL )
            propagate_extrinsics__splined( dq_drcamera, dp_drc,
                                           (const double*)du_dp,
                                           ddeltau_dux, ddeltau_duy,
                                           fx,fy);
        if( dq_dtcamera != NULL )
            propagate_extrinsics__splined( dq_dtcamera, dp_dtc,
                                           (const double*)du_dp,
                                           ddeltau_dux, ddeltau_duy,
                                           fx,fy);
        if( dq_drframe  != NULL )
            propagate_extrinsics__splined( dq_drframe,  dp_drf,
                                           (const double*)du_dp,
                                           ddeltau_dux, ddeltau_duy,
                                           fx,fy);
        if( dq_dtframe  != NULL )
            propagate_extrinsics__splined( dq_dtframe,  dp_dtf,
                                           (const double*)du_dp,
                                           ddeltau_dux, ddeltau_duy,
                                           fx,fy);
    }
}

typedef struct
{
    double* pool;
    uint16_t run_side_length;
    uint16_t ivar_stridey;
} gradient_sparse_meta_t;


// This is internal to project()
static
void _propagate_extrinsics_one(mrcal_point3_t* dp_dparam,
                               const mrcal_point3_t* pt_ref,
                               const double* drj_dparam,
                               const double* dtj_dparam,
                               const double* d_Rj_rj)
{
    // dRj[row0]/drj is 3x3 matrix at &d_Rj_rj[0]
    // dRj[row0]/drc = dRj[row0]/drj * drj_drc
    for(int i=0; i<3; i++)
    {
        mul_vec3_gen33_vout( pt_ref->xyz, &d_Rj_rj[9*i], dp_dparam[i].xyz );
        mul_vec3_gen33     ( dp_dparam[i].xyz,   drj_dparam);
        add_vec(3, dp_dparam[i].xyz, &dtj_dparam[3*i] );
    }
}
static
void _propagate_extrinsics_one_rzero(mrcal_point3_t* dp_dparam,
                                     const mrcal_point3_t* pt_ref,
                                     const double* dtj_dparam,
                                     const double* d_Rj_rj)
{
    // dRj[row0]/drj is 3x3 matrix at &d_Rj_rj[0]
    // dRj[row0]/drc = dRj[row0]/drj * drj_drc
    memcpy(dp_dparam->xyz, dtj_dparam, 9*sizeof(double));
}
static
void _propagate_extrinsics_one_tzero(mrcal_point3_t* dp_dparam,
                                     const mrcal_point3_t* pt_ref,
                                     const double* drj_dparam,
                                     const double* d_Rj_rj)
{
    // dRj[row0]/drj is 3x3 matrix at &d_Rj_rj[0]
    // dRj[row0]/drc = dRj[row0]/drj * drj_drc
    for(int i=0; i<3; i++)
    {
        mul_vec3_gen33_vout( pt_ref->xyz, &d_Rj_rj[9*i], dp_dparam[i].xyz );
        mul_vec3_gen33     ( dp_dparam[i].xyz,   drj_dparam);
    }
}
static
void _propagate_extrinsics_one_rzero_tidentity(mrcal_point3_t* dp_dparam,
                                               const mrcal_point3_t* pt_ref,
                                               const double* d_Rj_rj)
{
    dp_dparam[0] = (mrcal_point3_t){.x = 1.0};
    dp_dparam[1] = (mrcal_point3_t){.y = 1.0};
    dp_dparam[2] = (mrcal_point3_t){.z = 1.0};
}

static
void _propagate_extrinsics_one_cam0(mrcal_point3_t* dp_rf,
                                    const mrcal_point3_t* pt_ref,
                                    const double* _d_Rf_rf)
{
    // dRj[row0]/drj is 3x3 matrix at &_d_Rf_rf[0]
    // dRj[row0]/drc = dRj[row0]/drj * drj_drc
    for(int i=0; i<3; i++)
        mul_vec3_gen33_vout( pt_ref->xyz, &_d_Rf_rf[9*i], dp_rf[i].xyz );
}
static
mrcal_point3_t _propagate_extrinsics( // output
                                      mrcal_point3_t* _dp_drc,
                                      mrcal_point3_t* _dp_dtc,
                                      mrcal_point3_t* _dp_drf,
                                      mrcal_point3_t* _dp_dtf,
                                      mrcal_point3_t** dp_drc,
                                      mrcal_point3_t** dp_dtc,
                                      mrcal_point3_t** dp_drf,
                                      mrcal_point3_t** dp_dtf,

                                      // input
                                      const mrcal_point3_t* pt_ref,
                                      const geometric_gradients_t* gg,
                                      const double* Rj, const double* d_Rj_rj,
                                      const double* _tj )
{
    // Rj * pt + tj -> pt
    mrcal_point3_t p;
    mul_vec3_gen33t_vout(pt_ref->xyz, Rj, p.xyz);
    add_vec(3, p.xyz,  _tj);

    if(gg != NULL)
    {
        _propagate_extrinsics_one(                _dp_drc, pt_ref, gg->_d_rj_rc, gg->_d_tj_rc, d_Rj_rj);
        _propagate_extrinsics_one_rzero_tidentity(_dp_dtc, pt_ref,                             d_Rj_rj);
        _propagate_extrinsics_one_tzero(          _dp_drf, pt_ref, gg->_d_rj_rf,               d_Rj_rj);
        _propagate_extrinsics_one_rzero(          _dp_dtf, pt_ref,               gg->_d_tj_tf, d_Rj_rj);
        *dp_drc = _dp_drc;
        *dp_dtc = _dp_dtc;
        *dp_drf = _dp_drf;
        *dp_dtf = _dp_dtf;
    }
    else
    {
        // camera is at the reference. The "joint" coord system is the "frame"
        // coord system
        //
        //   p_cam = Rf p_ref + tf
        //
        // dp/drc = 0
        // dp/dtc = 0
        // dp/drf = reshape(dRf_drf p_ref)
        // dp/dtf = I
        _propagate_extrinsics_one_cam0(_dp_drf, pt_ref, d_Rj_rj);

        *dp_drc = NULL;
        *dp_dtc = NULL;
        *dp_drf = _dp_drf;
        *dp_dtf = NULL; // this is I. The user of this MUST know to interpret
        // it that way
    }
    return p;
}
// This is internal to project()
static
void _project_point( // outputs
                     mrcal_point2_t* q,
                     mrcal_point2_t* p_dq_dfxy,
                     double* p_dq_dintrinsics_nocore,
                     double* gradient_sparse_meta_pool,
                     int runlen,
                     mrcal_point3_t* restrict dq_drcamera,
                     mrcal_point3_t* restrict dq_dtcamera,
                     mrcal_point3_t* restrict dq_drframe,
                     mrcal_point3_t* restrict dq_dtframe,
                     mrcal_calobject_warp_t* restrict dq_dcalobject_warp,
                     int* restrict *          dq_dintrinsics_pool_int,
                     // inputs
                     const mrcal_point3_t* p,
                     const double* restrict intrinsics,
                     const mrcal_lensmodel_t* lensmodel,
                     const mrcal_calobject_warp_t* dpt_refz_dwarp,
                     // if NULL then the camera is at the reference
                     bool camera_at_identity,
                     const double* Rj,
                     const int Nintrinsics,
                     const mrcal_projection_precomputed_t* precomputed,
                     const mrcal_point3_t* dp_drc,
                     const mrcal_point3_t* dp_dtc,
                     const mrcal_point3_t* dp_drf,
                     const mrcal_point3_t* dp_dtf)
{
    if(lensmodel->type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC)
    {
        // only need 3+3 for quadratic splines
        double grad_ABCDx_ABCDy[4+4];
        int ivar0;

        _project_point_splined( // outputs
                                q, p_dq_dfxy,
                                grad_ABCDx_ABCDy,
                                &ivar0,

                                dq_drcamera,dq_dtcamera,dq_drframe,dq_dtframe,
                                // inputs
                                p,
                                dp_drc, dp_dtc, dp_drf, dp_dtf,
                                intrinsics,
                                camera_at_identity,
                                lensmodel->LENSMODEL_SPLINED_STEREOGRAPHIC__config.order,
                                lensmodel->LENSMODEL_SPLINED_STEREOGRAPHIC__config.Nx,
                                lensmodel->LENSMODEL_SPLINED_STEREOGRAPHIC__config.Ny,
                                precomputed->LENSMODEL_SPLINED_STEREOGRAPHIC__precomputed.segments_per_u);
        // WARNING: if I could assume that dq_dintrinsics_pool_double!=NULL then I wouldnt need to copy the context
        if(*dq_dintrinsics_pool_int != NULL)
        {
            *((*dq_dintrinsics_pool_int)++) = ivar0;
            memcpy(gradient_sparse_meta_pool,
                   grad_ABCDx_ABCDy,
                   sizeof(double)*runlen*2);
        }
    }
    else if(lensmodel->type == MRCAL_LENSMODEL_CAHVOR)
    {
        project_cahvor( // outputs
                        q,p_dq_dfxy,
                        p_dq_dintrinsics_nocore,
                        dq_drcamera,dq_dtcamera,dq_drframe,dq_dtframe,
                        // inputs
                        p,
                        dp_drc, dp_dtc, dp_drf, dp_dtf,
                        intrinsics,
                        camera_at_identity,
                        lensmodel);
    }
    else if(lensmodel->type == MRCAL_LENSMODEL_CAHVORE)
    {
        if(!project_cahvore( // outputs
                             q,p_dq_dfxy,
                             p_dq_dintrinsics_nocore,
                             dq_drcamera,dq_dtcamera,dq_drframe,dq_dtframe,
                             // inputs
                             p,
                             dp_drc, dp_dtc, dp_drf, dp_dtf,
                             intrinsics,
                             camera_at_identity,
                             lensmodel))
        {
            MSG("CAHVORE PROJECTION OF (%f,%f,%f) FAILED. I don't know what to do. Setting result and all gradients to 0",
                p->x, p->y, p->z);
            memset(q, 0, sizeof(*q));
            if(p_dq_dfxy)               memset(p_dq_dfxy,               0, sizeof(*p_dq_dfxy));
            if(p_dq_dintrinsics_nocore) memset(p_dq_dintrinsics_nocore, 0, sizeof(*p_dq_dintrinsics_nocore) * 2 * (Nintrinsics-4));
            if(dq_drcamera)             memset(dq_drcamera,             0, sizeof(*dq_drcamera));
            if(dq_dtcamera)             memset(dq_dtcamera,             0, sizeof(*dq_dtcamera));
            if(dq_drframe)              memset(dq_drframe,              0, sizeof(*dq_drframe));
            if(dq_dtframe)              memset(dq_dtframe,              0, sizeof(*dq_dtframe));

        }
    }
    else
    {
        _project_point_parametric( // outputs
                                   q,p_dq_dfxy,
                                   p_dq_dintrinsics_nocore,
                                   dq_drcamera,dq_dtcamera,dq_drframe,dq_dtframe,
                                   // inputs
                                   p,
                                   dp_drc, dp_dtc, dp_drf, dp_dtf,
                                   intrinsics,
                                   camera_at_identity,
                                   lensmodel);
    }

    if( dq_dcalobject_warp != NULL && dpt_refz_dwarp != NULL )
    {
        // p = proj(Rc Rf warp(x) + Rc tf + tc);
        // dp/dw = dp/dRcRf(warp(x)) dR(warp(x))/dwarp(x) dwarp/dw =
        //       = dp/dtc RcRf dwarp/dw
        // dp/dtc is dq_dtcamera
        // R is rodrigues(rj)
        // dwarp/dw = [0 0 0 ...]
        //            [0 0 0 ...]
        //            [a b c ...]
        // Let R = [r0 r1 r2]
        // dp/dw = dp/dt [a r2   b r2] =
        //         [a dp/dt r2    b dp/dt r2  ...]
        mrcal_point3_t* p_dq_dt;
        if(!camera_at_identity) p_dq_dt = dq_dtcamera;
        else                    p_dq_dt = dq_dtframe;
        double d[] =
            { p_dq_dt[0].xyz[0] * Rj[0*3 + 2] +
              p_dq_dt[0].xyz[1] * Rj[1*3 + 2] +
              p_dq_dt[0].xyz[2] * Rj[2*3 + 2],
              p_dq_dt[1].xyz[0] * Rj[0*3 + 2] +
              p_dq_dt[1].xyz[1] * Rj[1*3 + 2] +
              p_dq_dt[1].xyz[2] * Rj[2*3 + 2]};

        for(int i=0; i<MRCAL_NSTATE_CALOBJECT_WARP; i++)
        {
            dq_dcalobject_warp[0].values[i] = d[0]*dpt_refz_dwarp->values[i];
            dq_dcalobject_warp[1].values[i] = d[1]*dpt_refz_dwarp->values[i];
        }
    }
}

// Projects 3D point(s), and reports the projection, and all the gradients. This
// is the main internal callback in the optimizer. This operates in one of two modes:
//
// if(calibration_object_width_n == 0) then we're projecting ONE point. In world
// coords this point is at frame_rt->t. frame_rt->r is not referenced. q and the
// gradients reference 2 values (x,y in the imager)
//
// if(calibration_object_width_n > 0) then we're projecting a whole calibration
// object. The pose of this object is given in frame_rt. We project ALL
// calibration_object_width_n*calibration_object_height_n points. q and the
// gradients reference ALL of these points
static
void project( // out
             mrcal_point2_t* restrict q,

             // The intrinsics gradients. These are split among several arrays.
             // High-parameter-count lens models can return their gradients
             // sparsely. All the actual gradient values live in
             // dq_dintrinsics_pool_double, a buffer supplied by the caller. If
             // dq_dintrinsics_pool_double is not NULL, the rest of the
             // variables are assumed non-NULL, and we compute all the
             // intrinsics gradients. Conversely, if dq_dintrinsics_pool_double
             // is NULL, no intrinsics gradients are computed
             double*  restrict dq_dintrinsics_pool_double,
             int*     restrict dq_dintrinsics_pool_int,
             double** restrict dq_dfxy,
             double** restrict dq_dintrinsics_nocore,
             gradient_sparse_meta_t* gradient_sparse_meta,
             mrcal_point3_t* restrict dq_drcamera,
             mrcal_point3_t* restrict dq_dtcamera,
             mrcal_point3_t* restrict dq_drframe,
             mrcal_point3_t* restrict dq_dtframe,
             mrcal_calobject_warp_t* restrict dq_dcalobject_warp,

             // in

             // everything; includes the core, if there is one
             const double* restrict intrinsics,
             const mrcal_pose_t* restrict camera_rt,
             const mrcal_pose_t* restrict frame_rt,
             const mrcal_calobject_warp_t* restrict calobject_warp,

             bool camera_at_identity, // if true, camera_rt is unused
             const mrcal_lensmodel_t* lensmodel,
             const mrcal_projection_precomputed_t* precomputed,

             double calibration_object_spacing,
             int    calibration_object_width_n,
             int    calibration_object_height_n)
{
    assert(precomputed->ready);

    // Parametric and non-parametric models do different things:
    //
    // parametric models:
    //   u = distort(p, distortions)
    //   q = uxy/uz * fxy + cxy
    //
    //   extrinsic gradients:
    //       dqx/deee = d( ux/uz * fx + cx)/deee =
    //                = fx d(ux/uz)/deee =
    //                = fx/uz^2 ( uz dux/deee - duz/deee ux )
    //
    // nonparametric (splined) models
    //   u = stereographic(p)
    //   q = (u + deltau(u)) * f + c
    //
    //   Extrinsics:
    //     dqx/deee = fx (dux/deee + ddeltaux/du du/deee)
    //              = fx ( dux/deee (1 + ddeltaux/dux) + ddeltaux/duy duy/deee)
    //     dqy/deee = fy ( duy/deee (1 + ddeltauy/duy) + ddeltauy/dux dux/deee)
    //
    //   Intrinsics:
    //     dq/diii = f ddeltau/diii
    //
    // So the two kinds of models have completely different expressions for
    // their gradients, and I implement them separately

    const int Npoints =
        calibration_object_width_n ?
        calibration_object_width_n*calibration_object_height_n : 1;
    const int Nintrinsics = mrcal_lensmodel_num_params(lensmodel);

    // I need to compose two transformations
    //
    // (object in reference frame) -> [frame transform] -> (object in the reference frame) ->
    // -> [camera rt] -> (camera frame)
    //
    // Note that here the frame transform transforms TO the reference frame and
    // the camera transform transforms FROM the reference frame. This is how my
    // data is expected to be set up
    //
    // [Rc tc] [Rf tf] = [Rc*Rf  Rc*tf + tc]
    // [0  1 ] [0  1 ]   [0      1         ]
    //
    // This transformation (and its gradients) is handled by mrcal_compose_rt()
    // I refer to the camera*frame transform as the "joint" transform, or the
    // letter j
    geometric_gradients_t gg;

    double _joint_rt[6];
    double* joint_rt;

    // The caller has an odd-looking array reference [-3]. This is intended, but
    // the compiler throws a warning. I silence it here. gcc-10 produces a very
    // strange-looking warning that was reported to the maintainers:
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97261
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    mrcal_pose_t frame_rt_validr = {.t = frame_rt->t};
#pragma GCC diagnostic pop
    if(calibration_object_width_n) frame_rt_validr.r = frame_rt->r;

    if(!camera_at_identity)
    {
        // make sure I can pass mrcal_pose_t.r as an rt[] transformation
        mrcal_compose_rt( _joint_rt,
                          gg._d_rj_rc, gg._d_rj_rf,
                          gg._d_tj_rc, gg._d_tj_tf,
                          camera_rt     ->r.xyz,
                          frame_rt_validr.r.xyz);
        joint_rt = _joint_rt;
    }
    else
    {
        // We're looking at the reference frame, so this camera transform is
        // fixed at the identity. We don't need to compose anything, nor
        // propagate gradients for the camera extrinsics, since those don't
        // exist in the parameter vector

        // Here the "joint" transform is just the "frame" transform
        joint_rt = frame_rt_validr.r.xyz;
    }

    // Not using OpenCV distortions, the distortion and projection are not
    // coupled
    double Rj[3*3];
    double d_Rj_rj[9*3];

    mrcal_R_from_r(Rj, d_Rj_rj, joint_rt);

    mrcal_point2_t* p_dq_dfxy                  = NULL;
    double*   p_dq_dintrinsics_nocore    = NULL;
    bool      has_core                   = modelHasCore_fxfycxcy(lensmodel);
    bool      has_dense_intrinsics_grad  = (lensmodel->type != MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC);
    bool      has_sparse_intrinsics_grad = (lensmodel->type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC);
    int runlen = (lensmodel->type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC) ?
        (lensmodel->LENSMODEL_SPLINED_STEREOGRAPHIC__config.order + 1) :
        0;

    if(dq_dintrinsics_pool_double != NULL)
    {
        // nothing by default
        *dq_dfxy                   = NULL;
        *dq_dintrinsics_nocore     = NULL;
        gradient_sparse_meta->pool = NULL;
        int ivar_pool = 0;

        if(has_core)
        {
            // Each point produces 2 measurements. Each one depends on ONE fxy
            // element. So Npoints*2 of these
            *dq_dfxy  = &dq_dintrinsics_pool_double[ivar_pool];
            p_dq_dfxy = (mrcal_point2_t*)*dq_dfxy;
            ivar_pool += Npoints*2;
        }
        if(has_dense_intrinsics_grad)
        {
            *dq_dintrinsics_nocore = p_dq_dintrinsics_nocore = &dq_dintrinsics_pool_double[ivar_pool];
            ivar_pool += Npoints*2 * (Nintrinsics-4);
        }
        if(has_sparse_intrinsics_grad)
        {
            if(lensmodel->type != MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC)
            {
                MSG("Unhandled lens model: %d (%s)",
                    lensmodel->type,
                    mrcal_lensmodel_name_unconfigured(lensmodel));
                assert(0);
            }
            const mrcal_LENSMODEL_SPLINED_STEREOGRAPHIC__config_t* config =
                &lensmodel->LENSMODEL_SPLINED_STEREOGRAPHIC__config;
            *gradient_sparse_meta =
                {
                    &dq_dintrinsics_pool_double[ivar_pool],
                    static_cast<uint16_t>(config->order+1),
                    static_cast<uint16_t>(2*config->Nx),
                };
            ivar_pool += Npoints*2 * runlen;
        }
    }

    // These are produced by _propagate_extrinsics() and consumed by
    // _project_point()
    mrcal_point3_t _dp_drc[3];
    mrcal_point3_t _dp_dtc[3];
    mrcal_point3_t _dp_drf[3];
    mrcal_point3_t _dp_dtf[3];
    mrcal_point3_t* dp_drc;
    mrcal_point3_t* dp_dtc;
    mrcal_point3_t* dp_drf;
    mrcal_point3_t* dp_dtf;

    if( calibration_object_width_n == 0 )
    { // projecting discrete points
        mrcal_point3_t pt_ref = {}; // Unused
        mrcal_point3_t p =
            _propagate_extrinsics( _dp_drc,_dp_dtc,_dp_drf,_dp_dtf,
                                   &dp_drc,&dp_dtc,&dp_drf,&dp_dtf,
                                   &pt_ref,
                                   camera_at_identity ? NULL : &gg,
                                   Rj, d_Rj_rj, &joint_rt[3]);
        _project_point(  q,
                         p_dq_dfxy, p_dq_dintrinsics_nocore,
                         gradient_sparse_meta ? gradient_sparse_meta->pool : NULL,
                         runlen,
                         dq_drcamera, dq_dtcamera, dq_drframe, dq_dtframe, NULL,
                         &dq_dintrinsics_pool_int,

                         &p,
                         intrinsics, lensmodel,
                         NULL,
                         camera_at_identity, Rj,
                         Nintrinsics,
                         precomputed,
                         dp_drc,dp_dtc,dp_drf,dp_dtf);
    }
    else
    { // projecting a chessboard
        int i_pt = 0;
         // The calibration object has a simple grid geometry
        for(int y = 0; y<calibration_object_height_n; y++)
            for(int x = 0; x<calibration_object_width_n; x++)
            {
                mrcal_point3_t pt_ref = {.x = (double)x * calibration_object_spacing,
                                         .y = (double)y * calibration_object_spacing};
                mrcal_calobject_warp_t dpt_refz_dwarp = {};

                if(calobject_warp != NULL)
                {
                    // Add a board warp here. I have two parameters, and they describe
                    // additive flex along the x axis and along the y axis, in that
                    // order. In each direction the flex is a parabola, with the
                    // parameter k describing the max deflection at the center. If the
                    // ends are at +- 1 I have d = k*(1 - x^2). If the ends are at
                    // (0,N-1) the equivalent expression is: d = k*( 1 - 4*x^2/(N-1)^2 +
                    // 4*x/(N-1) - 1 ) = d = 4*k*(x/(N-1) - x^2/(N-1)^2) = d =
                    // 4.*k*x*r(1. - x*r)
                    double xr = (double)x / (double)(calibration_object_width_n -1);
                    double yr = (double)y / (double)(calibration_object_height_n-1);
                    double dx = 4. * xr * (1. - xr);
                    double dy = 4. * yr * (1. - yr);
                    pt_ref.z += calobject_warp->x2 * dx;
                    pt_ref.z += calobject_warp->y2 * dy;
                    dpt_refz_dwarp.x2 = dx;
                    dpt_refz_dwarp.y2 = dy;
                }

                mrcal_point3_t p =
                    _propagate_extrinsics( _dp_drc,_dp_dtc,_dp_drf,_dp_dtf,
                                           &dp_drc,&dp_dtc,&dp_drf,&dp_dtf,
                                           &pt_ref,
                                           camera_at_identity ? NULL : &gg,
                                           Rj, d_Rj_rj, &joint_rt[3]);

                mrcal_point3_t* dq_drcamera_here          = dq_drcamera        ? &dq_drcamera        [i_pt*2] : NULL;
                mrcal_point3_t* dq_dtcamera_here          = dq_dtcamera        ? &dq_dtcamera        [i_pt*2] : NULL;
                mrcal_point3_t* dq_drframe_here           = dq_drframe         ? &dq_drframe         [i_pt*2] : NULL;
                mrcal_point3_t* dq_dtframe_here           = dq_dtframe         ? &dq_dtframe         [i_pt*2] : NULL;
                mrcal_calobject_warp_t* dq_dcalobject_warp_here = dq_dcalobject_warp ? &dq_dcalobject_warp [i_pt*2] : NULL;

                mrcal_point3_t dq_dtcamera_here_dummy[2];
                mrcal_point3_t dq_dtframe_here_dummy [2];
                if(dq_dcalobject_warp)
                {
                    // I need all translation gradients to be available to
                    // compute the calobject_warp gradients (see the end of the
                    // _project_point() function above). So I compute those even
                    // if the caller didn't ask for them
                    if(!dq_dtcamera_here) dq_dtcamera_here = dq_dtcamera_here_dummy;
                    if(!dq_dtframe_here)  dq_dtframe_here  = dq_dtframe_here_dummy;
                }

                _project_point(&q[i_pt],
                               p_dq_dfxy ? &p_dq_dfxy[i_pt] : NULL,
                               p_dq_dintrinsics_nocore ? &p_dq_dintrinsics_nocore[2*(Nintrinsics-4)*i_pt] : NULL,
                               gradient_sparse_meta ? &gradient_sparse_meta->pool[i_pt*runlen*2] : NULL,
                               runlen,
                               dq_drcamera_here, dq_dtcamera_here, dq_drframe_here, dq_dtframe_here, dq_dcalobject_warp_here,
                               &dq_dintrinsics_pool_int,
                               &p,
                               intrinsics, lensmodel,
                               &dpt_refz_dwarp,
                               camera_at_identity, Rj,
                               Nintrinsics,
                               precomputed,
                               dp_drc,dp_dtc,dp_drf,dp_dtf);
                i_pt++;
            }
    }
}

// NOT A PART OF THE EXTERNAL API. This is exported for the mrcal python wrapper
// only
bool _mrcal_project_internal( // out
                             mrcal_point2_t* q,

                             // Stored as a row-first array of shape (N,2,3). Each
                             // row lives in a mrcal_point3_t
                             mrcal_point3_t* dq_dp,
                             // core, distortions concatenated. Stored as a row-first
                             // array of shape (N,2,Nintrinsics). This is a DENSE array.
                             // High-parameter-count lens models have very sparse
                             // gradients here, and the internal project() function
                             // returns those sparsely. For now THIS function densifies
                             // all of these
                             double*   dq_dintrinsics,

                             // in
                             const mrcal_point3_t* p,
                             int N,
                             const mrcal_lensmodel_t* lensmodel,
                             // core, distortions concatenated
                             const double* intrinsics,

                             int Nintrinsics,
                             const mrcal_projection_precomputed_t* precomputed)
{
    if( dq_dintrinsics == NULL )
    {
        for(int i=0; i<N; i++)
        {
            mrcal_pose_t frame = {.r = {},
                            .t = p[i]};

            // simple non-intrinsics-gradient path
            project( &q[i],
                     NULL, NULL, NULL, NULL, NULL,
                     NULL, NULL, NULL, dq_dp, NULL,

                     // in
                     intrinsics, NULL, &frame, NULL, true,
                     lensmodel, precomputed,
                     0.0, 0,0);
        }
        return true;
    }

    // Some models have sparse gradients, but I'm returning a dense array here.
    // So I init everything to 0
    memset(dq_dintrinsics, 0, N*2*Nintrinsics*sizeof(double));

    int Ngradients = get_Ngradients(lensmodel, Nintrinsics);

    for(int i=0; i<N; i++)
    {
        mrcal_pose_t frame = {.r = {},
                              .t = p[i]};

        // simple non-intrinsics-gradient path
        std::vector<double> dq_dintrinsics_pool_double(Ngradients);
        int    dq_dintrinsics_pool_int   [1];
        double* dq_dfxy               = NULL;
        double* dq_dintrinsics_nocore = NULL;
        gradient_sparse_meta_t gradient_sparse_meta = {}; // init to pacify compiler warning

        project( &q[i],

                 dq_dintrinsics_pool_double.data(),
                 dq_dintrinsics_pool_int,
                 &dq_dfxy, &dq_dintrinsics_nocore, &gradient_sparse_meta,

                 NULL, NULL, NULL, dq_dp, NULL,

                 // in
                 intrinsics, NULL, &frame, NULL, true,
                 lensmodel, precomputed,
                 0.0, 0,0);

        int Ncore = 0;
        if(dq_dfxy != NULL)
        {
            Ncore = 4;

            // fxy. off-diagonal elements are 0
            dq_dintrinsics[0*Nintrinsics + 0] = dq_dfxy[0];
            dq_dintrinsics[0*Nintrinsics + 1] = 0.0;
            dq_dintrinsics[1*Nintrinsics + 0] = 0.0;
            dq_dintrinsics[1*Nintrinsics + 1] = dq_dfxy[1];

            // cxy. Identity
            dq_dintrinsics[0*Nintrinsics + 2] = 1.0;
            dq_dintrinsics[0*Nintrinsics + 3] = 0.0;
            dq_dintrinsics[1*Nintrinsics + 2] = 0.0;
            dq_dintrinsics[1*Nintrinsics + 3] = 1.0;
        }
        if( dq_dintrinsics_nocore != NULL )
        {
            for(int i_xy=0; i_xy<2; i_xy++)
                memcpy(&dq_dintrinsics[i_xy*Nintrinsics + Ncore],
                       &dq_dintrinsics_nocore[i_xy*(Nintrinsics-Ncore)],
                       (Nintrinsics-Ncore)*sizeof(double));
        }
        if(gradient_sparse_meta.pool != NULL)
        {
            // u = stereographic(p)
            // q = (u + deltau(u)) * f + c
            //
            // Intrinsics:
            //   dq/diii = f ddeltau/diii
            //
            // ddeltau/diii = flatten(ABCDx[0..3] * ABCDy[0..3])

            const int     ivar0 = dq_dintrinsics_pool_int[0];
            const int     len   = gradient_sparse_meta.run_side_length;

            const double* ABCDx = &gradient_sparse_meta.pool[0];
            const double* ABCDy = &gradient_sparse_meta.pool[len];

            const int ivar_stridey = gradient_sparse_meta.ivar_stridey;
            const double* fxy = &intrinsics[0];
            for(int i_xy=0; i_xy<2; i_xy++)
                for(int iy=0; iy<len; iy++)
                    for(int ix=0; ix<len; ix++)
                    {
                        int ivar = ivar0 + ivar_stridey*iy + ix*2 + i_xy;
                        dq_dintrinsics[ivar + i_xy*Nintrinsics] =
                            ABCDx[ix]*ABCDy[iy]*fxy[i_xy];
                    }
        }

        // advance
        dq_dintrinsics = &dq_dintrinsics[2*Nintrinsics];
        if(dq_dp != NULL)
            dq_dp = &dq_dp[2];
    }
    return true;
}

// External interface to the internal project() function. The internal function
// is more general (supports geometric transformations prior to projection, and
// supports chessboards). dq_dintrinsics and/or dq_dp are allowed to be NULL if
// we're not interested in gradients.
//
// Projecting out-of-bounds points (beyond the field of view) returns undefined
// values. Generally things remain continuous even as we move off the imager
// domain. Pinhole-like projections will work normally if projecting a point
// behind the camera. Splined projections clamp to the nearest spline segment:
// the projection will fly off to infinity quickly since we're extrapolating a
// polynomial, but the function will remain continuous.
bool mrcal_project( // out
                   mrcal_point2_t* q,

                   // Stored as a row-first array of shape (N,2,3). Each row
                   // lives in a mrcal_point3_t.  May be NULL
                   mrcal_point3_t* dq_dp,

                   // core, distortions concatenated. Stored as a row-first
                   // array of shape (N,2,Nintrinsics). This is a DENSE array.
                   // High-parameter-count lens models have very sparse
                   // gradients here, and the internal project() function
                   // returns those sparsely. For now THIS function densifies
                   // all of these. May be NULL
                   double*   dq_dintrinsics,

                   // in
                   const mrcal_point3_t* p,
                   int N,
                   const mrcal_lensmodel_t* lensmodel,
                   // core, distortions concatenated
                   const double* intrinsics)
{
    // The outer logic (outside the loop-over-N-points) is duplicated in
    // mrcal_project() and in the python wrapper definition in _project() and
    // _project_withgrad() in mrcal-genpywrap.py. Please keep them in sync

    if(dq_dintrinsics != NULL || dq_dp != NULL)
    {
        mrcal_lensmodel_metadata_t meta = mrcal_lensmodel_metadata(lensmodel);
        if(!meta.has_gradients)
        {
            MSG("mrcal_project(lensmodel='%s') cannot return gradients; this is not yet implemented",
                mrcal_lensmodel_name_unconfigured(lensmodel));
            return false;
        }
    }

    int Nintrinsics = mrcal_lensmodel_num_params(lensmodel);

    // Special-case for opencv/pinhole and projection-only. cvProjectPoints2 and
    // project() have a lot of overhead apparently, and calling either in a loop
    // is very slow. I can call it once, and use its fast internal loop,
    // however. This special case does the same thing, but much faster.
    if(dq_dintrinsics == NULL && dq_dp == NULL &&
       (MRCAL_LENSMODEL_IS_OPENCV(lensmodel->type) ||
        lensmodel->type == MRCAL_LENSMODEL_PINHOLE))
    {
        _mrcal_project_internal_opencv( q, NULL,NULL,
                                        p, N, intrinsics, Nintrinsics);
        return true;
    }

    mrcal_projection_precomputed_t precomputed;
    _mrcal_precompute_lensmodel_data(&precomputed, lensmodel);

    return
        _mrcal_project_internal(q, dq_dp, dq_dintrinsics,
                                p, N, lensmodel, intrinsics,
                                Nintrinsics, &precomputed);
}


// Maps a set of distorted 2D imager points q to a 3D vector in camera
// coordinates that produced these pixel observations. The 3D vector is defined
// up-to-length. The returned vectors v are not normalized, and may have any
// length.
//
// This is the "reverse" direction, so an iterative nonlinear optimization is
// performed internally to compute this result. This is much slower than
// mrcal_project. For OpenCV distortions specifically, OpenCV has
// cvUndistortPoints() (and cv2.undistortPoints()), but these are inaccurate:
// https://github.com/opencv/opencv/issues/8811
bool mrcal_unproject( // out
                     mrcal_point3_t* out,

                     // in
                     const mrcal_point2_t* q,
                     int N,
                     const mrcal_lensmodel_t* lensmodel,
                     // core, distortions concatenated
                     const double* intrinsics)
{

    mrcal_lensmodel_metadata_t meta = mrcal_lensmodel_metadata(lensmodel);
    if(!meta.has_gradients)
    {
        MSG("mrcal_unproject(lensmodel='%s') is not yet implemented: we need gradients",
            mrcal_lensmodel_name_unconfigured(lensmodel));
        return false;
    }

    mrcal_projection_precomputed_t precomputed;
    _mrcal_precompute_lensmodel_data(&precomputed, lensmodel);

    return _mrcal_unproject_internal(out, q, N, lensmodel, intrinsics, &precomputed);
}

typedef struct
{
    const mrcal_lensmodel_t* lensmodel;
    // core, distortions concatenated
    const double* intrinsics;
    const mrcal_projection_precomputed_t* precomputed;
    const mrcal_point2_t* q;
} _unproject_callback_cookie_t;
static
void _unproject_callback(const double*   u,
                         double*         x,
                         double*         J,
                         void*           _cookie)
{
    _unproject_callback_cookie_t* cookie = (_unproject_callback_cookie_t*)_cookie;

    // u is the constant-fxy-cxy 2D stereographic
    // projection of the hypothesis v. I unproject it stereographically,
    // and project it using the actual model
    mrcal_point2_t dv_du[3];
    mrcal_pose_t frame = {};
    mrcal_unproject_stereographic( &frame.t, dv_du,
                                   (mrcal_point2_t*)u, 1,
                                   cookie->intrinsics );

    mrcal_point3_t dq_dtframe[2];
    mrcal_point2_t q_hypothesis;
    project( &q_hypothesis,
             NULL,NULL,NULL,NULL,NULL,
             NULL, NULL, NULL, dq_dtframe,
             NULL,

             // in
             cookie->intrinsics,
             NULL,
             &frame,
             NULL,
             true,
             cookie->lensmodel, cookie->precomputed,
             0.0, 0,0);
    x[0] = q_hypothesis.x - cookie->q->x;
    x[1] = q_hypothesis.y - cookie->q->y;
    J[0*2 + 0] =
        dq_dtframe[0].x*dv_du[0].x +
        dq_dtframe[0].y*dv_du[1].x +
        dq_dtframe[0].z*dv_du[2].x;
    J[0*2 + 1] =
        dq_dtframe[0].x*dv_du[0].y +
        dq_dtframe[0].y*dv_du[1].y +
        dq_dtframe[0].z*dv_du[2].y;
    J[1*2 + 0] =
        dq_dtframe[1].x*dv_du[0].x +
        dq_dtframe[1].y*dv_du[1].x +
        dq_dtframe[1].z*dv_du[2].x;
    J[1*2 + 1] =
        dq_dtframe[1].x*dv_du[0].y +
        dq_dtframe[1].y*dv_du[1].y +
        dq_dtframe[1].z*dv_du[2].y;
}

// NOT A PART OF THE EXTERNAL API. This is exported for the mrcal python wrapper
// only
bool _mrcal_unproject_internal( // out
                               mrcal_point3_t* out,

                               // in
                               const mrcal_point2_t* q,
                               int N,
                               const mrcal_lensmodel_t* lensmodel,
                               // core, distortions concatenated
                               const double* intrinsics,
                               const mrcal_projection_precomputed_t* precomputed)
{
    // easy special-cases
    if( lensmodel->type == MRCAL_LENSMODEL_PINHOLE )
    {
        mrcal_unproject_pinhole(out, NULL, q, N, intrinsics);
        return true;
    }
    if( lensmodel->type == MRCAL_LENSMODEL_STEREOGRAPHIC )
    {
        mrcal_unproject_stereographic(out, NULL, q, N, intrinsics);
        return true;
    }
    if( lensmodel->type == MRCAL_LENSMODEL_LONLAT )
    {
        mrcal_unproject_lonlat(out, NULL, q, N, intrinsics);
        return true;
    }
    if( lensmodel->type == MRCAL_LENSMODEL_LATLON )
    {
        mrcal_unproject_latlon(out, NULL, q, N, intrinsics);
        return true;
    }

    if( lensmodel->type == MRCAL_LENSMODEL_CAHVORE )
    {
        const int Nintrinsics  = mrcal_lensmodel_num_params(lensmodel);

        for(int i = Nintrinsics-3; i<Nintrinsics; i++)
            if(intrinsics[i] != 0.)
            {
                MSG("unproject() currently only works with a central projection. So I cannot unproject(CAHVORE,E!=0). Please set E=0 to centralize this model");
                return false;
            }
    }

    // I optimize in the space of the stereographic projection. This is a 2D
    // space with a direct mapping to/from observation vectors with a single
    // singularity directly behind the camera. The allows me to run an
    // unconstrained optimization here
    for(int i=0; i<N; i++)
    {
        const double fx = intrinsics[0];
        const double fy = intrinsics[1];
        const double cx = intrinsics[2];
        const double cy = intrinsics[3];

        // MSG("init. q=(%g,%g)", q[i].x, q[i].y);

        // initial estimate: pinhole projection
        const mrcal_point3_t v = {.x = (q[i].x-cx)/fx,
                                  .y = (q[i].y-cy)/fy,
                                  .z = 1.};
        mrcal_project_stereographic( (mrcal_point2_t*)out->xyz, NULL,
                                     &v,
                                     1,
                                     intrinsics );
        // MSG("init. out->xyz[]=(%g,%g)", out->x, out->y);


        dogleg_parameters2_t dogleg_parameters;
        dogleg_getDefaultParameters(&dogleg_parameters);
        dogleg_parameters.dogleg_debug = 0;

        _unproject_callback_cookie_t cookie =
            { .lensmodel   = lensmodel,
              .intrinsics  = intrinsics,
              .precomputed = precomputed,
              .q           = &q[i] };
        double norm2x =
            dogleg_optimize_dense2(out->xyz, 2, 2, _unproject_callback, (void*)&cookie,
                                   &dogleg_parameters,
                                   NULL);
        //This needs to be precise; if it isn't, I barf. Shouldn't happen
        //very often

        static bool already_complained = false;
        // MSG("norm2x = %g", norm2x);
        if(norm2x/2.0 > 1e-4)
        {
            if(!already_complained)
            {
                // MSG("WARNING: I wasn't able to precisely compute some points. norm2x=%f. Returning nan for those. Will complain just once",
                //     norm2x);
                already_complained = true;
            }
            double nan = strtod("NAN", NULL);
            out->xyz[0] = nan;
            out->xyz[1] = nan;
        }
        else
        {
            // out[0,1] is the stereographic representation of the observation
            // vector using idealized fx,fy,cx,cy. This is already the right
            // thing if we're reporting in 2d. Otherwise I need to unproject

            // This is the normal no-error path
            mrcal_unproject_stereographic((mrcal_point3_t*)out, NULL,
                                          (mrcal_point2_t*)out, 1,
                                          intrinsics);
            if(!model_supports_projection_behind_camera(lensmodel) && out->xyz[2] < 0.0)
            {
                out->xyz[0] *= -1.0;
                out->xyz[1] *= -1.0;
                out->xyz[2] *= -1.0;
            }
        }

        // Advance to the next point. Error or not
        out++;
    }
    return true;
}

// The following functions define/use the layout of the state vector. In general
// I do:
//
//   intrinsics_cam0
//   intrinsics_cam1
//   intrinsics_cam2
//   ...
//   extrinsics_cam1
//   extrinsics_cam2
//   extrinsics_cam3
//   ...
//   frame0
//   frame1
//   frame2
//   ....
//   calobject_warp0
//   calobject_warp1
//   ...

// From real values to unit-scale values. Optimizer sees unit-scale values
static int pack_solver_state_intrinsics( // out
                                         double* b, // subset based on problem_selections

                                         // in
                                         const double* intrinsics, // ALL variables. Not a subset
                                         const mrcal_lensmodel_t* lensmodel,
                                         mrcal_problem_selections_t problem_selections,
                                         int Ncameras_intrinsics )
{
    int i_state = 0;
    const int Nintrinsics  = mrcal_lensmodel_num_params(lensmodel);
    const int Ncore        = modelHasCore_fxfycxcy(lensmodel) ? 4 : 0;
    const int Ndistortions = Nintrinsics - Ncore;

    for(int icam_intrinsics=0; icam_intrinsics < Ncameras_intrinsics; icam_intrinsics++)
    {
        if( problem_selections.do_optimize_intrinsics_core && Ncore )
        {
            const mrcal_intrinsics_core_t* intrinsics_core = (const mrcal_intrinsics_core_t*)intrinsics;
            b[i_state++] = intrinsics_core->focal_xy [0] / SCALE_INTRINSICS_FOCAL_LENGTH;
            b[i_state++] = intrinsics_core->focal_xy [1] / SCALE_INTRINSICS_FOCAL_LENGTH;
            b[i_state++] = intrinsics_core->center_xy[0] / SCALE_INTRINSICS_CENTER_PIXEL;
            b[i_state++] = intrinsics_core->center_xy[1] / SCALE_INTRINSICS_CENTER_PIXEL;
        }

        if( problem_selections.do_optimize_intrinsics_distortions )

            for(int i = 0; i<Ndistortions; i++)
                b[i_state++] = intrinsics[Ncore + i] / SCALE_DISTORTION;

        intrinsics = &intrinsics[Nintrinsics];
    }
    return i_state;
}
// It is ugly to have this as separate from pack_solver_state_intrinsics(), but
// I am tired. THIS function uses only intrinsic arrays that respect the skipped
// optimization variables in problem_selections. And this function works inline
static int pack_solver_state_intrinsics_subset_to_subset( // out,in
                                         double* b, // subset based on problem_selections

                                         // in
                                         const mrcal_lensmodel_t* lensmodel,
                                         mrcal_problem_selections_t problem_selections,
                                         int Ncameras_intrinsics )
{
    if( !problem_selections.do_optimize_intrinsics_core &&
        !problem_selections.do_optimize_intrinsics_distortions )
        return 0;

    int i_state = 0;
    const int Nintrinsics  = mrcal_lensmodel_num_params(lensmodel);
    const int Ncore        = modelHasCore_fxfycxcy(lensmodel) ? 4 : 0;
    const int Ndistortions = Nintrinsics - Ncore;

    for(int icam_intrinsics=0; icam_intrinsics < Ncameras_intrinsics; icam_intrinsics++)
    {
        if( problem_selections.do_optimize_intrinsics_core && Ncore )
        {
            b[i_state++] /= SCALE_INTRINSICS_FOCAL_LENGTH;
            b[i_state++] /= SCALE_INTRINSICS_FOCAL_LENGTH;
            b[i_state++] /= SCALE_INTRINSICS_CENTER_PIXEL;
            b[i_state++] /= SCALE_INTRINSICS_CENTER_PIXEL;
        }

        if( problem_selections.do_optimize_intrinsics_distortions )
            for(int i = 0; i<Ndistortions; i++)
                b[i_state++] /= SCALE_DISTORTION;
    }
    return i_state;
}
static void pack_solver_state( // out
                              double* b,

                              // in
                              const mrcal_lensmodel_t* lensmodel,
                              const double* intrinsics, // Ncameras_intrinsics of these
                              const mrcal_pose_t*            extrinsics_fromref, // Ncameras_extrinsics of these
                              const mrcal_pose_t*            frames_toref,     // Nframes of these
                              const mrcal_point3_t*          points,     // Npoints of these
                              const mrcal_calobject_warp_t*  calobject_warp, // 1 of these
                              mrcal_problem_selections_t problem_selections,
                              int Ncameras_intrinsics, int Ncameras_extrinsics, int Nframes,
                              int Npoints_variable,
                              int Nobservations_board,
                              int Nstate_ref)
{
    int i_state = 0;

    i_state += pack_solver_state_intrinsics( b, intrinsics,
                                             lensmodel, problem_selections,
                                             Ncameras_intrinsics );

    if( problem_selections.do_optimize_extrinsics )
        for(int icam_extrinsics=0; icam_extrinsics < Ncameras_extrinsics; icam_extrinsics++)
        {
            b[i_state++] = extrinsics_fromref[icam_extrinsics].r.xyz[0] / SCALE_ROTATION_CAMERA;
            b[i_state++] = extrinsics_fromref[icam_extrinsics].r.xyz[1] / SCALE_ROTATION_CAMERA;
            b[i_state++] = extrinsics_fromref[icam_extrinsics].r.xyz[2] / SCALE_ROTATION_CAMERA;

            b[i_state++] = extrinsics_fromref[icam_extrinsics].t.xyz[0] / SCALE_TRANSLATION_CAMERA;
            b[i_state++] = extrinsics_fromref[icam_extrinsics].t.xyz[1] / SCALE_TRANSLATION_CAMERA;
            b[i_state++] = extrinsics_fromref[icam_extrinsics].t.xyz[2] / SCALE_TRANSLATION_CAMERA;
        }

    if( problem_selections.do_optimize_frames )
    {
        for(int iframe = 0; iframe < Nframes; iframe++)
        {
            b[i_state++] = frames_toref[iframe].r.xyz[0] / SCALE_ROTATION_FRAME;
            b[i_state++] = frames_toref[iframe].r.xyz[1] / SCALE_ROTATION_FRAME;
            b[i_state++] = frames_toref[iframe].r.xyz[2] / SCALE_ROTATION_FRAME;

            b[i_state++] = frames_toref[iframe].t.xyz[0] / SCALE_TRANSLATION_FRAME;
            b[i_state++] = frames_toref[iframe].t.xyz[1] / SCALE_TRANSLATION_FRAME;
            b[i_state++] = frames_toref[iframe].t.xyz[2] / SCALE_TRANSLATION_FRAME;
        }

        for(int i_point = 0; i_point < Npoints_variable; i_point++)
        {
            b[i_state++] = points[i_point].xyz[0] / SCALE_POSITION_POINT;
            b[i_state++] = points[i_point].xyz[1] / SCALE_POSITION_POINT;
            b[i_state++] = points[i_point].xyz[2] / SCALE_POSITION_POINT;
        }
    }

    if( has_calobject_warp(problem_selections,Nobservations_board) )
    {
        b[i_state++] = calobject_warp->x2 / SCALE_CALOBJECT_WARP;
        b[i_state++] = calobject_warp->y2 / SCALE_CALOBJECT_WARP;
    }

    assert(i_state == Nstate_ref);
}

// Same as above, but packs/unpacks a vector instead of structures
void mrcal_pack_solver_state_vector( // out, in
                                     double* b, // FULL state on input, unitless
                                                // state on output

                                     // in
                                     int Ncameras_intrinsics, int Ncameras_extrinsics,
                                     int Nframes,
                                     int Npoints, int Npoints_fixed, int Nobservations_board,
                                     mrcal_problem_selections_t problem_selections,
                                     const mrcal_lensmodel_t* lensmodel)
{
    int Npoints_variable = Npoints - Npoints_fixed;

    int i_state = 0;

    i_state += pack_solver_state_intrinsics_subset_to_subset( b,
                                             lensmodel, problem_selections,
                                             Ncameras_intrinsics );

    if( problem_selections.do_optimize_extrinsics )
        for(int icam_extrinsics=0; icam_extrinsics < Ncameras_extrinsics; icam_extrinsics++)
        {
            mrcal_pose_t* extrinsics_fromref = (mrcal_pose_t*)(&b[i_state]);

            b[i_state++] = extrinsics_fromref->r.xyz[0] / SCALE_ROTATION_CAMERA;
            b[i_state++] = extrinsics_fromref->r.xyz[1] / SCALE_ROTATION_CAMERA;
            b[i_state++] = extrinsics_fromref->r.xyz[2] / SCALE_ROTATION_CAMERA;

            b[i_state++] = extrinsics_fromref->t.xyz[0] / SCALE_TRANSLATION_CAMERA;
            b[i_state++] = extrinsics_fromref->t.xyz[1] / SCALE_TRANSLATION_CAMERA;
            b[i_state++] = extrinsics_fromref->t.xyz[2] / SCALE_TRANSLATION_CAMERA;
        }

    if( problem_selections.do_optimize_frames )
    {
        for(int iframe = 0; iframe < Nframes; iframe++)
        {
            mrcal_pose_t* frames_toref = (mrcal_pose_t*)(&b[i_state]);
            b[i_state++] = frames_toref->r.xyz[0] / SCALE_ROTATION_FRAME;
            b[i_state++] = frames_toref->r.xyz[1] / SCALE_ROTATION_FRAME;
            b[i_state++] = frames_toref->r.xyz[2] / SCALE_ROTATION_FRAME;

            b[i_state++] = frames_toref->t.xyz[0] / SCALE_TRANSLATION_FRAME;
            b[i_state++] = frames_toref->t.xyz[1] / SCALE_TRANSLATION_FRAME;
            b[i_state++] = frames_toref->t.xyz[2] / SCALE_TRANSLATION_FRAME;
        }

        for(int i_point = 0; i_point < Npoints_variable; i_point++)
        {
            mrcal_point3_t* points = (mrcal_point3_t*)(&b[i_state]);
            b[i_state++] = points->xyz[0] / SCALE_POSITION_POINT;
            b[i_state++] = points->xyz[1] / SCALE_POSITION_POINT;
            b[i_state++] = points->xyz[2] / SCALE_POSITION_POINT;
        }
    }

    if( has_calobject_warp(problem_selections,Nobservations_board) )
    {
        mrcal_calobject_warp_t* calobject_warp = (mrcal_calobject_warp_t*)(&b[i_state]);
        b[i_state++] = calobject_warp->x2 / SCALE_CALOBJECT_WARP;
        b[i_state++] = calobject_warp->y2 / SCALE_CALOBJECT_WARP;
    }
}

static int unpack_solver_state_intrinsics( // out

                                           // Ncameras_intrinsics of these
                                           double* intrinsics, // ALL variables. Not a subset.
                                                               // I don't touch the elemnents I'm
                                                               // not optimizing

                                           // in
                                           const double* b, // subset based on problem_selections
                                           const mrcal_lensmodel_t* lensmodel,
                                           mrcal_problem_selections_t problem_selections,
                                           int intrinsics_stride,
                                           int Ncameras_intrinsics )
{
    if( !problem_selections.do_optimize_intrinsics_core &&
        !problem_selections.do_optimize_intrinsics_distortions )
        return 0;

    const int Nintrinsics = mrcal_lensmodel_num_params(lensmodel);
    const int Ncore       = modelHasCore_fxfycxcy(lensmodel) ? 4 : 0;

    int i_state = 0;
    for(int icam_intrinsics=0; icam_intrinsics < Ncameras_intrinsics; icam_intrinsics++)
    {
        if( problem_selections.do_optimize_intrinsics_core && Ncore )
        {
            intrinsics[icam_intrinsics*intrinsics_stride + 0] = b[i_state++] * SCALE_INTRINSICS_FOCAL_LENGTH;
            intrinsics[icam_intrinsics*intrinsics_stride + 1] = b[i_state++] * SCALE_INTRINSICS_FOCAL_LENGTH;
            intrinsics[icam_intrinsics*intrinsics_stride + 2] = b[i_state++] * SCALE_INTRINSICS_CENTER_PIXEL;
            intrinsics[icam_intrinsics*intrinsics_stride + 3] = b[i_state++] * SCALE_INTRINSICS_CENTER_PIXEL;
        }

        if( problem_selections.do_optimize_intrinsics_distortions )
        {
            for(int i = 0; i<Nintrinsics-Ncore; i++)
                intrinsics[icam_intrinsics*intrinsics_stride + Ncore + i] = b[i_state++] * SCALE_DISTORTION;
        }
    }
    return i_state;
}
// It is ugly to have this as separate from unpack_solver_state_intrinsics(),
// but I am tired. THIS function uses only intrinsic arrays that respect the
// skipped optimization variables in problem_selections. And this function works
// inline
static int unpack_solver_state_intrinsics_subset_to_subset( // in,out
                                           double* b, // subset based on problem_selections

                                           // in
                                           const mrcal_lensmodel_t* lensmodel,
                                           mrcal_problem_selections_t problem_selections,
                                           int Ncameras_intrinsics )
{
    if( !problem_selections.do_optimize_intrinsics_core &&
        !problem_selections.do_optimize_intrinsics_distortions )
        return 0;

    int i_state = 0;
    const int Nintrinsics  = mrcal_lensmodel_num_params(lensmodel);
    const int Ncore        = modelHasCore_fxfycxcy(lensmodel) ? 4 : 0;
    const int Ndistortions = Nintrinsics - Ncore;

    for(int icam_intrinsics=0; icam_intrinsics < Ncameras_intrinsics; icam_intrinsics++)
    {
        if( problem_selections.do_optimize_intrinsics_core && Ncore )
        {
            b[i_state++] *= SCALE_INTRINSICS_FOCAL_LENGTH;
            b[i_state++] *= SCALE_INTRINSICS_FOCAL_LENGTH;
            b[i_state++] *= SCALE_INTRINSICS_CENTER_PIXEL;
            b[i_state++] *= SCALE_INTRINSICS_CENTER_PIXEL;
        }

        if( problem_selections.do_optimize_intrinsics_distortions )
            for(int i = 0; i<Ndistortions; i++)
                b[i_state++] *= SCALE_DISTORTION;
    }
    return i_state;
}

static int unpack_solver_state_extrinsics_one(// out
                                              mrcal_pose_t* extrinsic,

                                              // in
                                              const double* b)
{
    int i_state = 0;
    extrinsic->r.xyz[0] = b[i_state++] * SCALE_ROTATION_CAMERA;
    extrinsic->r.xyz[1] = b[i_state++] * SCALE_ROTATION_CAMERA;
    extrinsic->r.xyz[2] = b[i_state++] * SCALE_ROTATION_CAMERA;

    extrinsic->t.xyz[0] = b[i_state++] * SCALE_TRANSLATION_CAMERA;
    extrinsic->t.xyz[1] = b[i_state++] * SCALE_TRANSLATION_CAMERA;
    extrinsic->t.xyz[2] = b[i_state++] * SCALE_TRANSLATION_CAMERA;
    return i_state;
}

static int unpack_solver_state_framert_one(// out
                                           mrcal_pose_t* frame,

                                           // in
                                           const double* b)
{
    int i_state = 0;
    frame->r.xyz[0] = b[i_state++] * SCALE_ROTATION_FRAME;
    frame->r.xyz[1] = b[i_state++] * SCALE_ROTATION_FRAME;
    frame->r.xyz[2] = b[i_state++] * SCALE_ROTATION_FRAME;

    frame->t.xyz[0] = b[i_state++] * SCALE_TRANSLATION_FRAME;
    frame->t.xyz[1] = b[i_state++] * SCALE_TRANSLATION_FRAME;
    frame->t.xyz[2] = b[i_state++] * SCALE_TRANSLATION_FRAME;
    return i_state;

}

static int unpack_solver_state_point_one(// out
                                         mrcal_point3_t* point,

                                         // in
                                         const double* b)
{
    int i_state = 0;
    point->xyz[0] = b[i_state++] * SCALE_POSITION_POINT;
    point->xyz[1] = b[i_state++] * SCALE_POSITION_POINT;
    point->xyz[2] = b[i_state++] * SCALE_POSITION_POINT;
    return i_state;
}

static int unpack_solver_state_calobject_warp(// out
                                              mrcal_calobject_warp_t* calobject_warp,

                                              // in
                                              const double* b)
{
    int i_state = 0;
    calobject_warp->x2 = b[i_state++] * SCALE_CALOBJECT_WARP;
    calobject_warp->y2 = b[i_state++] * SCALE_CALOBJECT_WARP;
    return i_state;
}

// From unit-scale values to real values. Optimizer sees unit-scale values
static void unpack_solver_state( // out

                                 // ALL intrinsics; whether we're optimizing
                                 // them or not. Don't touch the parts of this
                                 // array we're not optimizing
                                 double* intrinsics_all,                 // Ncameras_intrinsics of these

                                 mrcal_pose_t*       extrinsics_fromref, // Ncameras_extrinsics of these
                                 mrcal_pose_t*       frames_toref,       // Nframes of these
                                 mrcal_point3_t*     points,             // Npoints of these
                                 mrcal_calobject_warp_t* calobject_warp, // 1 of these

                                 // in
                                 const double* b,
                                 const mrcal_lensmodel_t* lensmodel,
                                 mrcal_problem_selections_t problem_selections,
                                 int Ncameras_intrinsics, int Ncameras_extrinsics, int Nframes, int Npoints_variable,
                                 int Nobservations_board,
                                 int Nstate_ref)
{
    int i_state = unpack_solver_state_intrinsics(intrinsics_all,
                                                 b, lensmodel, problem_selections,
                                                 mrcal_lensmodel_num_params(lensmodel),
                                                 Ncameras_intrinsics);

    if( problem_selections.do_optimize_extrinsics )
        for(int icam_extrinsics=0; icam_extrinsics < Ncameras_extrinsics; icam_extrinsics++)
            i_state += unpack_solver_state_extrinsics_one( &extrinsics_fromref[icam_extrinsics], &b[i_state] );

    if( problem_selections.do_optimize_frames )
    {
        for(int iframe = 0; iframe < Nframes; iframe++)
            i_state += unpack_solver_state_framert_one( &frames_toref[iframe], &b[i_state] );
        for(int i_point = 0; i_point < Npoints_variable; i_point++)
            i_state += unpack_solver_state_point_one( &points[i_point], &b[i_state] );
    }

    if( has_calobject_warp(problem_selections,Nobservations_board) )
        i_state += unpack_solver_state_calobject_warp(calobject_warp, &b[i_state]);

    assert(i_state == Nstate_ref);
}
// Same as above, but packs/unpacks a vector instead of structures
void mrcal_unpack_solver_state_vector( // out, in
                                       double* b, // unitless state on input,
                                                  // scaled, meaningful state on
                                                  // output

                                       // in
                                       int Ncameras_intrinsics, int Ncameras_extrinsics,
                                       int Nframes,
                                       int Npoints, int Npoints_fixed, int Nobservations_board,
                                       mrcal_problem_selections_t problem_selections,
                                       const mrcal_lensmodel_t* lensmodel)
{
    int Npoints_variable = Npoints - Npoints_fixed;

    int i_state =
        unpack_solver_state_intrinsics_subset_to_subset(b,
                                                        lensmodel, problem_selections,
                                                        Ncameras_intrinsics);

    if( problem_selections.do_optimize_extrinsics )
    {
        mrcal_pose_t* extrinsics_fromref = (mrcal_pose_t*)(&b[i_state]);
        for(int icam_extrinsics=0; icam_extrinsics < Ncameras_extrinsics; icam_extrinsics++)
            i_state += unpack_solver_state_extrinsics_one( &extrinsics_fromref[icam_extrinsics], &b[i_state] );
    }

    if( problem_selections.do_optimize_frames )
    {
        mrcal_pose_t* frames_toref = (mrcal_pose_t*)(&b[i_state]);
        for(int iframe = 0; iframe < Nframes; iframe++)
            i_state += unpack_solver_state_framert_one( &frames_toref[iframe], &b[i_state] );
        mrcal_point3_t* points = (mrcal_point3_t*)(&b[i_state]);
        for(int i_point = 0; i_point < Npoints_variable; i_point++)
            i_state += unpack_solver_state_point_one( &points[i_point], &b[i_state] );
    }
    if( has_calobject_warp(problem_selections,Nobservations_board) )
    {
        mrcal_calobject_warp_t* calobject_warp = (mrcal_calobject_warp_t*)(&b[i_state]);
        i_state += unpack_solver_state_calobject_warp(calobject_warp, &b[i_state]);
    }
}

int mrcal_state_index_intrinsics(int icam_intrinsics,
                                 int Ncameras_intrinsics, int Ncameras_extrinsics,
                                 int Nframes,
                                 int Npoints, int Npoints_fixed, int Nobservations_board,
                                 mrcal_problem_selections_t problem_selections,
                                 const mrcal_lensmodel_t* lensmodel)
{
    if(Ncameras_intrinsics <= 0)
        return -1;
    int Nintrinsics = mrcal_num_intrinsics_optimization_params(problem_selections, lensmodel);
    if(Nintrinsics <= 0)
        return -1;
    if(!(0 <= icam_intrinsics && icam_intrinsics < Ncameras_intrinsics))
        return -1;
    return icam_intrinsics * Nintrinsics;
}

int mrcal_num_states_intrinsics(int Ncameras_intrinsics,
                                mrcal_problem_selections_t problem_selections,
                                const mrcal_lensmodel_t* lensmodel)
{
    return
        Ncameras_intrinsics *
        mrcal_num_intrinsics_optimization_params(problem_selections, lensmodel);
}

int mrcal_state_index_extrinsics(int icam_extrinsics,
                                 int Ncameras_intrinsics, int Ncameras_extrinsics,
                                 int Nframes,
                                 int Npoints, int Npoints_fixed, int Nobservations_board,
                                 mrcal_problem_selections_t problem_selections,
                                 const mrcal_lensmodel_t* lensmodel)
{
    if(Ncameras_extrinsics <= 0)
        return -1;
    if(!problem_selections.do_optimize_extrinsics)
        return -1;
    if(!(0 <= icam_extrinsics && icam_extrinsics < Ncameras_extrinsics))
        return -1;

    return
        mrcal_num_states_intrinsics(Ncameras_intrinsics,
                                    problem_selections,
                                    lensmodel) +
        (icam_extrinsics*6);
}

int mrcal_num_states_extrinsics(int Ncameras_extrinsics,
                                mrcal_problem_selections_t problem_selections)
{
    return problem_selections.do_optimize_extrinsics ? (6*Ncameras_extrinsics) : 0;
}

int mrcal_state_index_frames(int iframe,
                             int Ncameras_intrinsics, int Ncameras_extrinsics,
                             int Nframes,
                             int Npoints, int Npoints_fixed, int Nobservations_board,
                             mrcal_problem_selections_t problem_selections,
                             const mrcal_lensmodel_t* lensmodel)
{
    if(Nframes <= 0)
        return -1;
    if(!problem_selections.do_optimize_frames)
        return -1;
    if(!(0 <= iframe && iframe < Nframes))
        return -1;

    return
        mrcal_num_states_intrinsics(Ncameras_intrinsics,
                                    problem_selections,
                                    lensmodel) +
        mrcal_num_states_extrinsics(Ncameras_extrinsics,
                                    problem_selections) +
        (iframe*6);
}

int mrcal_num_states_frames(int Nframes,
                            mrcal_problem_selections_t problem_selections)
{
    return problem_selections.do_optimize_frames ? (6*Nframes) : 0;
}

int mrcal_state_index_points(int i_point,
                             int Ncameras_intrinsics, int Ncameras_extrinsics,
                             int Nframes,
                             int Npoints, int Npoints_fixed, int Nobservations_board,
                             mrcal_problem_selections_t problem_selections,
                             const mrcal_lensmodel_t* lensmodel)
{
    int Npoints_variable = Npoints - Npoints_fixed;

    if(Npoints_variable <= 0)
        return -1;
    if(!problem_selections.do_optimize_frames)
        return -1;
    if(!(0 <= i_point && i_point < Npoints_variable))
        return -1;

    return
        mrcal_num_states_intrinsics(Ncameras_intrinsics,
                                    problem_selections,
                                    lensmodel) +
        mrcal_num_states_extrinsics(Ncameras_extrinsics,
                                    problem_selections) +
        mrcal_num_states_frames    (Nframes,
                                    problem_selections) +
        (i_point*3);
}

int mrcal_num_states_points(int Npoints, int Npoints_fixed,
                            mrcal_problem_selections_t problem_selections)
{
    int Npoints_variable = Npoints - Npoints_fixed;
    return problem_selections.do_optimize_frames ? (Npoints_variable*3) : 0;
}

int mrcal_state_index_calobject_warp(int Ncameras_intrinsics, int Ncameras_extrinsics,
                                     int Nframes,
                                     int Npoints, int Npoints_fixed, int Nobservations_board,
                                     mrcal_problem_selections_t problem_selections,
                                     const mrcal_lensmodel_t* lensmodel)
{
    if(!has_calobject_warp(problem_selections,Nobservations_board))
        return -1;

    return
        mrcal_num_states_intrinsics(Ncameras_intrinsics,
                                    problem_selections,
                                    lensmodel) +
        mrcal_num_states_extrinsics(Ncameras_extrinsics,
                                    problem_selections) +
        mrcal_num_states_frames    (Nframes,
                                    problem_selections) +
        mrcal_num_states_points    (Npoints, Npoints_fixed,
                                    problem_selections);
}

int mrcal_num_states_calobject_warp(mrcal_problem_selections_t problem_selections,
                                    int Nobservations_board)
{
    if(has_calobject_warp(problem_selections,Nobservations_board))
        return MRCAL_NSTATE_CALOBJECT_WARP;
    return 0;
}

// Reports the icam_extrinsics corresponding to a given icam_intrinsics.
//
// If we're solving a vanilla calibration problem (stationary cameras observing
// a moving calibration object), each camera has a unique intrinsics index and a
// unique extrinsics index. This function reports the latter, given the former.
//
// On success, the result is written to *icam_extrinsics, and we return true. If
// the given camera is at the reference coordinate system, it has no extrinsics,
// and we report -1.
//
// If we have moving cameras (NOT a vanilla calibration problem), there isn't a
// single icam_extrinsics for a given icam_intrinsics, and we report an error by
// returning false
static
bool _corresponding_icam_extrinsics__check( const mrcal_camera_index_t* icam, int i,
                                            int* icam_map_to_extrinsics,
                                            int* icam_map_to_intrinsics,
                                            const char* what)
{
    int icam_intrinsics = icam->intrinsics;
    int icam_extrinsics = icam->extrinsics;

    if(icam_extrinsics < 0) icam_extrinsics = -1;

    if(icam_map_to_intrinsics[icam_extrinsics+1] == -100)
        icam_map_to_intrinsics[icam_extrinsics+1] = icam_intrinsics;
    else if(icam_map_to_intrinsics[icam_extrinsics+1] != icam_intrinsics)
    {
        MSG("Cannot compute icam_extrinsics. I don't have a vanilla calibration problem: %s observation %d has icam_intrinsics,icam_extrinsics %d,%d while I saw %d,%d previously",
            what, i,
            icam_map_to_intrinsics[icam_extrinsics+1], icam_extrinsics,
            icam_intrinsics, icam_extrinsics);
        return false;
    }

    if(icam_map_to_extrinsics[icam_intrinsics] == -100)
        icam_map_to_extrinsics[icam_intrinsics] = icam_extrinsics;
    else if(icam_map_to_extrinsics[icam_intrinsics] != icam_extrinsics)
    {
        MSG("Cannot compute icam_extrinsics. I don't have a vanilla calibration problem: %s observation %d has icam_intrinsics,icam_extrinsics %d,%d while I saw %d,%d previously",
            what, i,
            icam_intrinsics, icam_map_to_extrinsics[icam_intrinsics],
            icam_intrinsics, icam_extrinsics);
        return false;
    }
    return true;
}
bool mrcal_corresponding_icam_extrinsics(// out
                                         int* icam_extrinsics,

                                         // in
                                         int icam_intrinsics,
                                         int Ncameras_intrinsics,
                                         int Ncameras_extrinsics,
                                         int Nobservations_board,
                                         const mrcal_observation_board_t* observations_board,
                                         int Nobservations_point,
                                         const mrcal_observation_point_t* observations_point)
{
    if( !(Ncameras_intrinsics == Ncameras_extrinsics ||
          Ncameras_intrinsics == Ncameras_extrinsics+1 ) )
    {
        MSG("Cannot compute icam_extrinsics. I don't have a vanilla calibration problem (stationary cameras, cam0 is reference)");
        return false;
    }

    std::vector<int> icam_map_to_extrinsics(Ncameras_intrinsics);
    std::vector<int> icam_map_to_intrinsics(Ncameras_extrinsics+1);
    for(int i=0; i<Ncameras_intrinsics;   i++) icam_map_to_extrinsics[i] = -100;
    for(int i=0; i<Ncameras_extrinsics+1; i++) icam_map_to_intrinsics[i] = -100;

    for(int i=0; i<Nobservations_board; i++)
        if(!_corresponding_icam_extrinsics__check( &observations_board[i].icam, i,
                                                   icam_map_to_extrinsics.data(),
                                                   icam_map_to_intrinsics.data(),
                                                   "board"))
            return false;
    for(int i=0; i<Nobservations_point; i++)
        if(!_corresponding_icam_extrinsics__check( &observations_point[i].icam, i,
                                                   icam_map_to_extrinsics.data(),
                                                   icam_map_to_intrinsics.data(),
                                                   "point"))
            return false;

    *icam_extrinsics = icam_map_to_extrinsics[icam_intrinsics];

    return true;
}

// Doing this myself instead of hooking into the logic in libdogleg for now.
// Bring back the fancy libdogleg logic once everything stabilizes
//
// Note: this does NOT process discrete points because big errors in these
// observations don't cause clear outliers in the measurement vector, from what
// I can see in test-sfm-fixed-points. If we can find a way to tie observation
// errors directly to measurements, please add that here
static
bool markOutliers(// output, input

                  // the weight stored in each mrcal_point3_t.z indicates outlierness
                  // on entry AND on exit. Outliers have weight < 0.0
                  mrcal_point3_t* observations_board_pool,

                  // output
                  int* Noutliers_board,
                  int* Noutliers_triangulated_point,

                  // input

                  // for diagnostics only
                  const mrcal_observation_board_t* observations_board,

                  int Nobservations_board,
                  int Nobservations_point,

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: not const because this is where the outlier bit lives currently"
#endif
                  mrcal_observation_point_triangulated_t* observations_point_triangulated,
                  int Nobservations_point_triangulated,

                  int calibration_object_width_n,
                  int calibration_object_height_n,

                  const double* x_measurements,
                  const mrcal_pose_t* rt_extrinsics_fromref, // Ncameras_extrinsics of these
                  bool verbose)
{
    // I define an outlier as a feature that's > k stdevs past the mean. I make
    // a broad assumption that the error distribution is normally-distributed,
    // with mean 0. This is reasonable because this function is applied after
    // running the optimization.
    //
    // The threshold is based on the stdev of my observed residuals
    //
    // I have two separate thresholds. If any measurements are worse than the
    // higher threshold, then I will need to reoptimize, so I throw out some
    // extra points: all points worse than the lower threshold. This serves to
    // reduce the required re-optimizations

    const double k0 = 4.0;
    const double k1 = 5.0;

    *Noutliers_board   = 0;
    int Ninliers_board = 0;

    *Noutliers_triangulated_point   = 0;
    int Ninliers_triangulated_point = 0;

    const int imeasurement_board0 =
        mrcal_measurement_index_boards(0,
                                       Nobservations_board,
                                       Nobservations_point,
                                       calibration_object_width_n,
                                       calibration_object_height_n);
    const int imeasurement_point_triangulated0 =
        mrcal_measurement_index_points_triangulated(0,
                                                    Nobservations_board,
                                                    Nobservations_point,

                                                    observations_point_triangulated,
                                                    Nobservations_point_triangulated,

                                                    calibration_object_width_n,
                                                    calibration_object_height_n);


    // just in case
    if(Nobservations_point_triangulated <= 0)
    {
        Nobservations_point_triangulated = 0;
        observations_point_triangulated = NULL;
    }

    const double* x_boards =
        &x_measurements[ imeasurement_board0 ];
    const double* x_point_triangulated =
        &x_measurements[ imeasurement_point_triangulated0 ];

#define LOOP_BOARD_OBSERVATION(extra_while_condition)   \
    for(int i_observation_board=0, i_pt_board = 0;      \
        i_observation_board<Nobservations_board && extra_while_condition; \
        i_observation_board++)

#define LOOP_BOARD_FEATURE()                                            \
        const mrcal_observation_board_t* observation = &observations_board[i_observation_board]; \
        const int icam_intrinsics = observation->icam.intrinsics;       \
        for(int i_pt=0;                                                    \
            i_pt < calibration_object_width_n*calibration_object_height_n; \
            i_pt++, i_pt_board++)

#define LOOP_BOARD_FEATURE_HEADER()                                     \
            const mrcal_point3_t* pt_observed = &observations_board_pool[i_pt_board]; \
            double* weight = &observations_board_pool[i_pt_board].z;

#define LOOP_TRIANGULATED_POINT0(extra_while_condition) \
    for(int i0                              = 0,        \
            imeasurement_point_triangulated = 0;        \
        i0 < Nobservations_point_triangulated && extra_while_condition; \
        i0++)

#define LOOP_TRIANGULATED_POINT1()                      \
        mrcal_observation_point_triangulated_t* pt0 =   \
            &observations_point_triangulated[i0];       \
        if(pt0->last_in_set)                            \
            continue;                                   \
                                                        \
        int i1 = i0+1;

#define LOOP_TRIANGULATED_POINT_HEADER()                        \
            mrcal_observation_point_triangulated_t* pt1 =       \
                &observations_point_triangulated[i1];

#define LOOP_TRIANGULATED_POINT_FOOTER()        \
            imeasurement_point_triangulated++;  \
            if(pt1->last_in_set)                \
                break;                          \
            i1++;





    /////////////// Compute the variance to set the threshold
    bool foundNewOutliers = false;

    double var = 0.0;
    LOOP_BOARD_OBSERVATION(true)
    {
        LOOP_BOARD_FEATURE()
        {
            LOOP_BOARD_FEATURE_HEADER();

            if(*weight <= 0.0)
            {
                (*Noutliers_board)++;
                continue;
            }

            double dx = x_boards[2*i_pt_board + 0];
            double dy = x_boards[2*i_pt_board + 1];
            var += dx*dx + dy*dy;
            Ninliers_board++;
        }
    }

    MSG("I started with %d board outliers", *Noutliers_board);
    int Nmeasurements_outliers_triangulated_start = 0;

    LOOP_TRIANGULATED_POINT0(true)
    {
        LOOP_TRIANGULATED_POINT1();

        const mrcal_point3_t* v0 = &pt0->px;
        const mrcal_point3_t* t_r0;
        mrcal_point3_t        _t_r0;
        const mrcal_point3_t* v0_ref;
        mrcal_point3_t        _v0_ref;

        const int icam_extrinsics0 = pt0->icam.extrinsics;
        if( icam_extrinsics0 >= 0 )
        {
            const mrcal_pose_t* rt_0r = &rt_extrinsics_fromref[icam_extrinsics0];
            const double* r_0r = &rt_0r->r.xyz[0];
            const double* t_0r = &rt_0r->t.xyz[0];

            t_r0   = &_t_r0;
            v0_ref = &_v0_ref;

            mrcal_rotate_point_r_inverted(_t_r0.xyz, NULL,NULL,
                                          r_0r, t_0r);
            for(int i=0; i<3; i++)
                _t_r0.xyz[i] *= -1.;

            mrcal_rotate_point_r_inverted(_v0_ref.xyz, NULL,NULL,
                                          r_0r, v0->xyz);
        }
        else
        {
            t_r0   = NULL;
            v0_ref = v0;
        }


        while(true)
        {
            LOOP_TRIANGULATED_POINT_HEADER();

            /////////////// divergent triangulated observations are DEFINITELY outliers
            if(pt0->outlier || pt1->outlier)
                Nmeasurements_outliers_triangulated_start++;
            else
            {
                const mrcal_point3_t* v1 = &pt1->px;

                const mrcal_point3_t* t_10;
                mrcal_point3_t       _t_10;
                const mrcal_point3_t* v0_cam1;
                mrcal_point3_t        _v0_cam1;

                const int icam_extrinsics1 = pt1->icam.extrinsics;
                if( icam_extrinsics1 >= 0 )
                {
                    const mrcal_pose_t* rt_1r = &rt_extrinsics_fromref[icam_extrinsics1];

                    v0_cam1 = &_v0_cam1;

                    if( icam_extrinsics0 >= 0 )
                    {
                        t_10 = &_t_10;
                        mrcal_transform_point_rt( &_t_10.xyz[0], NULL, NULL,
                                                  &rt_1r->r.xyz[0], &t_r0->xyz[0] );
                    }
                    else
                    {
                        // t_r0 = 0 ->
                        //
                        // t_10 = R_1r*t_r0 + t_1r =
                        //      = R_1r*0    + t_1r =
                        //      = t_1r
                        t_10 = &rt_1r->t;
                    }

                    mrcal_rotate_point_r( &_v0_cam1.xyz[0], NULL, NULL,
                                          &rt_1r->r.xyz[0], &v0_ref->xyz[0] );
                }
                else
                {
                    // rt_1r = 0 ->
                    //
                    // t_10 = R_1r*t_r0 + t_1r =
                    //      = t_r0
                    t_10 = t_r0;
                    // At most one camera can sit at the reference. So if I'm
                    // here, I know that t_r0 != NULL and thus t_10 != NULL

                    v0_cam1 = v0_ref;
                }
                if(!_mrcal_triangulate_leecivera_mid2_is_convergent(v1, v0_cam1, t_10))
                {
                    // Outlier. I don't know which observations was the broken
                    // one, so I mark them both
                    pt0->outlier = true;
                    pt1->outlier = true;
                    foundNewOutliers = true;
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: outliers should not be marked in this first loop. This should happen in the following loop. Putting it here breaks the logic"
#endif


                    // There are a lot of these, so I'm disabling this print for
                    // now, to avoid spamming the terminal
                    //
                    // MSG("New divergent-feature outliers found: measurement %d observation (%d,%d)",
                    //     imeasurement_point_triangulated0 + imeasurement_point_triangulated,
                    //     i0, i1);

                }
            }
            // just marked divergent triangulations as outliers


            if(pt0->outlier || pt1->outlier)
                (*Noutliers_triangulated_point)++;
            else
            {
                var +=
                    x_point_triangulated[imeasurement_point_triangulated] *
                    x_point_triangulated[imeasurement_point_triangulated];
                Ninliers_triangulated_point++;
            }
            LOOP_TRIANGULATED_POINT_FOOTER();
        }
    }
    if(Nobservations_point_triangulated > 0)
    {
        MSG("I started with %d triangulated outliers", Nmeasurements_outliers_triangulated_start);
        MSG("I started with %d triangulated outliers", *Noutliers_triangulated_point);
    }
    var /= (double)(Ninliers_board*2 + Ninliers_triangulated_point);
    // MSG("Outlier rejection sees stdev = %f", sqrt(var));

    ///////////// Any new outliers found?
    LOOP_BOARD_OBSERVATION(!foundNewOutliers)
    {
        LOOP_BOARD_FEATURE()
        {
            LOOP_BOARD_FEATURE_HEADER();

            if(*weight <= 0.0)
                continue;

            double dx = x_boards[2*i_pt_board + 0];
            double dy = x_boards[2*i_pt_board + 1];
            // I have sigma = sqrt(var). Outliers have abs(x) > k*sigma
            // -> x^2 > k^2 var
            if(dx*dx > k1*k1*var ||
               dy*dy > k1*k1*var )
            {
                // MSG("Feature %d looks like an outlier. x/y are %f/%f stdevs off mean (assumed 0). Observed stdev: %f, limit: %f",
                //     i_pt_board,
                //     dx/sqrt(var),
                //     dy/sqrt(var),
                //     sqrt(var),
                //     k1);
                foundNewOutliers = true;
                break;
            }
        }
    }
    LOOP_TRIANGULATED_POINT0(!foundNewOutliers)
    {
        LOOP_TRIANGULATED_POINT1();
        while(true)
        {
            LOOP_TRIANGULATED_POINT_HEADER();

            if(!pt0->outlier && !pt1->outlier)
            {
                double dx = x_point_triangulated[imeasurement_point_triangulated];

                // I have sigma = sqrt(var). Outliers have abs(x) > k*sigma
                // -> x^2 > k^2 var
                if(dx*dx > k1*k1*var )
                {
                    foundNewOutliers = true;
                    break;
                }
            }
            LOOP_TRIANGULATED_POINT_FOOTER();
        }
    }
    if(!foundNewOutliers)
        return false;

    // I have new outliers: some measurements were found past the threshold. I
    // throw out a bit extra to leave some margin so that the next
    // re-optimization would hopefully be the last.
    LOOP_BOARD_OBSERVATION(true)
    {
        int Npt_inlier  = 0;
        int Npt_outlier = 0;
        LOOP_BOARD_FEATURE()
        {
            LOOP_BOARD_FEATURE_HEADER();
            if(*weight <= 0.0)
            {
                Npt_outlier++;
                continue;
            }
            Npt_inlier++;

            double dx = x_boards[2*i_pt_board + 0];
            double dy = x_boards[2*i_pt_board + 1];
            // I have sigma = sqrt(var). Outliers have abs(x) > k*sigma
            // -> x^2 > k^2 var
            if(dx*dx > k0*k0*var ||
               dy*dy > k0*k0*var )
            {
                *weight *= -1.0;
                (*Noutliers_board)++;
            }
        }

        if(Npt_inlier < 3)
            MSG("WARNING: Board observation %d (icam_intrinsics=%d, icam_extrinsics=%d, iframe=%d) had almost all of its points thrown out as outliers: only %d/%d remain. CHOLMOD is about to complain about a non-positive-definite JtJ. Something is wrong with this observation",
                i_observation_board,
                observation->icam.intrinsics,
                observation->icam.extrinsics,
                observation->iframe,
                Npt_inlier,
                Npt_inlier + Npt_outlier);
    }
    LOOP_TRIANGULATED_POINT0(true)
    {
        LOOP_TRIANGULATED_POINT1();
        while(true)
        {
            LOOP_TRIANGULATED_POINT_HEADER();

            if(!pt0->outlier && !pt1->outlier)
            {
                double dx = x_point_triangulated[imeasurement_point_triangulated];

                // I have sigma = sqrt(var). Outliers have abs(x) > k*sigma
                // -> x^2 > k^2 var
                if(dx*dx > k0*k0*var )
                {
                    // Outlier. I don't know which observations was the broken
                    // one, so I mark them both
                    pt0->outlier = true;
                    pt1->outlier = true;

                    (*Noutliers_triangulated_point)++;

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: outliers not returned to the caller yet, so I simply print them out here"
#endif
                    MSG("New outliers found: measurement %d observation (%d,%d)",
                        imeasurement_point_triangulated0 + imeasurement_point_triangulated,
                        i0, i1);
                }
            }

            LOOP_TRIANGULATED_POINT_FOOTER();
        }
    }

    return true;

#undef LOOP_BOARD_OBSERVATION
#undef LOOP_BOARD_FEATURE
#undef LOOP_BOARD_FEATURE_HEADER
#undef LOOP_TRIANGULATED_POINT0
#undef LOOP_TRIANGULATED_POINT1
#undef LOOP_TRIANGULATED_POINT_HEADER
#undef LOOP_TRIANGULATED_POINT_FOOTER
}


typedef struct
{
    // these are all UNPACKED
    const double*         intrinsics;         // Ncameras_intrinsics * NlensParams of these
    const mrcal_pose_t*   extrinsics_fromref; // Ncameras_extrinsics of these. Transform FROM the reference frame
    const mrcal_pose_t*   frames_toref;       // Nframes of these.    Transform TO the reference frame
    const mrcal_point3_t* points;             // Npoints of these.    In the reference frame
    const mrcal_calobject_warp_t* calobject_warp; // 1 of these. May be NULL if !problem_selections.do_optimize_calobject_warp

    // in
    int Ncameras_intrinsics, Ncameras_extrinsics, Nframes;
    int Npoints, Npoints_fixed;

    const mrcal_observation_board_t* observations_board;
    const mrcal_point3_t* observations_board_pool;
    int Nobservations_board;

    const mrcal_observation_point_t* observations_point;
    const mrcal_point3_t* observations_point_pool;
    int Nobservations_point;

    const mrcal_observation_point_triangulated_t* observations_point_triangulated;
    int Nobservations_point_triangulated;

    bool verbose;

    mrcal_lensmodel_t lensmodel;
    mrcal_projection_precomputed_t precomputed;
    const int* imagersizes; // Ncameras_intrinsics*2 of these

    mrcal_problem_selections_t       problem_selections;
    const mrcal_problem_constants_t* problem_constants;

    double calibration_object_spacing;
    int calibration_object_width_n;
    int calibration_object_height_n;

    const int Nmeasurements, N_j_nonzero, Nintrinsics;
} callback_context_t;

static
void penalty_range_normalization(// out
                                 double* penalty, double* dpenalty_ddistsq,

                                 // in
                                 // SIGNED distance. <0 means "behind the camera"
                                 const double distsq,
                                 const callback_context_t* ctx,
                                 const double weight)
{
    const double maxsq = ctx->problem_constants->point_max_range*ctx->problem_constants->point_max_range;
    if(distsq > maxsq)
    {
        *penalty = weight * (distsq/maxsq - 1.0);
        *dpenalty_ddistsq = weight*(1. / maxsq);
        return;
    }

    const double minsq = ctx->problem_constants->point_min_range*ctx->problem_constants->point_min_range;
    if(distsq < minsq)
    {
        // too close OR behind the camera
        *penalty = weight*(1.0 - distsq/minsq);
        *dpenalty_ddistsq = weight*(-1. / minsq);
        return;
    }

    *penalty = *dpenalty_ddistsq = 0.0;
}
static
void optimizer_callback(// input state
                       const double*   packed_state,

                       // output measurements
                       double*         x,

                       // Jacobian
                       cholmod_sparse* Jt,

                       const callback_context_t* ctx)
{
    double norm2_error = 0.0;

    int    iJacobian          = 0;
    int    iMeasurement       = 0;

    int*    Jrowptr = Jt ? (int*)   Jt->p : NULL;
    int*    Jcolidx = Jt ? (int*)   Jt->i : NULL;
    double* Jval    = Jt ? (double*)Jt->x : NULL;
#define STORE_JACOBIAN(col, g)                  \
    do                                          \
    {                                           \
        if(Jt) {                                \
            Jcolidx[ iJacobian ] = col;         \
            Jval   [ iJacobian ] = g;           \
        }                                       \
        iJacobian++;                            \
    } while(0)
#define STORE_JACOBIAN2(col0, g0, g1)           \
    do                                          \
    {                                           \
        if(Jt) {                                \
            Jcolidx[ iJacobian+0 ] = col0+0;    \
            Jval   [ iJacobian+0 ] = g0;        \
            Jcolidx[ iJacobian+1 ] = col0+1;    \
            Jval   [ iJacobian+1 ] = g1;        \
        }                                       \
        iJacobian += 2;                         \
    } while(0)
#define STORE_JACOBIAN3(col0, g0, g1, g2)           \
    do                                              \
    {                                               \
        if(Jt) {                                    \
            Jcolidx[ iJacobian+0 ] = col0+0;        \
            Jval   [ iJacobian+0 ] = g0;            \
            Jcolidx[ iJacobian+1 ] = col0+1;        \
            Jval   [ iJacobian+1 ] = g1;            \
            Jcolidx[ iJacobian+2 ] = col0+2;        \
            Jval   [ iJacobian+2 ] = g2;            \
        }                                           \
        iJacobian += 3;                             \
    } while(0)
#define STORE_JACOBIAN_N(col0, g0, scale, N)        \
    do                                              \
    {                                               \
        if(Jt) {                                    \
            for(int i=0; i<N; i++)                  \
            {                                       \
                Jcolidx[ iJacobian+i ] = col0+i;    \
                Jval   [ iJacobian+i ] = ((g0)==NULL) ? 0.0 : ((scale)*(g0)[i]); \
            }                                       \
        }                                           \
        iJacobian += N;                             \
    } while(0)
#define SCALE_JACOBIAN_N(col0, scale, N)            \
    do                                              \
    {                                               \
        if(Jt) {                                    \
            for(int i=0; i<N; i++)                  \
            {                                       \
                Jcolidx[ iJacobian+i ] = col0+i;    \
                Jval   [ iJacobian+i ] *= scale;    \
            }                                       \
        }                                           \
        iJacobian += N;                             \
    } while(0)



    int Ncore = modelHasCore_fxfycxcy(&ctx->lensmodel) ? 4 : 0;
    int Ncore_state = (modelHasCore_fxfycxcy(&ctx->lensmodel) &&
                       ctx->problem_selections.do_optimize_intrinsics_core) ? 4 : 0;

    // If I'm locking down some parameters, then the state vector contains a
    // subset of my data. I reconstitute the intrinsics and extrinsics here.
    // I do the frame poses later. This is a good way to do it if I have few
    // cameras. With many cameras (this will be slow)

    // WARNING: sparsify this. This is potentially a BIG thing on the stack
    std::vector<std::vector<double>> intrinsics_all(ctx->Ncameras_intrinsics, std::vector<double>(ctx->Nintrinsics));
    std::vector<mrcal_pose_t> camera_rt(ctx->Ncameras_extrinsics);

    mrcal_calobject_warp_t calobject_warp_local = {};
    const int i_var_calobject_warp =
        mrcal_state_index_calobject_warp(ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                         ctx->Nframes,
                                         ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                         ctx->problem_selections, &ctx->lensmodel);
    if(has_calobject_warp(ctx->problem_selections,ctx->Nobservations_board))
        unpack_solver_state_calobject_warp(&calobject_warp_local, &packed_state[i_var_calobject_warp]);
    else if(ctx->calobject_warp != NULL)
        calobject_warp_local = *ctx->calobject_warp;

    for(int icam_intrinsics=0;
        icam_intrinsics<ctx->Ncameras_intrinsics;
        icam_intrinsics++)
    {
        // Construct the FULL intrinsics vector, based on either the
        // optimization vector or the inputs, depending on what we're optimizing
        double* intrinsics_here  = &intrinsics_all[icam_intrinsics][0];

        int i_var_intrinsics =
            mrcal_state_index_intrinsics(icam_intrinsics,
                                         ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                         ctx->Nframes,
                                         ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                         ctx->problem_selections, &ctx->lensmodel);
        if(Ncore)
        {
            if( ctx->problem_selections.do_optimize_intrinsics_core )
            {
                intrinsics_here[0] = packed_state[i_var_intrinsics++] * SCALE_INTRINSICS_FOCAL_LENGTH;
                intrinsics_here[1] = packed_state[i_var_intrinsics++] * SCALE_INTRINSICS_FOCAL_LENGTH;
                intrinsics_here[2] = packed_state[i_var_intrinsics++] * SCALE_INTRINSICS_CENTER_PIXEL;
                intrinsics_here[3] = packed_state[i_var_intrinsics++] * SCALE_INTRINSICS_CENTER_PIXEL;
            }
            else
                memcpy( intrinsics_here,
                        &ctx->intrinsics[ctx->Nintrinsics*icam_intrinsics],
                        Ncore*sizeof(double) );
        }
        int nDistortion = ctx->Nintrinsics-Ncore;
        if (nDistortion) {
            double* distortions_here = &intrinsics_all[icam_intrinsics][Ncore];
            if( ctx->problem_selections.do_optimize_intrinsics_distortions )
            {
                for(int i = 0; i<ctx->Nintrinsics-Ncore; i++)
                    distortions_here[i] = packed_state[i_var_intrinsics++] * SCALE_DISTORTION;
            }
            else {
                memcpy( distortions_here,
                        &ctx->intrinsics[ctx->Nintrinsics*icam_intrinsics + Ncore],
                        (ctx->Nintrinsics-Ncore)*sizeof(double) );
            }
        }
    }
    for(int icam_extrinsics=0;
        icam_extrinsics<ctx->Ncameras_extrinsics;
        icam_extrinsics++)
    {
        if( icam_extrinsics < 0 ) continue;

        const int i_var_camera_rt =
            mrcal_state_index_extrinsics(icam_extrinsics,
                                         ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                         ctx->Nframes,
                                         ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                         ctx->problem_selections, &ctx->lensmodel);
        if(ctx->problem_selections.do_optimize_extrinsics)
            unpack_solver_state_extrinsics_one(&camera_rt[icam_extrinsics], &packed_state[i_var_camera_rt]);
        else
            memcpy(&camera_rt[icam_extrinsics], &ctx->extrinsics_fromref[icam_extrinsics], sizeof(mrcal_pose_t));
    }

    int i_feature = 0;
    for(int i_observation_board = 0;
        i_observation_board < ctx->Nobservations_board;
        i_observation_board++)
    {
        const mrcal_observation_board_t* observation = &ctx->observations_board[i_observation_board];

        const int icam_intrinsics = observation->icam.intrinsics;
        const int icam_extrinsics = observation->icam.extrinsics;
        const int iframe          = observation->iframe;


        // Some of these are bogus if problem_selections says they're inactive
        const int i_var_frame_rt =
            mrcal_state_index_frames(iframe,
                                     ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                     ctx->Nframes,
                                     ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                     ctx->problem_selections, &ctx->lensmodel);

        mrcal_pose_t frame_rt;
        if(ctx->problem_selections.do_optimize_frames)
            unpack_solver_state_framert_one(&frame_rt, &packed_state[i_var_frame_rt]);
        else
            memcpy(&frame_rt, &ctx->frames_toref[iframe], sizeof(mrcal_pose_t));

        const int i_var_intrinsics =
            mrcal_state_index_intrinsics(icam_intrinsics,
                                         ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                         ctx->Nframes,
                                         ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                         ctx->problem_selections, &ctx->lensmodel);
        // invalid if icam_extrinsics < 0, but unused in that case
        const int i_var_camera_rt  =
            mrcal_state_index_extrinsics(icam_extrinsics,
                                         ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                         ctx->Nframes,
                                         ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                         ctx->problem_selections, &ctx->lensmodel);

        // these are computed in respect to the real-unit parameters,
        // NOT the unit-scale parameters used by the optimizer
        std::vector<mrcal_point3_t> dq_drcamera       (ctx->calibration_object_width_n*ctx->calibration_object_height_n*2);
        std::vector<mrcal_point3_t> dq_dtcamera       (ctx->calibration_object_width_n*ctx->calibration_object_height_n*2);
        std::vector<mrcal_point3_t> dq_drframe        (ctx->calibration_object_width_n*ctx->calibration_object_height_n*2);
        std::vector<mrcal_point3_t> dq_dtframe        (ctx->calibration_object_width_n*ctx->calibration_object_height_n*2);
        std::vector<mrcal_calobject_warp_t> dq_dcalobject_warp(ctx->calibration_object_width_n*ctx->calibration_object_height_n*2);
        std::vector<mrcal_point2_t> q_hypothesis      (ctx->calibration_object_width_n*ctx->calibration_object_height_n);
        // I get the intrinsics gradients in separate arrays, possibly sparsely.
        // All the data lives in dq_dintrinsics_pool_double[], with the other data
        // indicating the meaning of the values in the pool.
        //
        // dq_dfxy serves a special-case for a perspective core. Such models
        // are very common, and they have x = fx vx/vz + cx and y = fy vy/vz +
        // cy. So x depends on fx and NOT on fy, and similarly for y. Similar
        // for cx,cy, except we know the gradient value beforehand. I support
        // this case explicitly here. I store dx/dfx and dy/dfy; no cross terms
        int Ngradients = get_Ngradients(&ctx->lensmodel, ctx->Nintrinsics);

        std::vector<double> dq_dintrinsics_pool_double(ctx->calibration_object_width_n*ctx->calibration_object_height_n*Ngradients);
        std::vector<int>    dq_dintrinsics_pool_int   (ctx->calibration_object_width_n*ctx->calibration_object_height_n);
        double* dq_dfxy = NULL;
        double* dq_dintrinsics_nocore = NULL;
        gradient_sparse_meta_t gradient_sparse_meta = {};

        int splined_intrinsics_grad_irun = 0;

        bool camera_at_identity = icam_extrinsics < 0;  
        project(q_hypothesis.data(),

                ctx->problem_selections.do_optimize_intrinsics_core || ctx->problem_selections.do_optimize_intrinsics_distortions ?
                  dq_dintrinsics_pool_double.data() : NULL,
                ctx->problem_selections.do_optimize_intrinsics_core || ctx->problem_selections.do_optimize_intrinsics_distortions ?
                  dq_dintrinsics_pool_int.data() : NULL,
                &dq_dfxy, &dq_dintrinsics_nocore, &gradient_sparse_meta,

                ctx->problem_selections.do_optimize_extrinsics ?
                dq_drcamera.data() : NULL,
                ctx->problem_selections.do_optimize_extrinsics ?
                dq_dtcamera.data() : NULL,
                ctx->problem_selections.do_optimize_frames ?
                dq_drframe.data() : NULL,
                ctx->problem_selections.do_optimize_frames ?
                dq_dtframe.data() : NULL,
                has_calobject_warp(ctx->problem_selections,ctx->Nobservations_board) ?
                dq_dcalobject_warp.data() : NULL,

                // input
                intrinsics_all[icam_intrinsics].data(),
                camera_at_identity ? NULL : &camera_rt[icam_extrinsics], &frame_rt,
                ctx->calobject_warp == NULL ? NULL : &calobject_warp_local,
                camera_at_identity,
                &ctx->lensmodel, &ctx->precomputed,
                ctx->calibration_object_spacing,
                ctx->calibration_object_width_n,
                ctx->calibration_object_height_n);

        for(int i_pt=0;
            i_pt < ctx->calibration_object_width_n*ctx->calibration_object_height_n;
            i_pt++, i_feature++)
        {
            const mrcal_point3_t* qx_qy_w__observed = &ctx->observations_board_pool[i_feature];
            double weight = qx_qy_w__observed->z;

            if(weight >= 0.0)
            {
                // I have my two measurements (dx, dy). I propagate their
                // gradient and store them
                for( int i_xy=0; i_xy<2; i_xy++ )
                {
                    const double err = (q_hypothesis[i_pt].xy[i_xy] - qx_qy_w__observed->xyz[i_xy]) * weight;

                    if(ctx->verbose)
                        MSG("obs/frame/cam_i/cam_e/dot: %d %d %d %d %d err: %g",
                            i_observation_board, iframe, icam_intrinsics, icam_extrinsics, i_pt, err);

                    if(Jt) Jrowptr[iMeasurement] = iJacobian;
                    x[iMeasurement] = err;
                    norm2_error += err*err;

                    if( ctx->problem_selections.do_optimize_intrinsics_core )
                    {
                        // fx,fy. x depends on fx only. y depends on fy only
                        STORE_JACOBIAN( i_var_intrinsics + i_xy,
                                        dq_dfxy[i_pt*2 + i_xy] *
                                        weight * SCALE_INTRINSICS_FOCAL_LENGTH );

                        // cx,cy. The gradients here are known to be 1. And x depends on cx only. And y depends on cy only
                        STORE_JACOBIAN( i_var_intrinsics + i_xy+2,
                                        weight * SCALE_INTRINSICS_CENTER_PIXEL );
                    }

                    if( ctx->problem_selections.do_optimize_intrinsics_distortions )
                    {
                        if(gradient_sparse_meta.pool != NULL)
                        {
                            // u = stereographic(p)
                            // q = (u + deltau(u)) * f + c
                            //
                            // Intrinsics:
                            //   dq/diii = f ddeltau/diii
                            //
                            // ddeltau/diii = flatten(ABCDx[0..3] * ABCDy[0..3])
                            const int ivar0 = dq_dintrinsics_pool_int[splined_intrinsics_grad_irun] -
                                ( ctx->problem_selections.do_optimize_intrinsics_core ? 0 : 4 );

                            const int     len   = gradient_sparse_meta.run_side_length;
                            const double* ABCDx = &gradient_sparse_meta.pool[len*2*splined_intrinsics_grad_irun + 0];
                            const double* ABCDy = &gradient_sparse_meta.pool[len*2*splined_intrinsics_grad_irun + len];

                            const int ivar_stridey = gradient_sparse_meta.ivar_stridey;
                            const double* fxy = &intrinsics_all[icam_intrinsics][0];

                            for(int iy=0; iy<len; iy++)
                                for(int ix=0; ix<len; ix++)
                                    STORE_JACOBIAN( i_var_intrinsics + ivar0 + iy*ivar_stridey + ix*2 + i_xy,
                                                    ABCDx[ix]*ABCDy[iy]*fxy[i_xy] *
                                                    weight * SCALE_DISTORTION );
                        }
                        else
                        {
                            for(int i=0; i<ctx->Nintrinsics-Ncore; i++)
                                STORE_JACOBIAN( i_var_intrinsics+Ncore_state + i,
                                                dq_dintrinsics_nocore[i_pt*2*(ctx->Nintrinsics-Ncore) +
                                                                       i_xy*(ctx->Nintrinsics-Ncore) +
                                                                       i] *
                                                weight * SCALE_DISTORTION );
                        }
                    }

                    if( ctx->problem_selections.do_optimize_extrinsics )
                        if( icam_extrinsics >= 0 )
                        {
                            STORE_JACOBIAN3( i_var_camera_rt + 0,
                                             dq_drcamera[i_pt * 2 + i_xy].xyz[0] *
                                             weight * SCALE_ROTATION_CAMERA,
                                             dq_drcamera[i_pt * 2 + i_xy].xyz[1] *
                                             weight * SCALE_ROTATION_CAMERA,
                                             dq_drcamera[i_pt * 2 + i_xy].xyz[2] *
                                             weight * SCALE_ROTATION_CAMERA);
                            STORE_JACOBIAN3( i_var_camera_rt + 3,
                                             dq_dtcamera[i_pt * 2 + i_xy].xyz[0] *
                                             weight * SCALE_TRANSLATION_CAMERA,
                                             dq_dtcamera[i_pt * 2 + i_xy].xyz[1] *
                                             weight * SCALE_TRANSLATION_CAMERA,
                                             dq_dtcamera[i_pt * 2 + i_xy].xyz[2] *
                                             weight * SCALE_TRANSLATION_CAMERA);
                        }

                    if( ctx->problem_selections.do_optimize_frames )
                    {
                        STORE_JACOBIAN3( i_var_frame_rt + 0,
                                         dq_drframe[i_pt * 2 + i_xy].xyz[0] *
                                         weight * SCALE_ROTATION_FRAME,
                                         dq_drframe[i_pt * 2 + i_xy].xyz[1] *
                                         weight * SCALE_ROTATION_FRAME,
                                         dq_drframe[i_pt * 2 + i_xy].xyz[2] *
                                         weight * SCALE_ROTATION_FRAME);
                        STORE_JACOBIAN3( i_var_frame_rt + 3,
                                         dq_dtframe[i_pt * 2 + i_xy].xyz[0] *
                                         weight * SCALE_TRANSLATION_FRAME,
                                         dq_dtframe[i_pt * 2 + i_xy].xyz[1] *
                                         weight * SCALE_TRANSLATION_FRAME,
                                         dq_dtframe[i_pt * 2 + i_xy].xyz[2] *
                                         weight * SCALE_TRANSLATION_FRAME);
                    }

                    if( has_calobject_warp(ctx->problem_selections,ctx->Nobservations_board) )
                    {
                        STORE_JACOBIAN_N( i_var_calobject_warp,
                                          dq_dcalobject_warp[i_pt * 2 + i_xy].values,
                                          weight * SCALE_CALOBJECT_WARP,
                                          MRCAL_NSTATE_CALOBJECT_WARP);
                    }

                    iMeasurement++;
                }
            }
            else
            {
                // Outlier.

                // This is arbitrary. I'm skipping this observation, so I don't
                // touch the projection results, and I set the measurement and
                // all its gradients to 0. I need to have SOME dependency on the
                // frame parameters to ensure a full-rank Hessian, so if we're
                // skipping all observations for this frame the system will
                // become singular. I don't currently handle this. libdogleg
                // will complain loudly, and add small diagonal L2
                // regularization terms
                for( int i_xy=0; i_xy<2; i_xy++ )
                {
                    const double err = 0.0;

                    if(ctx->verbose)
                        MSG( "obs/frame/cam_i/cam_e/dot: %d %d %d %d %d err: %g",
                             i_observation_board, iframe, icam_intrinsics, icam_extrinsics, i_pt, err);

                    if(Jt) Jrowptr[iMeasurement] = iJacobian;
                    x[iMeasurement] = err;
                    norm2_error += err*err;

                    if( ctx->problem_selections.do_optimize_intrinsics_core )
                    {
                        STORE_JACOBIAN( i_var_intrinsics + i_xy,   0.0 );
                        STORE_JACOBIAN( i_var_intrinsics + i_xy+2, 0.0 );
                    }

                    if( ctx->problem_selections.do_optimize_intrinsics_distortions )
                    {
                        if(gradient_sparse_meta.pool != NULL)
                        {
                            const int ivar0 = dq_dintrinsics_pool_int[splined_intrinsics_grad_irun] -
                                ( ctx->problem_selections.do_optimize_intrinsics_core ? 0 : 4 );
                            const int len          = gradient_sparse_meta.run_side_length;
                            const int ivar_stridey = gradient_sparse_meta.ivar_stridey;

                            for(int iy=0; iy<len; iy++)
                                for(int ix=0; ix<len; ix++)
                                    STORE_JACOBIAN( i_var_intrinsics + ivar0 + iy*ivar_stridey + ix*2 + i_xy, 0.0 );
                        }
                        else
                        {
                            for(int i=0; i<ctx->Nintrinsics-Ncore; i++)
                                STORE_JACOBIAN( i_var_intrinsics+Ncore_state + i, 0.0 );
                        }
                    }

                    if( ctx->problem_selections.do_optimize_extrinsics )
                        if( icam_extrinsics >= 0 )
                        {
                            STORE_JACOBIAN3( i_var_camera_rt + 0, 0.0, 0.0, 0.0);
                            STORE_JACOBIAN3( i_var_camera_rt + 3, 0.0, 0.0, 0.0);
                        }

                    if( ctx->problem_selections.do_optimize_frames )
                    {
                        // Arbitrary differences between the dimensions to keep
                        // my Hessian non-singular. This is 100% arbitrary. I'm
                        // skipping these measurements so these variables
                        // actually don't affect the computation at all
                        STORE_JACOBIAN3( i_var_frame_rt + 0, 0,0,0);
                        STORE_JACOBIAN3( i_var_frame_rt + 3, 0,0,0);
                    }

                    if( has_calobject_warp(ctx->problem_selections,ctx->Nobservations_board) )
                        STORE_JACOBIAN_N( i_var_calobject_warp,
                                          (double*)NULL, 0.0,
                                          MRCAL_NSTATE_CALOBJECT_WARP);

                    iMeasurement++;
                }
            }
            if(gradient_sparse_meta.pool != NULL)
                splined_intrinsics_grad_irun++;
        }
    }

    // Handle all the point observations. This is VERY similar to the
    // board-observation loop above. Please consolidate
    for(int i_observation_point = 0;
        i_observation_point < ctx->Nobservations_point;
        i_observation_point++)
    {
        const mrcal_observation_point_t* observation = &ctx->observations_point[i_observation_point];

        const int icam_intrinsics = observation->icam.intrinsics;
        const int icam_extrinsics = observation->icam.extrinsics;
        const int i_point          = observation->i_point;
        const bool use_position_from_state =
            ctx->problem_selections.do_optimize_frames &&
            i_point < ctx->Npoints - ctx->Npoints_fixed;

        const mrcal_point3_t* qx_qy_w__observed = &ctx->observations_point_pool[i_observation_point];
        double weight = qx_qy_w__observed->z;

        if(weight <= 0.0)
        {
            // Outlier. Cost = 0. Jacobians are 0 too, but I must preserve the
            // structure
            const int i_var_intrinsics =
                mrcal_state_index_intrinsics(icam_intrinsics,
                                             ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                             ctx->Nframes,
                                             ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                             ctx->problem_selections, &ctx->lensmodel);
            // invalid if icam_extrinsics < 0, but unused in that case
            const int i_var_camera_rt  =
                mrcal_state_index_extrinsics(icam_extrinsics,
                                             ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                             ctx->Nframes,
                                             ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                             ctx->problem_selections, &ctx->lensmodel);
            const int i_var_point      =
                mrcal_state_index_points(i_point,
                                         ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                         ctx->Nframes,
                                         ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                         ctx->problem_selections, &ctx->lensmodel);

            // I have my two measurements (dx, dy). I propagate their
            // gradient and store them
            for( int i_xy=0; i_xy<2; i_xy++ )
            {
                if(Jt) Jrowptr[iMeasurement] = iJacobian;
                x[iMeasurement] = 0;

                if( ctx->problem_selections.do_optimize_intrinsics_core )
                {
                    // fx,fy. x depends on fx only. y depends on fy only
                    STORE_JACOBIAN( i_var_intrinsics + i_xy, 0 );

                    // cx,cy. The gradients here are known to be 1. And x depends on cx only. And y depends on cy only
                    STORE_JACOBIAN( i_var_intrinsics + i_xy+2, 0);
                }

                if( ctx->problem_selections.do_optimize_intrinsics_distortions )
                {
                    if( (ctx->problem_selections.do_optimize_intrinsics_core || ctx->problem_selections.do_optimize_intrinsics_distortions) &&
                        ctx->lensmodel.type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC )
                    {
                        // sparse gradient. This is an outlier, so it doesn't
                        // matter which points I say I depend on, as long as I
                        // pick the right number, and says that j=0. I pick the
                        // control points at the start because why not
                        const mrcal_LENSMODEL_SPLINED_STEREOGRAPHIC__config_t* config =
                            &ctx->lensmodel.LENSMODEL_SPLINED_STEREOGRAPHIC__config;
                        int runlen = config->order+1;
                        for(int i=0; i<runlen*runlen; i++)
                            STORE_JACOBIAN( i_var_intrinsics+Ncore_state + i, 0);
                    }
                    else
                        for(int i=0; i<ctx->Nintrinsics-Ncore; i++)
                            STORE_JACOBIAN( i_var_intrinsics+Ncore_state + i, 0);
                }

                if(icam_extrinsics >= 0 && ctx->problem_selections.do_optimize_extrinsics )
                {
                    STORE_JACOBIAN3( i_var_camera_rt + 0, 0,0,0 );
                    STORE_JACOBIAN3( i_var_camera_rt + 3, 0,0,0 );
                }

                if( use_position_from_state )
                    STORE_JACOBIAN3( i_var_point, 0,0,0 );

                iMeasurement++;
            }

        // TEMPORARY TWEAK: disable range normalization
        // I will re-add this later
#if 0
            if(Jt) Jrowptr[iMeasurement] = iJacobian;
            x[iMeasurement] = 0;
            if(icam_extrinsics >= 0 && ctx->problem_selections.do_optimize_extrinsics )
            {
                STORE_JACOBIAN3( i_var_camera_rt + 0, 0,0,0 );
                STORE_JACOBIAN3( i_var_camera_rt + 3, 0,0,0 );
            }
            if( use_position_from_state )
                STORE_JACOBIAN3( i_var_point, 0,0,0 );
            iMeasurement++;
#endif

            continue;
        }


        const int i_var_intrinsics =
            mrcal_state_index_intrinsics(icam_intrinsics,
                                         ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                         ctx->Nframes,
                                         ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                         ctx->problem_selections, &ctx->lensmodel);
        // invalid if icam_extrinsics < 0, but unused in that case
        const int i_var_camera_rt  =
            mrcal_state_index_extrinsics(icam_extrinsics,
                                         ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                         ctx->Nframes,
                                         ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                         ctx->problem_selections, &ctx->lensmodel);
        const int i_var_point      =
            mrcal_state_index_points(i_point,
                                     ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                     ctx->Nframes,
                                     ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                     ctx->problem_selections, &ctx->lensmodel);
        mrcal_point3_t point_ref;
        if(use_position_from_state)
            unpack_solver_state_point_one(&point_ref, &packed_state[i_var_point]);
        else
            point_ref = ctx->points[i_point];

        int Ngradients = get_Ngradients(&ctx->lensmodel, ctx->Nintrinsics);

        // WARNING: "compute size(dq_dintrinsics_pool_double) correctly and maybe bounds-check"
        std::vector<double> dq_dintrinsics_pool_double(Ngradients);
        // used for LENSMODEL_SPLINED_STEREOGRAPHIC only, but getting rid of
        // this in other cases isn't worth the trouble
        int    dq_dintrinsics_pool_int   [1];
        double* dq_dfxy                             = NULL;
        double* dq_dintrinsics_nocore               = NULL;
        gradient_sparse_meta_t gradient_sparse_meta = {};

        mrcal_point3_t dq_drcamera[2];
        mrcal_point3_t dq_dtcamera[2];
        mrcal_point3_t dq_dpoint  [2];

        // The array reference [-3] is intended, but the compiler throws a
        // warning. I silence it here
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
        mrcal_point2_t q_hypothesis;
        project(&q_hypothesis,

                ctx->problem_selections.do_optimize_intrinsics_core || ctx->problem_selections.do_optimize_intrinsics_distortions ?
                dq_dintrinsics_pool_double.data() : NULL,
                ctx->problem_selections.do_optimize_intrinsics_core || ctx->problem_selections.do_optimize_intrinsics_distortions ?
                dq_dintrinsics_pool_int : NULL,
                &dq_dfxy, &dq_dintrinsics_nocore, &gradient_sparse_meta,

                ctx->problem_selections.do_optimize_extrinsics ?
                dq_drcamera : NULL,
                ctx->problem_selections.do_optimize_extrinsics ?
                dq_dtcamera : NULL,
                NULL, // frame rotation. I only have a point position
                use_position_from_state ? dq_dpoint : NULL,
                NULL,

                // input
                intrinsics_all[icam_intrinsics].data(),
                &camera_rt[icam_extrinsics],

                // I only have the point position, so the 'rt' memory
                // points 3 back. The fake "r" here will not be
                // referenced
                (mrcal_pose_t*)(&point_ref.xyz[-3]),
                NULL,

                icam_extrinsics < 0,
                &ctx->lensmodel, &ctx->precomputed,
                0,0,0);
#pragma GCC diagnostic pop

        // I have my two measurements (dx, dy). I propagate their
        // gradient and store them
        for( int i_xy=0; i_xy<2; i_xy++ )
        {
            const double err = (q_hypothesis.xy[i_xy] - qx_qy_w__observed->xyz[i_xy])*weight;

            if(Jt) Jrowptr[iMeasurement] = iJacobian;
            x[iMeasurement] = err;
            norm2_error += err*err;

            if( ctx->problem_selections.do_optimize_intrinsics_core )
            {
                // fx,fy. x depends on fx only. y depends on fy only
                STORE_JACOBIAN( i_var_intrinsics + i_xy,
                                dq_dfxy[i_xy] *
                                weight * SCALE_INTRINSICS_FOCAL_LENGTH );

                // cx,cy. The gradients here are known to be 1. And x depends on cx only. And y depends on cy only
                STORE_JACOBIAN( i_var_intrinsics + i_xy+2,
                                weight * SCALE_INTRINSICS_CENTER_PIXEL );
            }

            if( ctx->problem_selections.do_optimize_intrinsics_distortions )
            {
                if(gradient_sparse_meta.pool != NULL)
                {
                    // u = stereographic(p)
                    // q = (u + deltau(u)) * f + c
                    //
                    // Intrinsics:
                    //   dq/diii = f ddeltau/diii
                    //
                    // ddeltau/diii = flatten(ABCDx[0..3] * ABCDy[0..3])
                    const int ivar0 = dq_dintrinsics_pool_int[0] -
                        ( ctx->problem_selections.do_optimize_intrinsics_core ? 0 : 4 );

                    const int     len   = gradient_sparse_meta.run_side_length;
                    const double* ABCDx = &gradient_sparse_meta.pool[0];
                    const double* ABCDy = &gradient_sparse_meta.pool[len];

                    const int ivar_stridey = gradient_sparse_meta.ivar_stridey;
                    const double* fxy = &intrinsics_all[icam_intrinsics][0];

                    for(int iy=0; iy<len; iy++)
                        for(int ix=0; ix<len; ix++)
                        {
                            STORE_JACOBIAN( i_var_intrinsics + ivar0 + iy*ivar_stridey + ix*2 + i_xy,
                                            ABCDx[ix]*ABCDy[iy]*fxy[i_xy] *
                                            weight * SCALE_DISTORTION );
                        }
                }
                else
                {
                    for(int i=0; i<ctx->Nintrinsics-Ncore; i++)
                        STORE_JACOBIAN( i_var_intrinsics+Ncore_state + i,
                                        dq_dintrinsics_nocore[i_xy*(ctx->Nintrinsics-Ncore) +
                                                               i] *
                                        weight * SCALE_DISTORTION );
                }
            }

            if( ctx->problem_selections.do_optimize_extrinsics )
                if( icam_extrinsics >= 0 )
                {
                    STORE_JACOBIAN3( i_var_camera_rt + 0,
                                     dq_drcamera[i_xy].xyz[0] *
                                     weight * SCALE_ROTATION_CAMERA,
                                     dq_drcamera[i_xy].xyz[1] *
                                     weight * SCALE_ROTATION_CAMERA,
                                     dq_drcamera[i_xy].xyz[2] *
                                     weight * SCALE_ROTATION_CAMERA);
                    STORE_JACOBIAN3( i_var_camera_rt + 3,
                                     dq_dtcamera[i_xy].xyz[0] *
                                     weight * SCALE_TRANSLATION_CAMERA,
                                     dq_dtcamera[i_xy].xyz[1] *
                                     weight * SCALE_TRANSLATION_CAMERA,
                                     dq_dtcamera[i_xy].xyz[2] *
                                     weight * SCALE_TRANSLATION_CAMERA);
                }

            if( use_position_from_state )
                STORE_JACOBIAN3( i_var_point,
                                 dq_dpoint[i_xy].xyz[0] *
                                 weight * SCALE_POSITION_POINT,
                                 dq_dpoint[i_xy].xyz[1] *
                                 weight * SCALE_POSITION_POINT,
                                 dq_dpoint[i_xy].xyz[2] *
                                 weight * SCALE_POSITION_POINT);

            iMeasurement++;
        }


        // TEMPORARY TWEAK: disable range normalization
        // I will re-add this later
#if 0
        // Now the range normalization (make sure the range isn't
        // aphysically high or aphysically low)
        if(icam_extrinsics < 0)
        {
            double distsq =
                point_ref.x*point_ref.x +
                point_ref.y*point_ref.y +
                point_ref.z*point_ref.z;
            double penalty, dpenalty_ddistsq;
            if(model_supports_projection_behind_camera(&ctx->lensmodel) ||
               point_ref.z > 0.0)
                penalty_range_normalization(&penalty, &dpenalty_ddistsq, distsq, ctx,weight);
            else
            {
                penalty_range_normalization(&penalty, &dpenalty_ddistsq, -distsq, ctx,weight);
                dpenalty_ddistsq *= -1.;
            }

            if(Jt) Jrowptr[iMeasurement] = iJacobian;
            x[iMeasurement] = penalty;
            norm2_error += penalty*penalty;

            if( use_position_from_state )
            {
                double scale = 2.0 * dpenalty_ddistsq * SCALE_POSITION_POINT;
                STORE_JACOBIAN3( i_var_point,
                                 scale*point_ref.x,
                                 scale*point_ref.y,
                                 scale*point_ref.z );
            }

            iMeasurement++;
        }
        else
        {
            // I need to transform the point. I already computed
            // this stuff in project()...
            double Rc[3*3];
            double d_Rc_rc[9*3];

            mrcal_R_from_r(Rc,
                           d_Rc_rc,
                           camera_rt[icam_extrinsics].r.xyz);

            mrcal_point3_t pcam;
            mul_vec3_gen33t_vout(point_ref.xyz, Rc, pcam.xyz);
            add_vec(3, pcam.xyz, camera_rt[icam_extrinsics].t.xyz);

            double distsq =
                pcam.x*pcam.x +
                pcam.y*pcam.y +
                pcam.z*pcam.z;
            double penalty, dpenalty_ddistsq;
            if(model_supports_projection_behind_camera(&ctx->lensmodel) ||
               pcam.z > 0.0)
                penalty_range_normalization(&penalty, &dpenalty_ddistsq, distsq, ctx,weight);
            else
            {
                penalty_range_normalization(&penalty, &dpenalty_ddistsq, -distsq, ctx,weight);
                dpenalty_ddistsq *= -1.;
            }

            if(Jt) Jrowptr[iMeasurement] = iJacobian;
            x[iMeasurement] = penalty;
            norm2_error += penalty*penalty;

            if( ctx->problem_selections.do_optimize_extrinsics )
            {
                // pcam.x       = Rc[row0]*point*SCALE + tc
                // d(pcam.x)/dr = d(Rc[row0])/drc*point*SCALE
                // d(Rc[row0])/drc is 3x3 matrix at &d_Rc_rc[0]
                double d_ptcamx_dr[3];
                double d_ptcamy_dr[3];
                double d_ptcamz_dr[3];
                mul_vec3_gen33_vout( point_ref.xyz, &d_Rc_rc[9*0], d_ptcamx_dr );
                mul_vec3_gen33_vout( point_ref.xyz, &d_Rc_rc[9*1], d_ptcamy_dr );
                mul_vec3_gen33_vout( point_ref.xyz, &d_Rc_rc[9*2], d_ptcamz_dr );

                STORE_JACOBIAN3( i_var_camera_rt + 0,
                                 SCALE_ROTATION_CAMERA*
                                 2.0*dpenalty_ddistsq*( pcam.x*d_ptcamx_dr[0] +
                                                        pcam.y*d_ptcamy_dr[0] +
                                                        pcam.z*d_ptcamz_dr[0] ),
                                 SCALE_ROTATION_CAMERA*
                                 2.0*dpenalty_ddistsq*( pcam.x*d_ptcamx_dr[1] +
                                                        pcam.y*d_ptcamy_dr[1] +
                                                        pcam.z*d_ptcamz_dr[1] ),
                                 SCALE_ROTATION_CAMERA*
                                 2.0*dpenalty_ddistsq*( pcam.x*d_ptcamx_dr[2] +
                                                        pcam.y*d_ptcamy_dr[2] +
                                                        pcam.z*d_ptcamz_dr[2] ) );
                STORE_JACOBIAN3( i_var_camera_rt + 3,
                                 SCALE_TRANSLATION_CAMERA*
                                 2.0*dpenalty_ddistsq*pcam.x,
                                 SCALE_TRANSLATION_CAMERA*
                                 2.0*dpenalty_ddistsq*pcam.y,
                                 SCALE_TRANSLATION_CAMERA*
                                 2.0*dpenalty_ddistsq*pcam.z );
            }

            if( use_position_from_state )
                STORE_JACOBIAN3( i_var_point,
                                 SCALE_POSITION_POINT*
                                 2.0*dpenalty_ddistsq*(pcam.x*Rc[0] + pcam.y*Rc[3] + pcam.z*Rc[6]),
                                 SCALE_POSITION_POINT*
                                 2.0*dpenalty_ddistsq*(pcam.x*Rc[1] + pcam.y*Rc[4] + pcam.z*Rc[7]),
                                 SCALE_POSITION_POINT*
                                 2.0*dpenalty_ddistsq*(pcam.x*Rc[2] + pcam.y*Rc[5] + pcam.z*Rc[8]) );
            iMeasurement++;
        }
#endif
    }

    // Handle all the triangulated point observations. This is VERY similar to
    // the board-observation loop above. Please consolidate
    if( ctx->observations_point_triangulated != NULL &&
        ctx->Nobservations_point_triangulated )
    {
        if( ! (!ctx->problem_selections.do_optimize_intrinsics_core &&
               !ctx->problem_selections.do_optimize_intrinsics_distortions &&
               ctx->problem_selections.do_optimize_extrinsics ) )
        {
            // Shouldn't get here. Have a check with an error message in
            // mrcal_optimize() and mrcal_optimizer_callback()
            assert(0);
        }

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: mrcal_observation_point_t.px is the observation vector in camera coords. No outliers. No intrinsics"
// #warning "triangulated-solve: make weights work somehow. This is tied to outliers"
#endif

        for(int i0 = 0;
            i0 < ctx->Nobservations_point_triangulated;
            i0++)
        {
            const mrcal_observation_point_triangulated_t* pt0 =
                &ctx->observations_point_triangulated[i0];
            if(pt0->last_in_set)
                continue;

            if(!pt0->outlier)
            {
                // For better code efficiency I compute the triangulation in the
                // camera-1 coord system. This is because this code looks like
                //
                // for(point0)
                // {
                //   compute_stuff_for_point0;
                //   for(point1)
                //   {
                //     compute_stuff_for_point1;
                //     compute stuff based on products of point0 and point1;
                //   }
                // }
                //
                // Doing the triangulation in the frame of point1 allows me to do
                // more stuff in the outer compute_stuff_for_point0 computation, and
                // less in the inner compute_stuff_for_point1 computation

                // I need t10. I'm looking at a composition Rt_10 = Rt_1r*Rt_r0 =
                // (R_1r,t_1r)*(R_r0,t_r0) = (R_10, R_1r*t_r0 + t_1r) -> t_10 =
                // R_1r*t_r0 + t_1r = transform(Rt_1r, t_r0)
                //
                // I don't actually have t_r0: I have t_0r, so I need to compute an
                // inversion. y = R x + t -> x = Rinv y - Rinv t -> tinv = -Rinv t
                // t_r0 = -R_r0 t_0r

                const mrcal_point3_t* v0 = &pt0->px;

                const mrcal_point3_t* t_r0;
                mrcal_point3_t        _t_r0;
                double                dnegt_r0__dr_0r[3][3];
                double                dnegt_r0__dt_0r[3][3];

                const mrcal_point3_t* v0_ref;
                mrcal_point3_t        _v0_ref;
                double                dv0_ref__dr_0r[3][3];

                const int icam_extrinsics0 = pt0->icam.extrinsics;
                if( icam_extrinsics0 >= 0 )
                {
                    const mrcal_pose_t* rt_0r = &camera_rt[icam_extrinsics0];
                    const double* r_0r = &rt_0r->r.xyz[0];
                    const double* t_0r = &rt_0r->t.xyz[0];

                    t_r0   = &_t_r0;
                    v0_ref = &_v0_ref;

                    mrcal_rotate_point_r_inverted(_t_r0.xyz,
                                                  &dnegt_r0__dr_0r[0][0],
                                                  &dnegt_r0__dt_0r[0][0],

                                                  r_0r, t_0r);
                    for(int i=0; i<3; i++)
                        _t_r0.xyz[i] *= -1.;

                    mrcal_rotate_point_r_inverted(_v0_ref.xyz,
                                                  &dv0_ref__dr_0r[0][0],
                                                  NULL,

                                                  r_0r, v0->xyz);
                }
                else
                {
                    t_r0   = NULL;
                    v0_ref = v0;
                }


                int i1 = i0+1;

                while(true)
                {
                    const mrcal_observation_point_triangulated_t* pt1 =
                        &ctx->observations_point_triangulated[i1];

                    if(!pt1->outlier)
                    {
                        const mrcal_point3_t* v1 = &pt1->px;

                        const mrcal_point3_t* t_10;
                        mrcal_point3_t       _t_10;
                        const mrcal_point3_t* v0_cam1;
                        mrcal_point3_t        _v0_cam1;

                        double dt_10__drt_1r    [3][6];
                        double dt_10__dt_r0     [3][3];
                        double dv0_cam1__dr_1r  [3][3];
                        double dv0_cam1__dv0_ref[3][3];

                        const int icam_extrinsics1 = pt1->icam.extrinsics;
                        if( icam_extrinsics1 >= 0 )
                        {
                            const mrcal_pose_t* rt_1r = &camera_rt[icam_extrinsics1];

                            v0_cam1 = &_v0_cam1;


                            if( icam_extrinsics0 >= 0 )
                            {
                                t_10 = &_t_10;
                                mrcal_transform_point_rt( &_t_10.xyz[0],
                                                          &dt_10__drt_1r[0][0],
                                                          &dt_10__dt_r0 [0][0],
                                                          &rt_1r->r.xyz[0], &t_r0->xyz[0] );
                            }
                            else
                            {
                                // t_r0 = 0 ->
                                //
                                // t_10 = R_1r*t_r0 + t_1r =
                                //      = R_1r*0    + t_1r =
                                //      = t_1r
                                t_10 = &rt_1r->t;
                            }

                            mrcal_rotate_point_r( &_v0_cam1.xyz[0],
                                                  &dv0_cam1__dr_1r  [0][0],
                                                  &dv0_cam1__dv0_ref[0][0],
                                                  &rt_1r->r.xyz[0], &v0_ref->xyz[0] );
                        }
                        else
                        {
                            // rt_1r = 0 ->
                            //
                            // t_10 = R_1r*t_r0 + t_1r =
                            //      = t_r0
                            t_10 = t_r0;
                            // At most one camera can sit at the reference. So if I'm
                            // here, I know that t_r0 != NULL and thus t_10 != NULL

                            v0_cam1 = v0_ref;
                        }

                        mrcal_point3_t derr__dv0_cam1;
                        mrcal_point3_t derr__dt_10;

                        double err =
                            _mrcal_triangulated_error(&derr__dv0_cam1, &derr__dt_10,
                                                      v1, v0_cam1, t_10);


                        x[iMeasurement] = err;
                        norm2_error += err*err;

                        if(Jt)
                        {
                            Jrowptr[iMeasurement] = iJacobian;

                            // Now I propagate gradients. Dependency graph:
                            //
                            //   derr__dv0_cam1
                            //     dv0_cam1__dr_1r
                            //     dv0_cam1__dv0_ref
                            //       dv0_ref__dr_0r
                            //
                            //   derr__dt_10
                            //     dt_10__drt_1r
                            //     dt_10__dt_r0
                            //       dnegt_r0__dr_0r
                            //       dnegt_r0__dt_0r
                            //
                            // So
                            //
                            //   derr/dr_0r =
                            //     derr/dv0_cam1 dv0_cam1/dv0_ref dv0_ref/dr_0r +
                            //     derr/dt_10    dt_10/dt_r0      dt_r0/dr_0r
                            //
                            //   derr/dt_0r =
                            //     derr/dt_10    dt_10/dt_r0      dt_r0/dt_0r
                            //
                            //   derr/dr_1r =
                            //     derr/dv0_cam1 dv0_cam1/dr_1r +
                            //     derr/dt_10    dt_10/dr_1r
                            //
                            //   derr/dt_1r =
                            //     derr/dt_10    dt_10/dt_1r
                            if( icam_extrinsics0 >= 0 )
                            {
                                const int i_var_camera_rt0  =
                                    mrcal_state_index_extrinsics(icam_extrinsics0,
                                                                 ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                                                 ctx->Nframes,
                                                                 ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                                                 ctx->problem_selections, &ctx->lensmodel);

                                double* out;

                                out = &Jval[iJacobian];
                                double* derr__dt_r0;
                                double _derr__dt_r0[3];

                                if( icam_extrinsics1 >= 0 )
                                {
                                    derr__dt_r0 = _derr__dt_r0;
                                    mul_vec3t_gen33(derr__dt_r0, derr__dt_10.xyz, &dt_10__dt_r0[0][0], 1, );

                                    double temp[3];
                                    mul_vec3t_gen33(temp, derr__dv0_cam1.xyz, &dv0_cam1__dv0_ref[0][0], 1, );
                                    mul_vec3t_gen33(out,  temp,               &dv0_ref__dr_0r[0][0],    1, );
                                }
                                else
                                {
                                    // camera1 is at the reference, so I don't have
                                    // dt_10__dt_r0 and dv0_cam1__dv0_ref explicitly
                                    // stored.
                                    //
                                    // t_10    = t_r0   --> dt_10__dt_r0      = I
                                    // v0_cam1 = v0_ref --> dv0_cam1__dv0_ref = I
                                    derr__dt_r0 = derr__dt_10.xyz;
                                    mul_vec3t_gen33(out,  derr__dv0_cam1.xyz, &dv0_ref__dr_0r[0][0],    1, );
                                }


                                mul_vec3t_gen33(out, derr__dt_r0, &dnegt_r0__dr_0r[0][0],  -1, _accum);

                                SCALE_JACOBIAN_N( i_var_camera_rt0 + 0,
                                                  SCALE_ROTATION_CAMERA,
                                                  3 );


                                out = &Jval[iJacobian];
                                mul_vec3t_gen33(out, derr__dt_r0, &dnegt_r0__dt_0r[0][0], -1, );

                                SCALE_JACOBIAN_N( i_var_camera_rt0 + 3,
                                                  SCALE_TRANSLATION_CAMERA,
                                                  3 );
                            }
                            if( icam_extrinsics1 >= 0 )
                            {
                                const int i_var_camera_rt1  =
                                    mrcal_state_index_extrinsics(icam_extrinsics1,
                                                                 ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                                                 ctx->Nframes,
                                                                 ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                                                 ctx->problem_selections, &ctx->lensmodel);

                                double* out;

                                out = &Jval[iJacobian];

                                //   derr/dr_1r =
                                //     derr/dv0_cam1 dv0_cam1/dr_1r +
                                //     derr/dt_10    dt_10/dr_1r
                                //
                                //   derr/dt_1r =
                                //     derr/dt_10    dt_10/dt_1r
                                mul_vec3t_gen33(out,
                                                derr__dv0_cam1.xyz,
                                                &dv0_cam1__dr_1r[0][0],
                                                1,
                                                );

                                if( icam_extrinsics0 >= 0 )
                                {
                                    mul_genNM_genML_accum(out, 3,1,
                                                          1,3,3,
                                                          derr__dt_10.xyz, 3,1,
                                                          &dt_10__drt_1r[0][0], 6, 1,
                                                          1);
                                    SCALE_JACOBIAN_N( i_var_camera_rt1 + 0,
                                                      SCALE_ROTATION_CAMERA,
                                                      3 );

                                    out = &Jval[iJacobian];
                                    mul_genNM_genML(out, 3,1,
                                                    1,3,3,
                                                    derr__dt_10.xyz, 3,1,
                                                    &dt_10__drt_1r[0][3], 6, 1,
                                                    1);

                                    SCALE_JACOBIAN_N( i_var_camera_rt1 + 3,
                                                      SCALE_TRANSLATION_CAMERA,
                                                      3 );
                                }
                                else
                                {
                                    // camera0 is at the reference. dt_10__drt_1r is not
                                    // given explicitly
                                    //
                                    // t_10 = t_1r ->
                                    //   dt_10__dr_1r = 0
                                    //   dt_10__dt_1r = I
                                    // So
                                    //
                                    //   derr/dr_1r = derr/dv0_cam1 dv0_cam1/dr_1r
                                    //   derr/dt_1r = derr/dt_10
                                    SCALE_JACOBIAN_N( i_var_camera_rt1 + 0,
                                                      SCALE_ROTATION_CAMERA,
                                                      3 );

                                    out = &Jval[iJacobian];

                                    for(int i=0; i<3; i++)
                                        out[i] = derr__dt_10.xyz[i];

                                    SCALE_JACOBIAN_N( i_var_camera_rt1 + 3,
                                                      SCALE_TRANSLATION_CAMERA,
                                                      3 );
                                }
                            }
                        }
                        else
                        {
                            // Don't need the Jacobian. I just move iJacobian as needed
                            if( icam_extrinsics0 >= 0 )
                                iJacobian += 6;
                            if( icam_extrinsics1 >= 0 )
                                iJacobian += 6;
                        }
                    }
                    else
                    {
                        // pt1 is an outlier
                        const double err = 0.0;

                        const int icam_extrinsics1 = pt1->icam.extrinsics;

                        x[iMeasurement] = err;
                        norm2_error += err*err;

                        if(Jt)
                        {
                            Jrowptr[iMeasurement] = iJacobian;

                            if( icam_extrinsics0 >= 0 )
                            {
                                const int i_var_camera_rt0  =
                                    mrcal_state_index_extrinsics(icam_extrinsics0,
                                                                 ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                                                 ctx->Nframes,
                                                                 ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                                                 ctx->problem_selections, &ctx->lensmodel);

                                STORE_JACOBIAN_N( i_var_camera_rt0 + 0,
                                                  (double*)NULL, 0.0,
                                                  3 );
                                STORE_JACOBIAN_N( i_var_camera_rt0 + 3,
                                                  (double*)NULL, 0.0,
                                                  3 );
                            }
                            if( icam_extrinsics1 >= 0 )
                            {
                                const int i_var_camera_rt1  =
                                    mrcal_state_index_extrinsics(icam_extrinsics1,
                                                                 ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                                                 ctx->Nframes,
                                                                 ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                                                 ctx->problem_selections, &ctx->lensmodel);
                                STORE_JACOBIAN_N( i_var_camera_rt1 + 0,
                                                  (double*)NULL, 0.0,
                                                  3 );
                                STORE_JACOBIAN_N( i_var_camera_rt1 + 3,
                                                  (double*)NULL, 0.0,
                                                  3 );
                            }
                        }
                        else
                        {
                            // Don't need the Jacobian. I just move iJacobian as needed
                            if( icam_extrinsics0 >= 0 )
                                iJacobian += 6;
                            if( icam_extrinsics1 >= 0 )
                                iJacobian += 6;
                        }
                    }

                    iMeasurement++;

                    if(pt1->last_in_set)
                        break;
                    i1++;
                }
            }
            else
            {
                // pt0 is an outlier. I loop through all the pairwise
                // observations, but I ignore ALL of them
                const double err = 0.0;

                const int icam_extrinsics0 = pt0->icam.extrinsics;
                int i1 = i0+1;

                while(true)
                {
                    const mrcal_observation_point_triangulated_t* pt1 =
                        &ctx->observations_point_triangulated[i1];

                    const int icam_extrinsics1 = pt1->icam.extrinsics;

                    x[iMeasurement] = err;
                    norm2_error += err*err;

                    if(Jt)
                    {
                        Jrowptr[iMeasurement] = iJacobian;

                        if( icam_extrinsics0 >= 0 )
                        {
                            const int i_var_camera_rt0  =
                                mrcal_state_index_extrinsics(icam_extrinsics0,
                                                             ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                                             ctx->Nframes,
                                                             ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                                             ctx->problem_selections, &ctx->lensmodel);

                            STORE_JACOBIAN_N( i_var_camera_rt0 + 0,
                                              (double*)NULL, 0.0,
                                              3 );
                            STORE_JACOBIAN_N( i_var_camera_rt0 + 3,
                                              (double*)NULL, 0.0,
                                              3 );
                        }
                        if( icam_extrinsics1 >= 0 )
                        {
                            const int i_var_camera_rt1  =
                                mrcal_state_index_extrinsics(icam_extrinsics1,
                                                             ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                                             ctx->Nframes,
                                                             ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                                             ctx->problem_selections, &ctx->lensmodel);

                            STORE_JACOBIAN_N( i_var_camera_rt1 + 0,
                                              (double*)NULL, 0.0,
                                              3 );
                            STORE_JACOBIAN_N( i_var_camera_rt1 + 3,
                                              (double*)NULL, 0.0,
                                              3 );
                        }
                    }
                    else
                    {
                        // Don't need the Jacobian. I just move iJacobian as needed
                        if( icam_extrinsics0 >= 0 )
                            iJacobian += 6;
                        if( icam_extrinsics1 >= 0 )
                            iJacobian += 6;
                    }

                    iMeasurement++;

                    if(pt1->last_in_set)
                        break;
                    i1++;
                }
            }
        }
    }

    ///////////////// Regularization
    {
        const bool dump_regularizaton_details = false;

        // I want the total regularization cost to be low relative to the
        // other contributions to the cost. And I want each set of
        // regularization terms to weigh roughly the same. Let's say I want
        // regularization to account for ~ .5% of the other error
        // contributions:
        //
        //   Nmeasurements_rest*normal_pixel_error_sq * 0.005/Nregularization_types =
        //   Nmeasurements_regularization_distortion *normal_regularization_distortion_error_sq  =
        //   Nmeasurements_regularization_centerpixel*normal_regularization_centerpixel_error_sq =
        //   Nmeasurements_regularization_unity_cam01*normal_regularization_unity_cam01_error_sq
        //
        //   normal_regularization_distortion_error_sq     = (scale*normal_distortion_offset )^2
        //   normal_regularization_centerpixel_error_sq    = (scale*normal_centerpixel_value )^2
        //   normal_regularization_unity_cam01_error_sq    = (scale*normal_unity_cam01_value )^2
        //
        // Regularization introduces a bias to the solution. The
        // test-projection-uncertainty test measures it, and barfs if it is too
        // high. The constants should be adjusted if that test fails.
        int Nmeasurements_regularization_distortion  = 0;
        int Nmeasurements_regularization_centerpixel = 0;
        int Nmeasurements_regularization_unity_cam01 = 0;

        if(ctx->problem_selections.do_apply_regularization)
        {
            if(ctx->problem_selections.do_optimize_intrinsics_distortions)
                Nmeasurements_regularization_distortion =
                    ctx->Ncameras_intrinsics*(ctx->Nintrinsics-Ncore);
            if(ctx->problem_selections.do_optimize_intrinsics_core)
                Nmeasurements_regularization_centerpixel =
                    ctx->Ncameras_intrinsics*2;
        }
        if(ctx->problem_selections.do_apply_regularization_unity_cam01 &&
           ctx->problem_selections.do_optimize_extrinsics &&
           ctx->Ncameras_extrinsics > 0)
        {
            Nmeasurements_regularization_unity_cam01 = 1;
        }

        int Nmeasurements_nonregularization =
            ctx->Nmeasurements -
            (Nmeasurements_regularization_distortion +
             Nmeasurements_regularization_centerpixel +
             Nmeasurements_regularization_unity_cam01);

        double normal_pixel_error = 1.0;
        double expected_total_pixel_error_sq =
            (double)Nmeasurements_nonregularization *
            normal_pixel_error *
            normal_pixel_error;
        if(dump_regularizaton_details)
            MSG("expected_total_pixel_error_sq: %f", expected_total_pixel_error_sq);

        // This is set to 2 to match what mrcal 2.4 does, to keep the behavior
        // consistent. The exact value doesn't matter. In a previous commit (the
        // merge 5c3bdd2b) this was changed to 3, and I'm about to revert it
        // back to 2 (2024/07)
        const int Nregularization_types = 2;

        if(ctx->problem_selections.do_apply_regularization &&
           (ctx->problem_selections.do_optimize_intrinsics_distortions ||
            ctx->problem_selections.do_optimize_intrinsics_core))
        {
            double scale_regularization_distortion  = 0.0;
            double scale_regularization_centerpixel = 0.0;

            // compute scales
            {
                if(ctx->problem_selections.do_optimize_intrinsics_distortions)
                {
                    // I need to control this better, but this is sufficient for
                    // now. I need 2.0e-1 for splined models to effectively
                    // eliminate the curl in the splined model vector field. For
                    // other models I use 2.0 because that's what I had for a long
                    // time, and I don't want to change it to not break anything
                    double normal_distortion_value =
                        ctx->lensmodel.type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC ?
                        2.0e-1 :
                        2.0;

                    double expected_regularization_distortion_error_sq_noscale =
                        (double)Nmeasurements_regularization_distortion *
                        normal_distortion_value *
                        normal_distortion_value;

                    double scale_sq =
                        expected_total_pixel_error_sq * 0.005/(double)Nregularization_types / expected_regularization_distortion_error_sq_noscale;

                    if(dump_regularizaton_details)
                        MSG("expected_regularization_distortion_error_sq: %f", expected_regularization_distortion_error_sq_noscale*scale_sq);

                    scale_regularization_distortion = sqrt(scale_sq);
                }

                if(modelHasCore_fxfycxcy(&ctx->lensmodel) &&
                   ctx->problem_selections.do_optimize_intrinsics_core)
                {
                    double normal_centerpixel_offset = 500.0;

                    double expected_regularization_centerpixel_error_sq_noscale =
                        (double)Nmeasurements_regularization_centerpixel *
                        normal_centerpixel_offset *
                        normal_centerpixel_offset;

                    double scale_sq =
                        expected_total_pixel_error_sq * 0.005/(double)Nregularization_types / expected_regularization_centerpixel_error_sq_noscale;

                    if(dump_regularizaton_details)
                        MSG("expected_regularization_centerpixel_error_sq: %f", expected_regularization_centerpixel_error_sq_noscale*scale_sq);

                    scale_regularization_centerpixel = sqrt(scale_sq);
                }
            }

            // compute and store regularization terms
            {
                if( ctx->problem_selections.do_optimize_intrinsics_distortions )
                    for(int icam_intrinsics=0; icam_intrinsics<ctx->Ncameras_intrinsics; icam_intrinsics++)
                    {
                        const int i_var_intrinsics =
                            mrcal_state_index_intrinsics(icam_intrinsics,
                                                         ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                                         ctx->Nframes,
                                                         ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                                         ctx->problem_selections, &ctx->lensmodel);

                        if(ctx->lensmodel.type == MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC)
                        {
                            // Splined model regularization. I do directional L2
                            // regularization. At each knot I penalize contributions in
                            // the tangential direction much more than in the radial
                            // direction. Otherwise noise in the data produces lots of
                            // curl in the vector field. This isn't wrong, but it's much
                            // nicer if "right" in the camera coordinate system
                            // corresponds to "right" in pixel space
                            const int Nx = ctx->lensmodel.LENSMODEL_SPLINED_STEREOGRAPHIC__config.Nx;
                            const int Ny = ctx->lensmodel.LENSMODEL_SPLINED_STEREOGRAPHIC__config.Ny;

                            for(int iy=0; iy<Ny; iy++)
                                for(int ix=0; ix<Nx; ix++)
                                {
                                    double scale = scale_regularization_distortion;

                                    const int ivar = 2*( iy*Nx + ix );
                                    const double deltauxy[] =
                                        { intrinsics_all[icam_intrinsics][Ncore + ivar + 0],
                                          intrinsics_all[icam_intrinsics][Ncore + ivar + 1] };

                                    // WARNING: "Precompute uxy. This is lots of unnecessary computation in the inner loop"
                                    double uxy[] = { (double)(2*ix - Nx + 1),
                                                     (double)(2*iy - Ny + 1) };
                                    bool anisotropic = true;
                                    if(2*ix == Nx - 1 &&
                                       2*iy == Ny - 1 )
                                    {
                                        uxy[0] = 1.0;
                                        anisotropic = false;
                                    }
                                    else
                                    {
                                        double mag = sqrt(uxy[0]*uxy[0] + uxy[1]*uxy[1]);
                                        uxy[0] /= mag;
                                        uxy[1] /= mag;
                                    }

                                    double err;

                                    // I penalize radial corrections
                                    if(Jt) Jrowptr[iMeasurement] = iJacobian;
                                    err              = scale*(deltauxy[0]*uxy[0] +
                                                              deltauxy[1]*uxy[1]);
                                    x[iMeasurement]  = err;
                                    norm2_error     += err*err;
                                    STORE_JACOBIAN( i_var_intrinsics + Ncore_state + ivar + 0,
                                                    scale * uxy[0] * SCALE_DISTORTION );
                                    STORE_JACOBIAN( i_var_intrinsics + Ncore_state + ivar + 1,
                                                    scale * uxy[1] * SCALE_DISTORTION );
                                    iMeasurement++;

                                    // I REALLY penalize tangential corrections
                                    if(anisotropic) scale *= 10.;
                                    if(Jt) Jrowptr[iMeasurement] = iJacobian;
                                    err              = scale*(deltauxy[0]*uxy[1] - deltauxy[1]*uxy[0]);
                                    x[iMeasurement]  = err;
                                    norm2_error     += err*err;
                                    STORE_JACOBIAN( i_var_intrinsics + Ncore_state + ivar + 0,
                                                    scale * uxy[1] * SCALE_DISTORTION );
                                    STORE_JACOBIAN( i_var_intrinsics + Ncore_state + ivar + 1,
                                                    -scale * uxy[0] * SCALE_DISTORTION );
                                    iMeasurement++;
                                }
                        }
                        else
                        {
                            for(int j=0; j<ctx->Nintrinsics-Ncore; j++)
                            {
                                // This maybe should live elsewhere, but I put it here
                                // for now. Various distortion coefficients have
                                // different meanings, and should be regularized in
                                // different ways. Specific logic follows
                                double scale = scale_regularization_distortion;

                                if( MRCAL_LENSMODEL_IS_OPENCV(ctx->lensmodel.type) &&
                                    ctx->lensmodel.type >= MRCAL_LENSMODEL_OPENCV8 &&
                                    5 <= j && j <= 7 )
                                {
                                    // The radial distortion in opencv is x_distorted =
                                    // x*scale where r2 = norm2(xy - xyc) and
                                    //
                                    // scale = (1 + k0 r2 + k1 r4 + k4 r6)/(1 + k5 r2 + k6 r4 + k7 r6)
                                    //
                                    // Note that k2,k3 are tangential (NOT radial)
                                    // distortion components. Note that the r6 factor in
                                    // the numerator is only present for
                                    // >=MRCAL_LENSMODEL_OPENCV5. Note that the denominator
                                    // is only present for >= MRCAL_LENSMODEL_OPENCV8. The
                                    // danger with a rational model is that it's
                                    // possible to get into a situation where scale ~
                                    // 0/0 ~ 1. This would have very poorly behaved
                                    // derivatives. If all the rational coefficients are
                                    // ~0, then the denominator is always ~1, and this
                                    // problematic case can't happen. I favor that by
                                    // regularizing the coefficients in the denominator
                                    // more strongly
                                    scale *= 5.;
                                }

                                if(Jt) Jrowptr[iMeasurement] = iJacobian;
                                double err       = scale*intrinsics_all[icam_intrinsics][j+Ncore];
                                x[iMeasurement]  = err;
                                norm2_error     += err*err;

                                STORE_JACOBIAN( i_var_intrinsics + Ncore_state + j,
                                                scale * SCALE_DISTORTION );

                                iMeasurement++;
                                if(dump_regularizaton_details)
                                    MSG("regularization distortion: %g; norm2: %g", err, err*err);

                            }
                        }
                    }

                if( modelHasCore_fxfycxcy(&ctx->lensmodel) &&
                    ctx->problem_selections.do_optimize_intrinsics_core )
                    for(int icam_intrinsics=0; icam_intrinsics<ctx->Ncameras_intrinsics; icam_intrinsics++)
                    {
                        const int i_var_intrinsics =
                            mrcal_state_index_intrinsics(icam_intrinsics,
                                                         ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                                         ctx->Nframes,
                                                         ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                                         ctx->problem_selections, &ctx->lensmodel);

                        // And another regularization term: optical center should be
                        // near the middle. This breaks the symmetry between moving the
                        // center pixel coords and pitching/yawing the camera.
                        double cx_target = 0.5 * (double)(ctx->imagersizes[icam_intrinsics*2 + 0] - 1);
                        double cy_target = 0.5 * (double)(ctx->imagersizes[icam_intrinsics*2 + 1] - 1);

                        double err;

                        if(Jt) Jrowptr[iMeasurement] = iJacobian;
                        err = scale_regularization_centerpixel *
                            (intrinsics_all[icam_intrinsics][2] - cx_target);
                        x[iMeasurement]  = err;
                        norm2_error     += err*err;
                        STORE_JACOBIAN( i_var_intrinsics + 2,
                                        scale_regularization_centerpixel * SCALE_INTRINSICS_CENTER_PIXEL );
                        iMeasurement++;
                        if(dump_regularizaton_details)
                            MSG("regularization center pixel off-center: %g; norm2: %g", err, err*err);

                        if(Jt) Jrowptr[iMeasurement] = iJacobian;
                        err = scale_regularization_centerpixel *
                            (intrinsics_all[icam_intrinsics][3] - cy_target);
                        x[iMeasurement]  = err;
                        norm2_error     += err*err;
                        STORE_JACOBIAN( i_var_intrinsics + 3,
                                        scale_regularization_centerpixel * SCALE_INTRINSICS_CENTER_PIXEL );
                        iMeasurement++;
                        if(dump_regularizaton_details)
                            MSG("regularization center pixel off-center: %g; norm2: %g", err, err*err);
                    }
            }
        }


        if(ctx->problem_selections.do_apply_regularization_unity_cam01 &&
           ctx->problem_selections.do_optimize_extrinsics &&
           ctx->Ncameras_extrinsics > 0)
        {
            double scale_regularization_unity_cam01 = 0.0;

            // compute scales
            {
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: better unity_cam01 scale"
#endif
                double normal_unity_cam01_value = 1.0;

                double expected_regularization_unity_cam01_error_sq_noscale =
                    (double)Nmeasurements_regularization_unity_cam01 *
                    normal_unity_cam01_value *
                    normal_unity_cam01_value;

                double scale_sq =
                    expected_total_pixel_error_sq * 0.005/(double)Nregularization_types / expected_regularization_unity_cam01_error_sq_noscale;

                if(dump_regularizaton_details)
                    MSG("expected_regularization_unity_cam01_error_sq: %f", expected_regularization_unity_cam01_error_sq_noscale*scale_sq);

                scale_regularization_unity_cam01 = sqrt(scale_sq);
            }

            // compute and store regularization terms
            {
                // I have the pose for the first camera: rt_0r. The distance
                // between the origin of this camera and the origin of the
                // reference is t_0r
                const mrcal_point3_t* t_0r = &camera_rt[0].t;

                const int i_var_extrinsics =
                    mrcal_state_index_extrinsics(0,
                                                 ctx->Ncameras_intrinsics, ctx->Ncameras_extrinsics,
                                                 ctx->Nframes,
                                                 ctx->Npoints, ctx->Npoints_fixed, ctx->Nobservations_board,
                                                 ctx->problem_selections, &ctx->lensmodel);

                if(Jt) Jrowptr[iMeasurement] = iJacobian;
                double err =
                    scale_regularization_unity_cam01 *
                    (norm2_vec(3, t_0r->xyz) - 1.);
                x[iMeasurement]  = err;
                norm2_error     += err*err;

                for(int i=0; i<3; i++)
                    STORE_JACOBIAN( i_var_extrinsics+3 + i,
                                    scale_regularization_unity_cam01 * SCALE_TRANSLATION_CAMERA *
                                    2.* t_0r->xyz[i]);

                iMeasurement++;
                if(dump_regularizaton_details)
                    MSG("regularization unity_cam01: %g; norm2: %g", err, err*err);
            }
        }
    }

    if(Jt) Jrowptr[iMeasurement] = iJacobian;
    if(iMeasurement != ctx->Nmeasurements)
    {
        MSG("Assertion (iMeasurement == ctx->Nmeasurements) failed: (%d != %d)",
            iMeasurement, ctx->Nmeasurements);
        assert(0);
    }
    if(iJacobian    != ctx->N_j_nonzero  )
    {
        MSG("Assertion (iJacobian    == ctx->N_j_nonzero  ) failed: (%d != %d)",
            iJacobian, ctx->N_j_nonzero);
        assert(0);
    }
}

bool mrcal_optimizer_callback(// out

                             // These output pointers may NOT be NULL, unlike
                             // their analogues in mrcal_optimize()

                             // Shape (Nstate,)
                             double* b_packed,
                             // used only to confirm that the user passed-in the buffer they
                             // should have passed-in. The size must match exactly
                             int buffer_size_b_packed,

                             // Shape (Nmeasurements,)
                             double* x,
                             // used only to confirm that the user passed-in the buffer they
                             // should have passed-in. The size must match exactly
                             int buffer_size_x,

                             // output Jacobian. May be NULL if we don't need
                             // it. This is the unitless Jacobian, used by the
                             // internal optimization routines
                             cholmod_sparse* Jt,


                             // in

                             // intrinsics is a concatenation of the intrinsics core
                             // and the distortion params. The specific distortion
                             // parameters may vary, depending on lensmodel, so
                             // this is a variable-length structure
                             const double*             intrinsics,         // Ncameras_intrinsics * NlensParams
                             const mrcal_pose_t*       extrinsics_fromref, // Ncameras_extrinsics of these. Transform FROM the reference frame
                             const mrcal_pose_t*       frames_toref,       // Nframes of these.    Transform TO the reference frame
                             const mrcal_point3_t*     points,             // Npoints of these.    In the reference frame
                             const mrcal_calobject_warp_t* calobject_warp, // 1 of these. May be NULL if !problem_selections.do_optimize_calobject_warp

                             int Ncameras_intrinsics, int Ncameras_extrinsics, int Nframes,
                             int Npoints, int Npoints_fixed, // at the end of points[]

                             const mrcal_observation_board_t* observations_board,
                             const mrcal_observation_point_t* observations_point,
                             int Nobservations_board,
                             int Nobservations_point,

                             const mrcal_observation_point_triangulated_t* observations_point_triangulated,
                             int Nobservations_point_triangulated,

                             // All the board pixel observations, in order. .x,
                             // .y are the pixel observations .z is the weight
                             // of the observation. Most of the weights are
                             // expected to be 1.0. Less precise observations
                             // have lower weights.
                             //
                             // z<0 indicates that this is an outlier
                             const mrcal_point3_t* observations_board_pool,

                             // Same thing, but for discrete points. Array of shape
                             //
                             // ( Nobservations_point,)
                             const mrcal_point3_t* observations_point_pool,

                             const mrcal_lensmodel_t* lensmodel,
                             const int* imagersizes, // Ncameras_intrinsics*2 of these

                             mrcal_problem_selections_t       problem_selections,
                             const mrcal_problem_constants_t* problem_constants,

                             double calibration_object_spacing,
                             int calibration_object_width_n,
                             int calibration_object_height_n,
                             bool verbose)
{
    bool result = false;

    if( ( observations_point_triangulated != NULL &&
          Nobservations_point_triangulated ) &&
        ! (!problem_selections.do_optimize_intrinsics_core &&
           !problem_selections.do_optimize_intrinsics_distortions &&
           problem_selections.do_optimize_extrinsics ) )
    {
        MSG("ERROR: We have triangulated points. At this time this is only allowed if we're NOT optimizing intrinsics AND if we ARE optimizing extrinsics.");
        return result;
    }

    if( Nobservations_board > 0 )
    {
        if( problem_selections.do_optimize_calobject_warp && calobject_warp == NULL )
        {
            MSG("ERROR: We're optimizing the calibration object warp, so a buffer with a seed MUST be passed in.");
            return result;
        }
    }
    else
        problem_selections.do_optimize_calobject_warp = false;

    if(!modelHasCore_fxfycxcy(lensmodel))
        problem_selections.do_optimize_intrinsics_core = false;

    if(!problem_selections.do_optimize_intrinsics_core        &&
       !problem_selections.do_optimize_intrinsics_distortions &&
       !problem_selections.do_optimize_extrinsics             &&
       !problem_selections.do_optimize_frames                 &&
       !problem_selections.do_optimize_calobject_warp)
    {
        MSG("Not optimizing any of our variables!");
        return result;
    }


    const int Nstate = mrcal_num_states(Ncameras_intrinsics, Ncameras_extrinsics,
                                        Nframes,
                                        Npoints, Npoints_fixed, Nobservations_board,
                                        problem_selections,
                                        lensmodel);
    if( buffer_size_b_packed != Nstate*(int)sizeof(double) )
    {
        MSG("The buffer passed to fill-in b_packed has the wrong size. Needed exactly %d bytes, but got %d bytes",
            Nstate*(int)sizeof(double),buffer_size_b_packed);
        return result;
    }

    int Nmeasurements = mrcal_num_measurements(Nobservations_board,
                                               Nobservations_point,
                                               observations_point_triangulated,
                                               Nobservations_point_triangulated,
                                               calibration_object_width_n,
                                               calibration_object_height_n,
                                               Ncameras_intrinsics, Ncameras_extrinsics,
                                               Nframes,
                                               Npoints, Npoints_fixed,
                                               problem_selections,
                                               lensmodel);
    int Nintrinsics = mrcal_lensmodel_num_params(lensmodel);
    int N_j_nonzero = _mrcal_num_j_nonzero(Nobservations_board,
                                           Nobservations_point,
                                           observations_point_triangulated,
                                           Nobservations_point_triangulated,
                                           calibration_object_width_n,
                                           calibration_object_height_n,
                                           Ncameras_intrinsics, Ncameras_extrinsics,
                                           Nframes,
                                           Npoints, Npoints_fixed,
                                           observations_board,
                                           observations_point,
                                           problem_selections,
                                           lensmodel);

    if( buffer_size_x != Nmeasurements*(int)sizeof(double) )
    {
        MSG("The buffer passed to fill-in x has the wrong size. Needed exactly %d bytes, but got %d bytes",
            Nmeasurements*(int)sizeof(double),buffer_size_x);
        return result;
    }

    const int Npoints_fromBoards =
        Nobservations_board *
        calibration_object_width_n*calibration_object_height_n;

    const callback_context_t ctx = {
        .intrinsics                 = intrinsics,
        .extrinsics_fromref         = extrinsics_fromref,
        .frames_toref               = frames_toref,
        .points                     = points,
        .calobject_warp             = calobject_warp,
        .Ncameras_intrinsics        = Ncameras_intrinsics,
        .Ncameras_extrinsics        = Ncameras_extrinsics,
        .Nframes                    = Nframes,
        .Npoints                    = Npoints,
        .Npoints_fixed              = Npoints_fixed,
        .observations_board         = observations_board,
        .observations_board_pool    = observations_board_pool,
        .Nobservations_board        = Nobservations_board,
        .observations_point         = observations_point,
        .observations_point_pool    = observations_point_pool,
        .Nobservations_point        = Nobservations_point,
        .observations_point_triangulated  = observations_point_triangulated,
        .Nobservations_point_triangulated = Nobservations_point_triangulated,
        .verbose                    = verbose,
        .lensmodel                  = *lensmodel,
        .imagersizes                = imagersizes,
        .problem_selections         = problem_selections,
        .problem_constants          = problem_constants,
        .calibration_object_spacing = calibration_object_spacing,
        .calibration_object_width_n = calibration_object_width_n  > 0 ? calibration_object_width_n  : 0,
        .calibration_object_height_n= calibration_object_height_n > 0 ? calibration_object_height_n : 0,
        .Nmeasurements              = Nmeasurements,
        .N_j_nonzero                = N_j_nonzero,
        .Nintrinsics                = Nintrinsics};
    _mrcal_precompute_lensmodel_data((mrcal_projection_precomputed_t*)&ctx.precomputed, lensmodel);

    pack_solver_state(b_packed,
                      lensmodel, intrinsics,
                      extrinsics_fromref,
                      frames_toref,
                      points,
                      calobject_warp,
                      problem_selections,
                      Ncameras_intrinsics, Ncameras_extrinsics,
                      Nframes, Npoints-Npoints_fixed,
                      Nobservations_board,
                      Nstate);

    optimizer_callback(b_packed, x, Jt, &ctx);

    result = true;

    return result;
}

mrcal_stats_t
mrcal_optimize( // out
                // Each one of these output pointers may be NULL

                // Shape (Nstate,)
                double* b_packed_final,
                // used only to confirm that the user passed-in the buffer they
                // should have passed-in. The size must match exactly
                int buffer_size_b_packed_final,

                // Shape (Nmeasurements,)
                double* x_final,
                // used only to confirm that the user passed-in the buffer they
                // should have passed-in. The size must match exactly
                int buffer_size_x_final,

                // out, in

                // These are a seed on input, solution on output

                // intrinsics is a concatenation of the intrinsics core and the
                // distortion params. The specific distortion parameters may
                // vary, depending on lensmodel, so this is a variable-length
                // structure
                double*             intrinsics,         // Ncameras_intrinsics * NlensParams
                mrcal_pose_t*       extrinsics_fromref, // Ncameras_extrinsics of these. Transform FROM the reference frame
                mrcal_pose_t*       frames_toref,       // Nframes of these.    Transform TO the reference frame
                mrcal_point3_t*     points,             // Npoints of these.    In the reference frame
                mrcal_calobject_warp_t* calobject_warp, // 1 of these. May be NULL if !problem_selections.do_optimize_calobject_warp

                // in
                int Ncameras_intrinsics, int Ncameras_extrinsics, int Nframes,
                int Npoints, int Npoints_fixed, // at the end of points[]

                const mrcal_observation_board_t* observations_board,
                const mrcal_observation_point_t* observations_point,
                int Nobservations_board,
                int Nobservations_point,

                const mrcal_observation_point_triangulated_t* observations_point_triangulated,
                int Nobservations_point_triangulated,

                // All the board pixel observations, in order.
                // .x, .y are the pixel observations
                // .z is the weight of the observation. Most of the weights are
                // expected to be 1.0, which implies that the noise on the
                // observation has standard deviation of
                // observed_pixel_uncertainty. observed_pixel_uncertainty scales
                // inversely with the weight.
                //
                // z<0 indicates that this is an outlier. This is respected on
                // input (even if !do_apply_outlier_rejection). New outliers are
                // marked with z<0 on output, so this isn't const
                mrcal_point3_t* observations_board_pool,

                // Same thing, but for discrete points. Array of shape
                //
                // ( Nobservations_point,)
                mrcal_point3_t* observations_point_pool,

                const mrcal_lensmodel_t* lensmodel,
                const int* imagersizes, // Ncameras_intrinsics*2 of these
                mrcal_problem_selections_t       problem_selections,
                const mrcal_problem_constants_t* problem_constants,

                double calibration_object_spacing,
                int calibration_object_width_n,
                int calibration_object_height_n,
                bool verbose,

                bool check_gradient)
{
    if( Nobservations_board > 0 )
    {
        if( problem_selections.do_optimize_calobject_warp && calobject_warp == NULL )
        {
            MSG("ERROR: We're optimizing the calibration object warp, so a buffer with a seed MUST be passed in.");
            return (mrcal_stats_t){.rms_reproj_error__pixels = -1.0};
        }
    }
    else
        problem_selections.do_optimize_calobject_warp = false;

    if( ( observations_point_triangulated != NULL &&
          Nobservations_point_triangulated ) &&
        ! (!problem_selections.do_optimize_intrinsics_core &&
           !problem_selections.do_optimize_intrinsics_distortions &&
           problem_selections.do_optimize_extrinsics ) )
    {
        MSG("ERROR: We have triangulated points. At this time this is only allowed if we're NOT optimizing intrinsics AND if we ARE optimizing extrinsics.");
        // Because the input to the triangulation routines is unprojected
        // vectors, and I don't want to unproject as part of the optimization
        // callback. And because I must optimize SOMETHING, so if I have fixed
        // intrinsics then the extrinsics cannot be fixed
        return (mrcal_stats_t){.rms_reproj_error__pixels = -1.0};
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: can I loosen this? Optimizing intrinsics and triangulated points together should work"
#endif
    }

    if(!modelHasCore_fxfycxcy(lensmodel))
        problem_selections.do_optimize_intrinsics_core = false;

    if(!problem_selections.do_optimize_intrinsics_core        &&
       !problem_selections.do_optimize_intrinsics_distortions &&
       !problem_selections.do_optimize_extrinsics             &&
       !problem_selections.do_optimize_frames                 &&
       !problem_selections.do_optimize_calobject_warp)
    {
        MSG("Warning: Not optimizing any of our variables");
    }

    dogleg_parameters2_t dogleg_parameters;
    dogleg_getDefaultParameters(&dogleg_parameters);
    dogleg_parameters.dogleg_debug = verbose ? DOGLEG_DEBUG_VNLOG : 0;

    // These were derived empirically, seeking high accuracy, fast convergence
    // and without serious concern for performance. I looked only at a single
    // frame. Tweak them please
    dogleg_parameters.Jt_x_threshold                    = 0;
    dogleg_parameters.update_threshold                  = 1e-6;
    dogleg_parameters.trustregion_threshold             = 0;
    dogleg_parameters.max_iterations                    = 300;
    // dogleg_parameters.trustregion_decrease_factor    = 0.1;
    // dogleg_parameters.trustregion_decrease_threshold = 0.15;
    // dogleg_parameters.trustregion_increase_factor    = 4.0
    // dogleg_parameters.trustregion_increase_threshold = 0.75;

    const int Npoints_fromBoards =
        Nobservations_board *
        calibration_object_width_n*calibration_object_height_n;

    callback_context_t ctx = {
        .intrinsics                 = intrinsics,
        .extrinsics_fromref         = extrinsics_fromref,
        .frames_toref               = frames_toref,
        .points                     = points,
        .calobject_warp             = calobject_warp,
        .Ncameras_intrinsics        = Ncameras_intrinsics,
        .Ncameras_extrinsics        = Ncameras_extrinsics,
        .Nframes                    = Nframes,
        .Npoints                    = Npoints,
        .Npoints_fixed              = Npoints_fixed,
        .observations_board         = observations_board,
        .observations_board_pool    = observations_board_pool,
        .Nobservations_board        = Nobservations_board,
        .observations_point         = observations_point,
        .observations_point_pool    = observations_point_pool,
        .Nobservations_point        = Nobservations_point,
        .observations_point_triangulated  = observations_point_triangulated,
        .Nobservations_point_triangulated = Nobservations_point_triangulated,
        .verbose                    = verbose,
        .lensmodel                  = *lensmodel,
        .imagersizes                = imagersizes,
        .problem_selections         = problem_selections,
        .problem_constants          = problem_constants,
        .calibration_object_spacing = calibration_object_spacing,
        .calibration_object_width_n = calibration_object_width_n  > 0 ? calibration_object_width_n  : 0,
        .calibration_object_height_n= calibration_object_height_n > 0 ? calibration_object_height_n : 0,
        .Nmeasurements              = mrcal_num_measurements(Nobservations_board,
                                                             Nobservations_point,
                                                             observations_point_triangulated,
                                                             Nobservations_point_triangulated,
                                                             calibration_object_width_n,
                                                             calibration_object_height_n,
                                                             Ncameras_intrinsics, Ncameras_extrinsics,
                                                             Nframes,
                                                             Npoints, Npoints_fixed,
                                                             problem_selections,
                                                             lensmodel),
        .N_j_nonzero                = _mrcal_num_j_nonzero(Nobservations_board,
                                                           Nobservations_point,
                                                           observations_point_triangulated,
                                                           Nobservations_point_triangulated,
                                                           calibration_object_width_n,
                                                           calibration_object_height_n,
                                                           Ncameras_intrinsics, Ncameras_extrinsics,
                                                           Nframes,
                                                           Npoints, Npoints_fixed,
                                                           observations_board,
                                                           observations_point,
                                                           problem_selections,
                                                           lensmodel),
        .Nintrinsics                = mrcal_lensmodel_num_params(lensmodel)};
    _mrcal_precompute_lensmodel_data((mrcal_projection_precomputed_t*)&ctx.precomputed, lensmodel);

    const int Nstate = mrcal_num_states(Ncameras_intrinsics, Ncameras_extrinsics,
                                        Nframes,
                                        Npoints, Npoints_fixed, Nobservations_board,
                                        problem_selections,
                                        lensmodel);

    if( b_packed_final != NULL &&
        buffer_size_b_packed_final != Nstate*(int)sizeof(double) )
    {
        MSG("The buffer passed to fill-in b_packed_final has the wrong size. Needed exactly %d bytes, but got %d bytes",
            Nstate*(int)sizeof(double),buffer_size_b_packed_final);
        return (mrcal_stats_t){.rms_reproj_error__pixels = -1.0};
    }
    if( x_final != NULL &&
        buffer_size_x_final != ctx.Nmeasurements*(int)sizeof(double) )
    {
        MSG("The buffer passed to fill-in x_final has the wrong size. Needed exactly %d bytes, but got %d bytes",
            ctx.Nmeasurements*(int)sizeof(double),buffer_size_x_final);
        return (mrcal_stats_t){.rms_reproj_error__pixels = -1.0};
    }


    dogleg_solverContext_t* solver_context = NULL;

    if(verbose)
        MSG("## Nmeasurements=%d, Nstate=%d", ctx.Nmeasurements, Nstate);
    if(ctx.Nmeasurements <= Nstate)
    {
        MSG("WARNING: problem isn't overdetermined: Nmeasurements=%d, Nstate=%d. Solver may not converge, and if it does, the results aren't reliable. Add more constraints and/or regularization",
            ctx.Nmeasurements, Nstate);
    }

    // WARNING: is it reasonable to put this on the stack? Can I use
    // b_packed_final for this?
    std::vector<double> packed_state(Nstate);
    pack_solver_state(packed_state.data(),
                      lensmodel, intrinsics,
                      extrinsics_fromref,
                      frames_toref,
                      points,
                      calobject_warp,
                      problem_selections,
                      Ncameras_intrinsics, Ncameras_extrinsics,
                      Nframes, Npoints-Npoints_fixed,
                      Nobservations_board,
                      Nstate);

    double norm2_error = -1.0;
    mrcal_stats_t stats = {.rms_reproj_error__pixels = -1.0 };

    if( !check_gradient )
    {
        stats.Noutliers_board              = 0;
        stats.Noutliers_triangulated_point = 0;

        const int Nmeasurements_board =
            mrcal_num_measurements_boards(Nobservations_board,
                                          calibration_object_width_n,
                                          calibration_object_height_n);
        for(int i=0; i<Nmeasurements_board/2; i++)
            if(observations_board_pool[i].z < 0.0)
                stats.Noutliers_board++;
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: check for point outliers here as well. Pull the triangulated-point outlier code out of markOutliers, and call it here (divergent-ray finder and looking at marked outliers)"
#endif

        double outliernessScale = -1.0;
        do
        {
            if(solver_context != NULL)
                dogleg_freeContext(&solver_context);

            norm2_error = dogleg_optimize2(packed_state.data(),
                                           Nstate, ctx.Nmeasurements, ctx.N_j_nonzero,
                                           (dogleg_callback_t*)&optimizer_callback, &ctx,
                                           &dogleg_parameters,
                                           &solver_context);

            if(norm2_error < 0) {
                // libdogleg barfed. I quit out
                if(solver_context != NULL)
                    dogleg_freeContext(&solver_context);

                return stats;
            }
#if 0
            // Not using dogleg_markOutliers() (yet...)

            if(outliernessScale < 0.0 && verbose)
                // These are for debug reporting
                dogleg_reportOutliers(getConfidence,
                                      &outliernessScale,
                                      2, Npoints_fromBoards,
                                      stats.Noutliers_board,
                                      solver_context->beforeStep, solver_context);
#endif

        } while( problem_selections.do_apply_outlier_rejection &&
                 markOutliers(observations_board_pool,
                              &stats.Noutliers_board,
                              &stats.Noutliers_triangulated_point,
                              observations_board,
                              Nobservations_board,
                              Nobservations_point,
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: not const for now. this will become const once the outlier bit moves to the point_triangulated pool"
#endif
                              (mrcal_observation_point_triangulated_t*)observations_point_triangulated,
                              Nobservations_point_triangulated,
                              calibration_object_width_n,
                              calibration_object_height_n,
                              solver_context->beforeStep->x,
                              extrinsics_fromref,
                              verbose) &&
                 ([=]{MSG("Threw out some outliers. New count = %d/%d (%.1f%%). Going again",
                       stats.Noutliers_board,
                       Nmeasurements_board,
                       (double)(stats.Noutliers_board * 100) / (double)Nmeasurements_board);
                   return true;}()));
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: the above print should deal with triangulated points too"
#endif


        // Done. I have the final state. I spit it back out
        unpack_solver_state( intrinsics,         // Ncameras_intrinsics of these
                             extrinsics_fromref, // Ncameras_extrinsics of these
                             frames_toref,       // Nframes of these
                             points,             // Npoints of these
                             calobject_warp,
                             packed_state.data(),
                             lensmodel,
                             problem_selections,
                             Ncameras_intrinsics, Ncameras_extrinsics,
                             Nframes, Npoints-Npoints_fixed,
                             Nobservations_board,
                             Nstate);

        double regularization_ratio_distortion  = 0.0;
        double regularization_ratio_centerpixel = 0.0;
        const int imeas_reg0 =
            mrcal_measurement_index_regularization(observations_point_triangulated,
                                                   Nobservations_point_triangulated,
                                                   calibration_object_width_n,
                                                   calibration_object_height_n,
                                                   Ncameras_intrinsics, Ncameras_extrinsics,
                                                   Nframes,
                                                   Npoints, Npoints_fixed, Nobservations_board, Nobservations_point,
                                                   problem_selections,
                                                   lensmodel);

        if(imeas_reg0 >= 0 && verbose)
        {
            // we have regularization
            const double* xreg = &solver_context->beforeStep->x[imeas_reg0];

            int Ncore = modelHasCore_fxfycxcy(lensmodel) ? 4 : 0;

            int Nmeasurements_regularization_distortion  = 0;
            if(problem_selections.do_optimize_intrinsics_distortions)
                Nmeasurements_regularization_distortion =
                    Ncameras_intrinsics*(ctx.Nintrinsics-Ncore);

            int Nmeasurements_regularization_centerpixel = 0;
            if(problem_selections.do_optimize_intrinsics_core)
                Nmeasurements_regularization_centerpixel =
                    Ncameras_intrinsics*2;

            double norm2_err_regularization_distortion     = 0;
            double norm2_err_regularization_centerpixel    = 0;

            for(int i=0; i<Nmeasurements_regularization_distortion; i++)
            {
                double x = *(xreg++);
                norm2_err_regularization_distortion += x*x;
            }
            for(int i=0; i<Nmeasurements_regularization_centerpixel; i++)
            {
                double x = *(xreg++);
                norm2_err_regularization_centerpixel += x*x;
            }

            regularization_ratio_distortion  = norm2_err_regularization_distortion      / norm2_error;
            regularization_ratio_centerpixel = norm2_err_regularization_centerpixel     / norm2_error;

            // These are important to the dev, but not to the end user. So I
            // disable these by default
            if(regularization_ratio_distortion > 0.01)
                MSG("WARNING: regularization ratio for lens distortion exceeds 1%%. Is the scale factor too high? Ratio = %.3g/%.3g = %.3g",
                    norm2_err_regularization_distortion,  norm2_error, regularization_ratio_distortion);
            if(regularization_ratio_centerpixel > 0.01)
                MSG("WARNING: regularization ratio for the projection centerpixel exceeds 1%%. Is the scale factor too high? Ratio = %.3g/%.3g = %.3g",
                    norm2_err_regularization_centerpixel, norm2_error, regularization_ratio_centerpixel);

            double regularization_ratio_unity_cam01 = 0.0;
            if(problem_selections.do_apply_regularization_unity_cam01 &&
               problem_selections.do_optimize_extrinsics &&
               Ncameras_extrinsics > 0)
            {
                int Nmeasurements_regularization_unity_cam01  = 1;
                double norm2_err_regularization_unity_cam01   = 0;

                for(int i=0; i<Nmeasurements_regularization_unity_cam01; i++)
                {
                    double x = *(xreg++);
                    norm2_err_regularization_unity_cam01 += x*x;
                }

                regularization_ratio_unity_cam01  = norm2_err_regularization_unity_cam01      / norm2_error;
                if(regularization_ratio_unity_cam01 > 0.01)
                    MSG("WARNING: regularization ratio for unity_cam01 exceeds 1%%. Is the scale factor too high? Ratio = %.3g/%.3g = %.3g",
                        norm2_err_regularization_unity_cam01,  norm2_error, regularization_ratio_unity_cam01);
            }

            assert(xreg == &solver_context->beforeStep->x[ctx.Nmeasurements]);


            // Disable this by default. Splined models have LOTS of
            // parameters, and I don't want to print them. Usually.
            //
            // for(int i=0; i<Nmeasurements_regularization; i++)
            // {
            //     double x = solver_context->beforeStep->x[ctx.Nmeasurements - Nmeasurements_regularization + i];
            //     MSG("regularization %d: %f (squared: %f)", i, x, x*x);
            // }
            MSG("reg err ratio (distortion,centerpixel): %.3g %.3g",
                regularization_ratio_distortion,
                regularization_ratio_centerpixel);

            if(problem_selections.do_apply_regularization_unity_cam01 &&
               problem_selections.do_optimize_extrinsics &&
               Ncameras_extrinsics > 0)
            {
                MSG("reg err ratio (unity_cam01): %.3g",
                    regularization_ratio_unity_cam01);
            }
        }
    }
    else
        for(int ivar=0; ivar<Nstate; ivar++)
            dogleg_testGradient(ivar, packed_state.data(),
                                Nstate, ctx.Nmeasurements, ctx.N_j_nonzero,
                                (dogleg_callback_t*)&optimizer_callback, &ctx);

    stats.rms_reproj_error__pixels =
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: not correct for triangulated points either: 1 measurement, not 2"
#endif
        // THIS IS NOT CORRECT FOR POINTS. I have 3 measurements for points currently
        sqrt(norm2_error / (double)ctx.Nmeasurements);

    if(b_packed_final)
        memcpy(b_packed_final, solver_context->beforeStep->p, Nstate*sizeof(double));
    if(x_final)
        memcpy(x_final, solver_context->beforeStep->x, ctx.Nmeasurements*sizeof(double));

    if(solver_context != NULL)
        dogleg_freeContext(&solver_context);

    return stats;
}

bool mrcal_write_cameramodel_file(const char* filename,
                                  const mrcal_cameramodel_t* cameramodel)
{
    bool result = false;
    FILE* fp = fopen(filename, "w");
    if(fp == NULL)
    {
        MSG("Couldn't open('%s')", filename);
        return false;
    }

    char lensmodel_string[1024];
    if(!mrcal_lensmodel_name(lensmodel_string, sizeof(lensmodel_string),
                             &cameramodel->lensmodel))
    {
        MSG("Couldn't construct lensmodel string. Unconfigured string: '%s'",
            mrcal_lensmodel_name_unconfigured(&cameramodel->lensmodel));
        if(fp != NULL)
            fclose(fp);
        return result;
    }

    int Nparams = mrcal_lensmodel_num_params(&cameramodel->lensmodel);
    if(Nparams<0)
    {
        MSG("Couldn't get valid Nparams from lensmodel string '%s'",
            lensmodel_string);
        if(fp != NULL)
            fclose(fp);
        return result;;
    }

    fprintf(fp, "{\n");
    fprintf(fp, "  'lensmodel':  '%s',\n", lensmodel_string);
    fprintf(fp, "  'intrinsics': [ ");
    for(int i=0; i<Nparams; i++)
        fprintf(fp, "%f, ", cameramodel->intrinsics[i]);
    fprintf(fp, "],\n");
    fprintf(fp, "  'imagersize': [ %u, %u ],\n",
            cameramodel->imagersize[0],
            cameramodel->imagersize[1]);
    fprintf(fp, "  'extrinsics': [ %f, %f, %f, %f, %f, %f ]\n",
            cameramodel->rt_cam_ref[0],
            cameramodel->rt_cam_ref[1],
            cameramodel->rt_cam_ref[2],
            cameramodel->rt_cam_ref[3],
            cameramodel->rt_cam_ref[4],
            cameramodel->rt_cam_ref[5]);

    fprintf(fp,"}\n");
    result = true;

    if(fp != NULL)
        fclose(fp);
    return result;
}


#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: fixed points should live in a separate array, instead of at the end of the 'points' array"
#endif
