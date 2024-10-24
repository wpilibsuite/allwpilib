// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

// This is an internal header to make the stride logic work. Not to be seen by
// the end-users or installed

// stride-aware matrix access
#define _P1(x, stride0, i0)                     \
    (*(double*)( (char*)(x) +                   \
    (i0) * (stride0)))
#define _P2(x, stride0, stride1, i0, i1)        \
    (*(double*)( (char*)(x) +                   \
    (i0) * (stride0) +                          \
    (i1) * (stride1)))
#define _P3(x, stride0, stride1, stride2,i0, i1, i2)    \
    (*(double*)( (char*)(x) +                           \
    (i0) * (stride0) +                                  \
    (i1) * (stride1) +                                  \
    (i2) * (stride2)))

#define P1(x, i0)       _P1(x, x##_stride0,                           i0)
#define P2(x, i0,i1)    _P2(x, x##_stride0, x##_stride1,              i0,i1)
#define P3(x, i0,i1,i2) _P3(x, x##_stride0, x##_stride1, x##_stride2, i0,i1,i2)

// Init strides. If a given stride is <= 0, set the default, as we would have if
// the data was contiguous
#define init_stride_1D(x, d0) \
    if( x ## _stride0 <= 0) x ## _stride0 = sizeof(*x)
#define init_stride_2D(x, d0, d1)                       \
    if( x ## _stride1 <= 0) x ## _stride1 = sizeof(*x); \
    if( x ## _stride0 <= 0) x ## _stride0 = d1 * x ## _stride1
#define init_stride_3D(x, d0, d1, d2)                   \
    if( x ## _stride2 <= 0) x ## _stride2 = sizeof(*x); \
    if( x ## _stride1 <= 0) x ## _stride1 = d2 * x ## _stride2; \
    if( x ## _stride0 <= 0) x ## _stride0 = d1 * x ## _stride1
