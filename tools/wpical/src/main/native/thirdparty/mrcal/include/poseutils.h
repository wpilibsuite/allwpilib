// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <stdbool.h>

// Unless specified all arrays stored in contiguous matrices in row-major order.
//
// All functions are defined using the mrcal_..._full() form, which supports
// non-contiguous input and output arrays, and some optional arguments. Strides
// are used to specify the array layout.
//
// All functions have a convenience wrapper macro that is a simpler way to call
// the function, usable with contiguous arrays and defaults.
//
// All the functions use double-precision floating point to store the data, and
// C ints to store strides. The strides are given in bytes. In the
// mrcal_..._full() functions, each array is followed by the strides, one per
// dimension.
//
// I have two different representations of pose transformations:
//
// - Rt is a concatenated (4,3) array: Rt = nps.glue(R,t, axis=-2). The
//   transformation is R*x+t
//
// - rt is a concatenated (6,) array: rt = nps.glue(r,t, axis=-1). The
//   transformation is R*x+t where R = R_from_r(r)
//
// I treat all vectors as column vectors, so matrix multiplication works from
// the left: to rotate a vector x by a rotation matrix R I have
//
//   x_rotated = R * x


// Store an identity rotation matrix into the given (3,3) array
//
// This is simply an identity matrix
#define mrcal_identity_R(R) mrcal_identity_R_full(R,0,0)
void mrcal_identity_R_full(double* R,      // (3,3) array
                           int R_stride0,  // in bytes. <= 0 means "contiguous"
                           int R_stride1   // in bytes. <= 0 means "contiguous"
                           );

// Store an identity rodrigues rotation into the given (3,) array
//
// This is simply an array of zeros
#define mrcal_identity_r(r) mrcal_identity_r_full(r,0)
void mrcal_identity_r_full(double* r,      // (3,) array
                           int r_stride0   // in bytes. <= 0 means "contiguous"
                           );

// Store an identity Rt transformation into the given (4,3) array
#define mrcal_identity_Rt(Rt) mrcal_identity_Rt_full(Rt,0,0)
void mrcal_identity_Rt_full(double* Rt,      // (4,3) array
                            int Rt_stride0,  // in bytes. <= 0 means "contiguous"
                            int Rt_stride1   // in bytes. <= 0 means "contiguous"
                            );

// Store an identity rt transformation into the given (6,) array
#define mrcal_identity_rt(rt)  mrcal_identity_rt_full(rt,0)
void mrcal_identity_rt_full(double* rt,      // (6,) array
                            int rt_stride0   // in bytes. <= 0 means "contiguous"
                            );

// Rotate the point x_in in a (3,) array by the rotation matrix R in a (3,3)
// array. This is simply the matrix-vector multiplication R x_in
//
// The result is returned in a (3,) array x_out.
//
// The gradient dx_out/dR is returned in a (3, 3,3) array J_R. Set to NULL if
// this is not wanted
//
// The gradient dx_out/dx_in is returned in a (3,3) array J_x. This is simply
// the matrix R. Set to NULL if this is not wanted
//
// In-place operation is supported; the output array may be the same as the
// input arrays to overwrite the input.
#define mrcal_rotate_point_R(         x_out,J_R,J_x,R,x_in) mrcal_rotate_point_R_full(x_out,0,J_R,0,0,0,J_x,0,0,R,0,0,x_in,0, false)
#define mrcal_rotate_point_R_inverted(x_out,J_R,J_x,R,x_in) mrcal_rotate_point_R_full(x_out,0,J_R,0,0,0,J_x,0,0,R,0,0,x_in,0, true)
void mrcal_rotate_point_R_full( // output
                               double* x_out,      // (3,) array
                               int x_out_stride0,  // in bytes. <= 0 means "contiguous"
                               double* J_R,        // (3,3,3) array. May be NULL
                               int J_R_stride0,    // in bytes. <= 0 means "contiguous"
                               int J_R_stride1,    // in bytes. <= 0 means "contiguous"
                               int J_R_stride2,    // in bytes. <= 0 means "contiguous"
                               double* J_x,        // (3,3) array. May be NULL
                               int J_x_stride0,    // in bytes. <= 0 means "contiguous"
                               int J_x_stride1,    // in bytes. <= 0 means "contiguous"

                               // input
                               const double* R,    // (3,3) array. May be NULL
                               int R_stride0,      // in bytes. <= 0 means "contiguous"
                               int R_stride1,      // in bytes. <= 0 means "contiguous"
                               const double* x_in, // (3,) array. May be NULL
                               int x_in_stride0,   // in bytes. <= 0 means "contiguous"

                               bool inverted       // if true, I apply a
                                                   // rotation in the opposite
                                                   // direction. J_R corresponds
                                                   // to the input R
                               );

