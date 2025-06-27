// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "basic-geometry.h"

// All of these return (0,0,0) if the rays are parallel or divergent, or if the
// intersection is behind either of the two cameras. No gradients are reported
// in that case

// Basic closest-approach-in-3D routine. This is the "Mid" method in
// "Triangulation: Why Optimize?", Seong Hun Lee and Javier Civera
// https://arxiv.org/abs/1907.11917
mrcal_point3_t
mrcal_triangulate_geometric(// outputs
                            // These all may be NULL
                            mrcal_point3_t* dm_dv0,
                            mrcal_point3_t* dm_dv1,
                            mrcal_point3_t* dm_dt01,

                            // inputs

                            // not-necessarily normalized vectors in the camera-0
                            // coord system
                            const mrcal_point3_t* v0,
                            const mrcal_point3_t* v1,
                            const mrcal_point3_t* t01);

// Minimize L2 pinhole reprojection error. Described in "Triangulation Made
// Easy", Peter Lindstrom, IEEE Conference on Computer Vision and Pattern
// Recognition, 2010. This is the "L2 img 5-iteration" method (but with only 2
// iterations) in "Triangulation: Why Optimize?", Seong Hun Lee and Javier
// Civera. https://arxiv.org/abs/1907.11917
// Lindstrom's paper recommends 2 iterations
mrcal_point3_t
mrcal_triangulate_lindstrom(// outputs
                            // These all may be NULL
                            mrcal_point3_t* dm_dv0,
                            mrcal_point3_t* dm_dv1,
                            mrcal_point3_t* dm_dRt01,

                            // inputs

                            // not-necessarily normalized vectors in the LOCAL
                            // coordinate system. This is different from the other
                            // triangulation routines
                            const mrcal_point3_t* v0_local,
                            const mrcal_point3_t* v1_local,
                            const mrcal_point3_t* Rt01);

// Minimize L1 angle error. Described in "Closed-Form Optimal Two-View
// Triangulation Based on Angular Errors", Seong Hun Lee and Javier Civera. ICCV
// 2019. This is the "L1 ang" method in "Triangulation: Why Optimize?", Seong
// Hun Lee and Javier Civera. https://arxiv.org/abs/1907.11917
mrcal_point3_t
mrcal_triangulate_leecivera_l1(// outputs
                               // These all may be NULL
                               mrcal_point3_t* dm_dv0,
                               mrcal_point3_t* dm_dv1,
                               mrcal_point3_t* dm_dt01,

                               // inputs

                               // not-necessarily normalized vectors in the camera-0
                               // coord system
                               const mrcal_point3_t* v0,
                               const mrcal_point3_t* v1,
                               const mrcal_point3_t* t01);

// Minimize L-infinity angle error. Described in "Closed-Form Optimal Two-View
// Triangulation Based on Angular Errors", Seong Hun Lee and Javier Civera. ICCV
// 2019. This is the "L-infinity ang" method in "Triangulation: Why Optimize?",
// Seong Hun Lee and Javier Civera. https://arxiv.org/abs/1907.11917
mrcal_point3_t
mrcal_triangulate_leecivera_linf(// outputs
                                 // These all may be NULL
                                 mrcal_point3_t* dm_dv0,
                                 mrcal_point3_t* dm_dv1,
                                 mrcal_point3_t* dm_dt01,

                                 // inputs

                                 // not-necessarily normalized vectors in the camera-0
                                 // coord system
                                 const mrcal_point3_t* v0,
                                 const mrcal_point3_t* v1,
                                 const mrcal_point3_t* t01);

// The "Mid2" method in "Triangulation: Why Optimize?", Seong Hun Lee and Javier
// Civera. https://arxiv.org/abs/1907.11917
mrcal_point3_t
mrcal_triangulate_leecivera_mid2(// outputs
                                 // These all may be NULL
                                 mrcal_point3_t* dm_dv0,
                                 mrcal_point3_t* dm_dv1,
                                 mrcal_point3_t* dm_dt01,

                                 // inputs

                                 // not-necessarily normalized vectors in the camera-0
                                 // coord system
                                 const mrcal_point3_t* v0,
                                 const mrcal_point3_t* v1,
                                 const mrcal_point3_t* t01);

// The "wMid2" method in "Triangulation: Why Optimize?", Seong Hun Lee and Javier
// Civera. https://arxiv.org/abs/1907.11917
mrcal_point3_t
mrcal_triangulate_leecivera_wmid2(// outputs
                                  // These all may be NULL
                                  mrcal_point3_t* dm_dv0,
                                  mrcal_point3_t* dm_dv1,
                                  mrcal_point3_t* dm_dt01,

                                  // inputs

                                  // not-necessarily normalized vectors in the camera-0
                                  // coord system
                                  const mrcal_point3_t* v0,
                                  const mrcal_point3_t* v1,
                                  const mrcal_point3_t* t01);

// I don't implement triangulate_leecivera_l2() yet because it requires
// computing an SVD, which is far slower than what the rest of these functions
// do

double
_mrcal_triangulated_error(// outputs
                          mrcal_point3_t* _derr_dv1,
                          mrcal_point3_t* _derr_dt01,

                          // inputs

                          // not-necessarily normalized vectors in the camera-0
                          // coord system
                          const mrcal_point3_t* _v0,
                          const mrcal_point3_t* _v1,
                          const mrcal_point3_t* _t01);

bool
_mrcal_triangulate_leecivera_mid2_is_convergent(// inputs

                                                // not-necessarily normalized vectors in the camera-0
                                                // coord system
                                                const mrcal_point3_t* _v0,
                                                const mrcal_point3_t* _v1,
                                                const mrcal_point3_t* _t01);
