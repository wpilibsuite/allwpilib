// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

// THESE ARE NOT A PART OF THE EXTERNAL API. Exported for the mrcal python
// wrapper only

// These models have no precomputed data
typedef struct { int dummy; } mrcal_LENSMODEL_PINHOLE__precomputed_t;
typedef struct { int dummy; } mrcal_LENSMODEL_STEREOGRAPHIC__precomputed_t;
typedef struct { int dummy; } mrcal_LENSMODEL_LONLAT__precomputed_t;
typedef struct { int dummy; } mrcal_LENSMODEL_LATLON__precomputed_t;
typedef struct { int dummy; } mrcal_LENSMODEL_OPENCV4__precomputed_t;
typedef struct { int dummy; } mrcal_LENSMODEL_OPENCV5__precomputed_t;
typedef struct { int dummy; } mrcal_LENSMODEL_OPENCV8__precomputed_t;
typedef struct { int dummy; } mrcal_LENSMODEL_OPENCV12__precomputed_t;
typedef struct { int dummy; } mrcal_LENSMODEL_CAHVOR__precomputed_t;
typedef struct { int dummy; } mrcal_LENSMODEL_CAHVORE__precomputed_t;

// The splined stereographic models configuration parameters can be used to
// compute the segment size. I cache this computation
typedef struct
{
    // The distance between adjacent knots (1 segment) is u_per_segment =
    // 1/segments_per_u
    double segments_per_u;
} mrcal_LENSMODEL_SPLINED_STEREOGRAPHIC__precomputed_t;

typedef struct
{
    bool ready;
    union
    {
#define PRECOMPUTED_STRUCT(s,n) mrcal_ ##s##__precomputed_t s##__precomputed;
        MRCAL_LENSMODEL_LIST(PRECOMPUTED_STRUCT)
#undef PRECOMPUTED_STRUCT
    };
} mrcal_projection_precomputed_t;


void _mrcal_project_internal_opencv( // outputs
                                    mrcal_point2_t* q,
                                    mrcal_point3_t* dq_dp,               // may be NULL
                                    double* dq_dintrinsics_nocore, // may be NULL

                                    // inputs
                                    const mrcal_point3_t* p,
                                    int N,
                                    const double* intrinsics,
                                    int Nintrinsics);
bool _mrcal_project_internal( // out
                             mrcal_point2_t* q,

                             // Stored as a row-first array of shape (N,2,3). Each
                             // trailing ,3 dimension element is a mrcal_point3_t
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
                             const mrcal_projection_precomputed_t* precomputed);
void _mrcal_precompute_lensmodel_data(mrcal_projection_precomputed_t* precomputed,
                                      const mrcal_lensmodel_t* lensmodel);
bool _mrcal_unproject_internal( // out
                               mrcal_point3_t* out,

                               // in
                               const mrcal_point2_t* q,
                               int N,
                               const mrcal_lensmodel_t* lensmodel,
                               // core, distortions concatenated
                               const double* intrinsics,
                               const mrcal_projection_precomputed_t* precomputed);

// Report the number of non-zero entries in the optimization jacobian
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
                         const mrcal_lensmodel_t* lensmodel);