// Rotate the point x_in in a (3,) array by the rodrigues rotation in a (3,)
// array.
//
// The result is returned in a (3,) array x_out.
//
// The gradient dx_out/dr is returned in a (3,3) array J_r. Set to NULL if this
// is not wanted
//
// The gradient dx_out/dx_in is returned in a (3,3) array J_x. Set to NULL if
// this is not wanted
//
// In-place operation is supported; the output array may be the same as the
// input arrays to overwrite the input.
#define mrcal_rotate_point_r(         x_out,J_r,J_x,r,x_in) mrcal_rotate_point_r_full(x_out,0,J_r,0,0,J_x,0,0,r,0,x_in,0, false)
#define mrcal_rotate_point_r_inverted(x_out,J_r,J_x,r,x_in) mrcal_rotate_point_r_full(x_out,0,J_r,0,0,J_x,0,0,r,0,x_in,0, true)
void mrcal_rotate_point_r_full( // output
                               double* x_out,      // (3,) array
                               int x_out_stride0,  // in bytes. <= 0 means "contiguous"
                               double* J_r,        // (3,3) array. May be NULL
                               int J_r_stride0,    // in bytes. <= 0 means "contiguous"
                               int J_r_stride1,    // in bytes. <= 0 means "contiguous"
                               double* J_x,        // (3,3) array. May be NULL
                               int J_x_stride0,    // in bytes. <= 0 means "contiguous"
                               int J_x_stride1,    // in bytes. <= 0 means "contiguous"

                               // input
                               const double* r,    // (3,) array. May be NULL
                               int r_stride0,      // in bytes. <= 0 means "contiguous"
                               const double* x_in, // (3,) array. May be NULL
                               int x_in_stride0,   // in bytes. <= 0 means "contiguous"

                               bool inverted       // if true, I apply a
                                                   // rotation in the opposite
                                                   // direction. J_r corresponds
                                                   // to the input r
                               );


// Transform the point x_in in a (3,) array by the Rt transformation in a (4,3)
// array.
//
// The result is returned in a (3,) array x_out.
//
// The gradient dx_out/dRt is returned in a (3, 4,3) array J_Rt. Set to NULL if
// this is not wanted
//
// The gradient dx_out/dx_in is returned in a (3,3) array J_x. This is simply
// the matrix R. Set to NULL if this is not wanted
//
// In-place operation is supported; the output array may be the same as the
// input arrays to overwrite the input.
#define mrcal_transform_point_Rt(         x_out,J_Rt,J_x,Rt,x_in) mrcal_transform_point_Rt_full(x_out,0,J_Rt,0,0,0,J_x,0,0,Rt,0,0,x_in,0, false)
#define mrcal_transform_point_Rt_inverted(x_out,J_Rt,J_x,Rt,x_in) mrcal_transform_point_Rt_full(x_out,0,J_Rt,0,0,0,J_x,0,0,Rt,0,0,x_in,0, true)
void mrcal_transform_point_Rt_full( // output
                                   double* x_out,      // (3,) array
                                   int x_out_stride0,  // in bytes. <= 0 means "contiguous"
                                   double* J_Rt,       // (3,4,3) array. May be NULL
                                   int J_Rt_stride0,   // in bytes. <= 0 means "contiguous"
                                   int J_Rt_stride1,   // in bytes. <= 0 means "contiguous"
                                   int J_Rt_stride2,   // in bytes. <= 0 means "contiguous"
                                   double* J_x,        // (3,3) array. May be NULL
                                   int J_x_stride0,    // in bytes. <= 0 means "contiguous"
                                   int J_x_stride1,    // in bytes. <= 0 means "contiguous"

                                   // input
                                   const double* Rt,   // (4,3) array. May be NULL
                                   int Rt_stride0,     // in bytes. <= 0 means "contiguous"
                                   int Rt_stride1,     // in bytes. <= 0 means "contiguous"
                                   const double* x_in, // (3,) array. May be NULL
                                   int x_in_stride0,   // in bytes. <= 0 means "contiguous"

                                   bool inverted       // if true, I apply a
                                                       // transformation in the opposite
                                                       // direction. J_Rt corresponds
                                                       // to the input Rt
                                   );

