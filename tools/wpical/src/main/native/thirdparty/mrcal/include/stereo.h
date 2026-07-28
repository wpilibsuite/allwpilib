// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "mrcal-types.h"
#include "mrcal-image.h"

// The reference implementation in Python is _rectified_resolution_python() in
// stereo.py
//
// The Python wrapper is mrcal.rectified_resolution(), and the documentation is
// in the docstring of that function
bool mrcal_rectified_resolution( // output and input
                                 // > 0: use given value
                                 // < 0: autodetect and scale
                                 double* pixels_per_deg_az,
                                 double* pixels_per_deg_el,

                                 // input
                                 const mrcal_lensmodel_t*     lensmodel,
                                 const double*                intrinsics,
                                 const mrcal_point2_t*        azel_fov_deg,
                                 const mrcal_point2_t*        azel0_deg,
                                 const double*                R_cam0_rect0,
                                 const mrcal_lensmodel_type_t rectification_model_type);

// The reference implementation in Python is _rectified_system_python() in
// stereo.py
//
// The Python wrapper is mrcal.rectified_system(), and the documentation is in
// the docstring of that function
bool mrcal_rectified_system(// output
                            unsigned int*     imagersize_rectified,
                            double*           fxycxy_rectified,
                            double*           rt_rect0_ref,
                            double*           baseline,

                            // input, output
                            // > 0: use given value
                            // < 0: autodetect and scale
                            double* pixels_per_deg_az,
                            double* pixels_per_deg_el,

                            // input, output
                            // if(..._autodetect) { the results are returned here }
                            mrcal_point2_t* azel_fov_deg,
                            mrcal_point2_t* azel0_deg,

                            // input
                            const mrcal_lensmodel_t* lensmodel0,
                            const double*            intrinsics0,

                            const double*            rt_cam0_ref,
                            const double*            rt_cam1_ref,

                            const mrcal_lensmodel_type_t rectification_model_type,

                            bool   az0_deg_autodetect,
                            bool   el0_deg_autodetect,
                            bool   az_fov_deg_autodetect,
                            bool   el_fov_deg_autodetect);

// The reference implementation in Python is _rectification_maps_python() in
// stereo.py
//
// The Python wrapper is mrcal.rectification_maps(), and the documentation is in
// the docstring of that function
bool mrcal_rectification_maps(// output
                              // Dense array of shape (Ncameras=2, Nel, Naz, Nxy=2)
                              float* rectification_maps,

                              // input
                              const mrcal_lensmodel_t* lensmodel0,
                              const double*            intrinsics0,
                              const double*            r_cam0_ref,

                              const mrcal_lensmodel_t* lensmodel1,
                              const double*            intrinsics1,
                              const double*            r_cam1_ref,

                              const mrcal_lensmodel_type_t rectification_model_type,
                              const double*                fxycxy_rectified,
                              const unsigned int*          imagersize_rectified,
                              const double*                r_rect0_ref);


// The reference implementation in Python is _stereo_range_python() in
// stereo.py
//
// The Python wrapper is mrcal.stereo_range(), and the documentation is in
// the docstring of that function
bool mrcal_stereo_range_sparse(// output
                               double* range, // N of these

                               // input
                               const double*                disparity, // N of these
                               const mrcal_point2_t*        qrect0,    // N of these
                               const int                    N,         // how many points

                               const double                 disparity_min,
                               const double                 disparity_max,

                               // models_rectified
                               const mrcal_lensmodel_type_t rectification_model_type,
                               const double*                fxycxy_rectified,
                               const double                 baseline);

bool mrcal_stereo_range_dense(// output
                              mrcal_image_double_t* range,

                              // input
                              const mrcal_image_uint16_t*  disparity_scaled,
                              const uint16_t               disparity_scale,

                              // Used to detect invalid values. Set to
                              // 0,UINT16_MAX to ignore
                              const uint16_t               disparity_scaled_min,
                              const uint16_t               disparity_scaled_max,

                              // models_rectified
                              const mrcal_lensmodel_type_t rectification_model_type,
                              const double*                fxycxy_rectified,
                              const double                 baseline);
