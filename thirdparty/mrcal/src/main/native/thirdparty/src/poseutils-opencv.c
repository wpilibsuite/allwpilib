// The implementation of mrcal_R_from_r is based on opencv.
// The sources have been heavily modified, but the opencv logic remains.
//
// from opencv-4.1.2+dfsg/modules/calib3d/src/calibration.cpp
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of

// Apparently I need this in MSVC to get constants
#define _USE_MATH_DEFINES

#include <math.h>
#include <float.h>

#include "poseutils.h"
#include "strides.h"

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
                          )
{
    init_stride_2D(R, 3,3);
    init_stride_3D(J, 3,3,3 );
    init_stride_1D(r, 3 );

    double norm2r = 0.0;
    for(int i=0; i<3; i++)
        norm2r += P1(r,i)*P1(r,i);

    if( norm2r < DBL_EPSILON*DBL_EPSILON )
    {
        mrcal_identity_R_full(R, R_stride0, R_stride1);

        if( J )
        {
            for(int i=0; i<3; i++)
                for(int j=0; j<3; j++)
                    for(int k=0; k<3; k++)
                        P3(J,i,j,k) = 0.;

            P3(J,1,2,0) = -1.;
            P3(J,2,0,1) = -1.;
            P3(J,0,1,2) = -1.;

            P3(J,2,1,0) =  1.;
            P3(J,0,2,1) =  1.;
            P3(J,1,0,2) =  1.;
        }
        return;
    }

    double theta = sqrt(norm2r);

    double s = sin(theta);
    double c = cos(theta);
    double c1 = 1. - c;
    double itheta = 1./theta;

    double r_unit[3];
    for(int i=0; i<3; i++)
        r_unit[i] = P1(r,i) * itheta;

    // R = cos(theta)*I + (1 - cos(theta))*r*rT + sin(theta)*[r_x]
    P2(R, 0,0) = c + c1*r_unit[0]*r_unit[0];
    P2(R, 0,1) =     c1*r_unit[0]*r_unit[1] - s*r_unit[2];
    P2(R, 0,2) =     c1*r_unit[0]*r_unit[2] + s*r_unit[1];
    P2(R, 1,0) =     c1*r_unit[0]*r_unit[1] + s*r_unit[2];
    P2(R, 1,1) = c + c1*r_unit[1]*r_unit[1];
    P2(R, 1,2) =     c1*r_unit[1]*r_unit[2] - s*r_unit[0];
    P2(R, 2,0) =     c1*r_unit[0]*r_unit[2] - s*r_unit[1];
    P2(R, 2,1) =     c1*r_unit[1]*r_unit[2] + s*r_unit[0];
    P2(R, 2,2) = c + c1*r_unit[2]*r_unit[2];

    if( J )
    {
        // opencv had some logic with lots of 0s. I unrolled all of the
        // loops, and removed all the resulting 0 terms
        double a0, a1, a3;
        double a2 = itheta * c1;
        double a4 = itheta * s;

        a0 = -s        *r_unit[0];
        a1 = (s - 2*a2)*r_unit[0];
        a3 = (c -   a4)*r_unit[0];
        P3(J,0,0,0) = a0 + a1*r_unit[0]*r_unit[0] + a2*(r_unit[0]+r_unit[0]);
        P3(J,0,1,0) =      a1*r_unit[0]*r_unit[1] + a2*r_unit[1]   - a3*r_unit[2];
        P3(J,0,2,0) =      a1*r_unit[0]*r_unit[2] + a2*r_unit[2]   + a3*r_unit[1];
        P3(J,1,0,0) =      a1*r_unit[0]*r_unit[1] + a2*r_unit[1]   + a3*r_unit[2];
        P3(J,1,1,0) = a0 + a1*r_unit[1]*r_unit[1];
        P3(J,1,2,0) =      a1*r_unit[1]*r_unit[2]                  - a3*r_unit[0] - a4;
        P3(J,2,0,0) =      a1*r_unit[0]*r_unit[2] + a2*r_unit[2]   - a3*r_unit[1];
        P3(J,2,1,0) =      a1*r_unit[1]*r_unit[2]                  + a3*r_unit[0] + a4;
        P3(J,2,2,0) = a0 + a1*r_unit[2]*r_unit[2];

        a0 = -s        *r_unit[1];
        a1 = (s - 2*a2)*r_unit[1];
        a3 = (c -   a4)*r_unit[1];
        P3(J,0,0,1) = a0 + a1*r_unit[0]*r_unit[0];
        P3(J,0,1,1) =      a1*r_unit[0]*r_unit[1] + a2*r_unit[0]   - a3*r_unit[2];
        P3(J,0,2,1) =      a1*r_unit[0]*r_unit[2]                  + a3*r_unit[1] + a4;
        P3(J,1,0,1) =      a1*r_unit[0]*r_unit[1] + a2*r_unit[0]   + a3*r_unit[2];
        P3(J,1,1,1) = a0 + a1*r_unit[1]*r_unit[1] + a2*(r_unit[1]+r_unit[1]);
        P3(J,1,2,1) =      a1*r_unit[1]*r_unit[2] + a2*r_unit[2]   - a3*r_unit[0];
        P3(J,2,0,1) =      a1*r_unit[0]*r_unit[2]                  - a3*r_unit[1] - a4;
        P3(J,2,1,1) =      a1*r_unit[1]*r_unit[2] + a2*r_unit[2]   + a3*r_unit[0];
        P3(J,2,2,1) = a0 + a1*r_unit[2]*r_unit[2];

        a0 = -s        *r_unit[2];
        a1 = (s - 2*a2)*r_unit[2];
        a3 = (c -   a4)*r_unit[2];
        P3(J,0,0,2) = a0 + a1*r_unit[0]*r_unit[0];
        P3(J,0,1,2) =      a1*r_unit[0]*r_unit[1]                  - a3*r_unit[2] - a4;
        P3(J,0,2,2) =      a1*r_unit[0]*r_unit[2] + a2*r_unit[0]   + a3*r_unit[1];
        P3(J,1,0,2) =      a1*r_unit[0]*r_unit[1]                  + a3*r_unit[2] + a4;
        P3(J,1,1,2) = a0 + a1*r_unit[1]*r_unit[1];
        P3(J,1,2,2) =      a1*r_unit[1]*r_unit[2] + a2*r_unit[1]   - a3*r_unit[0];
        P3(J,2,0,2) =      a1*r_unit[0]*r_unit[2] + a2*r_unit[0]   - a3*r_unit[1];
        P3(J,2,1,2) =      a1*r_unit[1]*r_unit[2] + a2*r_unit[1]   + a3*r_unit[0];
        P3(J,2,2,2) = a0 + a1*r_unit[2]*r_unit[2] + a2*(r_unit[2]+r_unit[2]);
    }
}