// Transform the point x_in in a (3,) array by the rt transformation in a (6,)
// array.
//
// The result is returned in a (3,) array x_out.
//
// The gradient dx_out/drt is returned in a (3,6) array J_rt. Set to NULL if
// this is not wanted
//
// The gradient dx_out/dx_in is returned in a (3,3) array J_x. This is simply
// the matrix R. Set to NULL if this is not wanted
//
// In-place operation is supported; the output array may be the same as the
// input arrays to overwrite the input.
#define mrcal_transform_point_rt(         x_out,J_rt,J_x,rt,x_in) mrcal_transform_point_rt_full(x_out,0,J_rt,0,0,J_x,0,0,rt,0,x_in,0, false)
#define mrcal_transform_point_rt_inverted(x_out,J_rt,J_x,rt,x_in) mrcal_transform_point_rt_full(x_out,0,J_rt,0,0,J_x,0,0,rt,0,x_in,0, true)
void mrcal_transform_point_rt_full( // output
                                   double* x_out,      // (3,) array
                                   int x_out_stride0,  // in bytes. <= 0 means "contiguous"
                                   double* J_rt,       // (3,6) array. May be NULL
                                   int J_rt_stride0,   // in bytes. <= 0 means "contiguous"
                                   int J_rt_stride1,   // in bytes. <= 0 means "contiguous"
                                   double* J_x,        // (3,3) array. May be NULL
                                   int J_x_stride0,    // in bytes. <= 0 means "contiguous"
                                   int J_x_stride1,    // in bytes. <= 0 means "contiguous"

                                   // input
                                   const double* rt,   // (6,) array. May be NULL
                                   int rt_stride0,     // in bytes. <= 0 means "contiguous"
                                   const double* x_in, // (3,) array. May be NULL
                                   int x_in_stride0,   // in bytes. <= 0 means "contiguous"

                                   bool inverted       // if true, I apply the
                                                       // transformation in the
                                                       // opposite direction.
                                                       // J_rt corresponds to
                                                       // the input rt
                                   );

// Convert a rotation matrix in a (3,3) array to a rodrigues vector in a (3,)
// array
//
// The result is returned in a (3,) array r
//
// The gradient dr/dR is returned in a (3, 3,3) array J. Set to NULL if this is
// not wanted
#define mrcal_r_from_R(r,J,R) mrcal_r_from_R_full(r,0,J,0,0,0,R,0,0)
void mrcal_r_from_R_full( // output
                         double* r,       // (3,) vector
                         int r_stride0,   // in bytes. <= 0 means "contiguous"
                         double* J,       // (3,3,3) array. Gradient. May be NULL
                         int J_stride0,   // in bytes. <= 0 means "contiguous"
                         int J_stride1,   // in bytes. <= 0 means "contiguous"
                         int J_stride2,   // in bytes. <= 0 means "contiguous"

                         // input
                         const double* R, // (3,3) array
                         int R_stride0,   // in bytes. <= 0 means "contiguous"
                         int R_stride1    // in bytes. <= 0 means "contiguous"
                         );

