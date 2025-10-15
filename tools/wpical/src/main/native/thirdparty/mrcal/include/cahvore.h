// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <stdbool.h>
#include "basic-geometry.h"

bool project_cahvore_internals( // outputs
                                mrcal_point3_t* __restrict pdistorted,
                                double*         __restrict dpdistorted_dintrinsics_nocore,
                                double*         __restrict dpdistorted_dp,

                                // inputs
                                const mrcal_point3_t* __restrict p,
                                const double* __restrict intrinsics_nocore,
                                const double cahvore_linearity);