// Convert a rodrigues vector in a (3,) array to a rotation matrix in a (3,3)
// array
//
// The result is returned in a (3,3) array R
//
// The gradient dR/dr is returned in a (3,3 ,3) array J. Set to NULL if this is
// not wanted
#define mrcal_R_from_r(R,J,r) mrcal_R_from_r_full(R,0,0,J,0,0,0,r,0)
void mrcal_R_from_r_full( // outputs
                         double* R,       // (3,3) array
                         int R_stride0,   // in bytes. <= 0 means "contiguous"
                         int R_stride1,   // in bytes. <= 0 means "contiguous"
                         double* J,       // (3,3,3) array. Gradient. May be NULL
                         int J_stride0,   // in bytes. <= 0 means "contiguous"
                         int J_stride1,   // in bytes. <= 0 means "contiguous"
                         int J_stride2,   // in bytes. <= 0 means "contiguous"

                         // input
                         const double* r, // (3,) vector
                         int r_stride0    // in bytes. <= 0 means "contiguous"
                          );

// Invert a rotation matrix. This is a transpose
//
// The input is given in R_in in a (3,3) array
//
// The result is returned in a (3,3) array R_out
//
// In-place operation is supported; the output array may be the same as the
// input arrays to overwrite the input.
#define mrcal_invert_R(R_out,R_in) mrcal_invert_R_full(R_out,0,0,R_in,0,0)
void mrcal_invert_R_full( // output
                         double* R_out,      // (3,3) array
                         int R_out_stride0,  // in bytes. <= 0 means "contiguous"
                         int R_out_stride1,  // in bytes. <= 0 means "contiguous"

                         // input
                         const double* R_in, // (3,3) array
                         int R_in_stride0,   // in bytes. <= 0 means "contiguous"
                         int R_in_stride1    // in bytes. <= 0 means "contiguous"
                         );

// Convert an Rt transformation in a (4,3) array to an rt transformation in a
// (6,) array
//
// The result is returned in a (6,) array rt
//
// The gradient dr/dR is returned in a (3, 3,3) array J_R. Set to NULL if this
// is not wanted
//
// The t terms are identical, so dt/dt = identity and I do not return it
//
// The r and R terms are independent of the t terms, so dr/dt and dt/dR are both
// 0, and I do not return them
#define mrcal_rt_from_Rt(rt,J_R,Rt) mrcal_rt_from_Rt_full(rt,0,J_R,0,0,0,Rt,0,0)
void mrcal_rt_from_Rt_full(   // output
                           double* rt,      // (6,) vector
                           int rt_stride0,  // in bytes. <= 0 means "contiguous"
                           double* J_R,     // (3,3,3) array. Gradient. May be NULL
                           // No J_t. It's always the identity
                           int J_R_stride0, // in bytes. <= 0 means "contiguous"
                           int J_R_stride1, // in bytes. <= 0 means "contiguous"
                           int J_R_stride2, // in bytes. <= 0 means "contiguous"

                           // input
                           const double* Rt,  // (4,3) array
                           int Rt_stride0,    // in bytes. <= 0 means "contiguous"
                           int Rt_stride1     // in bytes. <= 0 means "contiguous"
                           );

// Convert an rt transformation in a (6,) array to an Rt transformation in a
// (4,3) array
//
// The result is returned in a (4,3) array Rt
//
// The gradient dR/dr is returned in a (3,3 ,3) array J_r. Set to NULL if this
// is not wanted
//
// The t terms are identical, so dt/dt = identity and I do not return it
//
// The r and R terms are independent of the t terms, so dR/dt and dt/dr are both
// 0, and I do not return them
#define mrcal_Rt_from_rt(Rt,J_r,rt) mrcal_Rt_from_rt_full(Rt,0,0,J_r,0,0,0,rt,0)
void mrcal_Rt_from_rt_full(   // output
                           double* Rt,      // (4,3) array
                           int Rt_stride0,  // in bytes. <= 0 means "contiguous"
                           int Rt_stride1,  // in bytes. <= 0 means "contiguous"
                           double* J_r,     // (3,3,3) array. Gradient. May be NULL
                           // No J_t. It's just the identity
                           int J_r_stride0, // in bytes. <= 0 means "contiguous"
                           int J_r_stride1, // in bytes. <= 0 means "contiguous"
                           int J_r_stride2, // in bytes. <= 0 means "contiguous"

                           // input
                           const double* rt, // (6,) vector
                           int rt_stride0    // in bytes. <= 0 means "contiguous"
                           );

// Invert an Rt transformation
//
// The input is given in Rt_in in a (4,3) array
//
// The result is returned in a (4,3) array Rt_out
//
// In-place operation is supported; the output array may be the same as the
// input arrays to overwrite the input.
#define mrcal_invert_Rt(Rt_out,Rt_in) mrcal_invert_Rt_full(Rt_out,0,0,Rt_in,0,0)
void mrcal_invert_Rt_full( // output
                          double* Rt_out,      // (4,3) array
                          int Rt_out_stride0,  // in bytes. <= 0 means "contiguous"
                          int Rt_out_stride1,  // in bytes. <= 0 means "contiguous"

                          // input
                          const double* Rt_in, // (4,3) array
                          int Rt_in_stride0,   // in bytes. <= 0 means "contiguous"
                          int Rt_in_stride1    // in bytes. <= 0 means "contiguous"
                          );

// Invert an rt transformation
//
// The input is given in rt_in in a (6,) array
//
// The result is returned in a (6,) array rt_out
//
// The gradient dtout/drin is returned in a (3,3) array dtout_drin. Set to NULL
// if this is not wanted
//
// The gradient dtout/dtin is returned in a (3,3) array dtout_dtin. Set to NULL
// if this is not wanted
//
// The gradient drout/drin is always -identity. So it is not returned
//
// The gradient drout/dtin is always 0. So it is not returned
//
// In-place operation is supported; the output array may be the same as the
// input arrays to overwrite the input.
#define mrcal_invert_rt(rt_out,dtout_drin,dtout_dtin,rt_in) mrcal_invert_rt_full(rt_out,0,dtout_drin,0,0,dtout_dtin,0,0,rt_in,0)
void mrcal_invert_rt_full( // output
                          double* rt_out,          // (6,) array
                          int rt_out_stride0,      // in bytes. <= 0 means "contiguous"
                          double* dtout_drin,      // (3,3) array
                          int dtout_drin_stride0,  // in bytes. <= 0 means "contiguous"
                          int dtout_drin_stride1,  // in bytes. <= 0 means "contiguous"
                          double* dtout_dtin,      // (3,3) array
                          int dtout_dtin_stride0,  // in bytes. <= 0 means "contiguous"
                          int dtout_dtin_stride1,  // in bytes. <= 0 means "contiguous"

                          // input
                          const double* rt_in,     // (6,) array
                          int rt_in_stride0        // in bytes. <= 0 means "contiguous"
                          );

// Compose two Rt transformations
//
// Rt = Rt0 * Rt1  --->  Rt(x) = Rt0( Rt1(x) )
//
// The input transformations are given in (4,3) arrays Rt_0 and Rt_1
//
// The result is returned in a (4,3) array Rt_out
//
// In-place operation is supported; the output array may be the same as either
// of the input arrays to overwrite the input.
#define mrcal_compose_Rt(Rt_out,Rt_0,Rt_1) mrcal_compose_Rt_full(Rt_out,0,0,Rt_0,0,0,Rt_1,0,0)
void mrcal_compose_Rt_full( // output
                           double* Rt_out,      // (4,3) array
                           int Rt_out_stride0,  // in bytes. <= 0 means "contiguous"
                           int Rt_out_stride1,  // in bytes. <= 0 means "contiguous"

                           // input
                           const double* Rt_0,  // (4,3) array
                           int Rt_0_stride0,    // in bytes. <= 0 means "contiguous"
                           int Rt_0_stride1,    // in bytes. <= 0 means "contiguous"
                           const double* Rt_1,  // (4,3) array
                           int Rt_1_stride0,    // in bytes. <= 0 means "contiguous"
                           int Rt_1_stride1     // in bytes. <= 0 means "contiguous"
                           );

// Compose two rt transformations
//
// rt = rt0 * rt1  --->  rt(x) = rt0( rt1(x) )
//
// The input transformations are given in (6,) arrays rt_0 and rt_1
//
// The result is returned in a (6,) array rt_out
//
// The gradient dr/dr0 is returned in a (3,3) array dr_dr0. Set to NULL if this
// is not wanted
//
// The gradient dr/dr1 is returned in a (3,3) array dr_dr1. Set to NULL if this
// is not wanted
//
// The gradient dt/dr0 is returned in a (3,3) array dt_dr0. Set to NULL if this
// is not wanted
//
// The gradient dt/dt1 is returned in a (3,3) array dt_dt1. Set to NULL if this
// is not wanted
//
// The gradients dr/dt0, dr/dt1, dt/dr1 are always 0, so they are never returned
//
// The gradient dt/dt0 is always identity, so it is never returned
//
// In-place operation is supported; the output array may be the same as either
// of the input arrays to overwrite the input.
#define mrcal_compose_rt(rt_out,dr_dr0,dr_dr1,dt_dr0,dt_dt1,rt_0,rt_1) mrcal_compose_rt_full(rt_out,0,dr_dr0,0,0,dr_dr1,0,0,dt_dr0,0,0,dt_dt1,0,0,rt_0,0,rt_1,0)
void mrcal_compose_rt_full( // output
                           double* rt_out,       // (6,) array
                           int rt_out_stride0,   // in bytes. <= 0 means "contiguous"
                           double* dr_dr0,       // (3,3) array; may be NULL
                           int dr_dr0_stride0,   // in bytes. <= 0 means "contiguous"
                           int dr_dr0_stride1,   // in bytes. <= 0 means "contiguous"
                           double* dr_dr1,       // (3,3) array; may be NULL
                           int dr_dr1_stride0,   // in bytes. <= 0 means "contiguous"
                           int dr_dr1_stride1,   // in bytes. <= 0 means "contiguous"
                           double* dt_dr0,       // (3,3) array; may be NULL
                           int dt_dr0_stride0,   // in bytes. <= 0 means "contiguous"
                           int dt_dr0_stride1,   // in bytes. <= 0 means "contiguous"
                           double* dt_dt1,       // (3,3) array; may be NULL
                           int dt_dt1_stride0,   // in bytes. <= 0 means "contiguous"
                           int dt_dt1_stride1,   // in bytes. <= 0 means "contiguous"

                           // input
                           const double* rt_0,   // (6,) array
                           int rt_0_stride0,     // in bytes. <= 0 means "contiguous"
                           const double* rt_1,   // (6,) array
                           int rt_1_stride0      // in bytes. <= 0 means "contiguous"
                           );

// Compose two angle-axis rotations
//
// r = r0 * r1  --->  r(x) = r0( r1(x) )
//
// The input rotations are given in (3,) arrays r_0 and r_1
//
// The result is returned in a (3,) array r_out
//
// The gradient dr/dr0 is returned in a (3,3) array dr_dr0. Set to NULL if this
// is not wanted
//
// The gradient dr/dr1 is returned in a (3,3) array dr_dr1. Set to NULL if this
// is not wanted
//
// In-place operation is supported; the output array may be the same as either
// of the input arrays to overwrite the input.
#define mrcal_compose_r(r_out,dr_dr0,dr_dr1,r_0,r_1) mrcal_compose_r_full(r_out,0,dr_dr0,0,0,dr_dr1,0,0,r_0,0,r_1,0)
void mrcal_compose_r_full( // output
                           double* r_out,       // (3,) array
                           int r_out_stride0,   // in bytes. <= 0 means "contiguous"
                           double* dr_dr0,      // (3,3) array; may be NULL
                           int dr_dr0_stride0,  // in bytes. <= 0 means "contiguous"
                           int dr_dr0_stride1,  // in bytes. <= 0 means "contiguous"
                           double* dr_dr1,      // (3,3) array; may be NULL
                           int dr_dr1_stride0,  // in bytes. <= 0 means "contiguous"
                           int dr_dr1_stride1,  // in bytes. <= 0 means "contiguous"

                           // input
                           const double* r_0,   // (3,) array
                           int r_0_stride0,     // in bytes. <= 0 means "contiguous"
                           const double* r_1,   // (3,) array
                           int r_1_stride0      // in bytes. <= 0 means "contiguous"
                           );

// Special-case rotation compositions for the uncertainty computation
//
// Same as mrcal_compose_r() except
//
// - r0 is assumed to be 0, so we don't ingest it, and we don't report the
//   composition result
// - we ONLY report the dr01/dr0 gradient
//
// In python this function is equivalent to
//
//   _,dr01_dr0,_ = compose_r(np.zeros((3,),),
//                            r1,
//                            get_gradients=True)
#define mrcal_compose_r_tinyr0_gradientr0(dr_dr0,r_1) \
    mrcal_compose_r_tinyr0_gradientr0_full(dr_dr0,0,0,r_1,0)
void mrcal_compose_r_tinyr0_gradientr0_full( // output
                           double* dr_dr0,      // (3,3) array; may be NULL
                           int dr_dr0_stride0,  // in bytes. <= 0 means "contiguous"
                           int dr_dr0_stride1,  // in bytes. <= 0 means "contiguous"

                           // input
                           const double* r_1,   // (3,) array
                           int r_1_stride0      // in bytes. <= 0 means "contiguous"
                           );
// Same as mrcal_compose_r() except
//
// - r1 is assumed to be 0, so we don't ingest it, and we don't report the
//   composition result
// - we ONLY report the dr01/dr1 gradient
//
// In python this function is equivalent to
//
//   _,_,dr01_dr1 = compose_r(r0,
//                            np.zeros((3,),),
//                            get_gradients=True)
#define mrcal_compose_r_tinyr1_gradientr1(dr_dr1,r_0) \
    mrcal_compose_r_tinyr1_gradientr1_full(dr_dr1,0,0,r_0,0)
void mrcal_compose_r_tinyr1_gradientr1_full( // output
                           double* dr_dr1,      // (3,3) array; may be NULL
                           int dr_dr1_stride0,  // in bytes. <= 0 means "contiguous"
                           int dr_dr1_stride1,  // in bytes. <= 0 means "contiguous"

                           // input
                           const double* r_0,   // (3,) array
                           int r_0_stride0      // in bytes. <= 0 means "contiguous"
                           );

// Procrustes fit functions. Align two corresponding sets of normalized
// direction vectors or points. Return true on success
bool mrcal_align_procrustes_vectors_R01(// out
                                        double* R01,
                                        // in
                                        const int N,
                                        // (N,3) arrays
                                        // normalized direction vectors
                                        const double* v0,
                                        const double* v1,

                                        // (N,) array; may be NULL to use an even
                                        // weighting
                                        const double* weights);

bool mrcal_align_procrustes_points_Rt01(// out
                                        double* Rt01,
                                        // in
                                        const int N,
                                        // (N,3) arrays
                                        const double* p0,
                                        const double* p1,

                                        // (N,) array; may be NULL to use an even
                                        // weighting
                                        const double* weights);

// Compute a non-unique rotation to map a given vector to [0,0,1]
void mrcal_R_aligned_to_vector(// out
                               double* R,
                               // in
                               const double* v);
