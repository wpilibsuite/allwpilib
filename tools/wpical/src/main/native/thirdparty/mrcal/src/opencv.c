// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "mrcal.h"


// The implementation of _mrcal_project_internal_opencv is based on opencv. The
// sources have been heavily modified, but the opencv logic remains. This
// function is a cut-down cvProjectPoints2Internal() to keep only the
// functionality I want and to use my interfaces. Putting this here allows me to
// drop the C dependency on opencv. Which is a good thing, since opencv dropped
// their C API
//
// from opencv-4.2.0+dfsg/modules/calib3d/src/calibration.cpp
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

// NOT A PART OF THE EXTERNAL API. This is exported for the mrcal python wrapper
// only
void _mrcal_project_internal_opencv( // outputs
                                    mrcal_point2_t* q,
                                    mrcal_point3_t* dq_dp,         // may be NULL
                                    double* dq_dintrinsics_nocore, // may be NULL

                                    // inputs
                                    const mrcal_point3_t* p,
                                    int N,
                                    const double* intrinsics,
                                    int Nintrinsics)
{
    const double fx = intrinsics[0];
    const double fy = intrinsics[1];
    const double cx = intrinsics[2];
    const double cy = intrinsics[3];

    double k[12] = {};
    for(int i=0; i<Nintrinsics-4; i++)
        k[i] = intrinsics[i+4];

    for( int i = 0; i < N; i++ )
    {
        double z_recip = 1./p[i].z;
        double x = p[i].x * z_recip;
        double y = p[i].y * z_recip;

        double r2      = x*x + y*y;
        double r4      = r2*r2;
        double r6      = r4*r2;
        double a1      = 2*x*y;
        double a2      = r2 + 2*x*x;
        double a3      = r2 + 2*y*y;
        double cdist   = 1 + k[0]*r2 + k[1]*r4 + k[4]*r6;
        double icdist2 = 1./(1 + k[5]*r2 + k[6]*r4 + k[7]*r6);
        double xd      = x*cdist*icdist2 + k[2]*a1 + k[3]*a2 + k[8]*r2+k[9]*r4;
        double yd      = y*cdist*icdist2 + k[2]*a3 + k[3]*a1 + k[10]*r2+k[11]*r4;

        q[i].x = xd*fx + cx;
        q[i].y = yd*fy + cy;


        if( dq_dp )
        {
            double dx_dp[] = { z_recip, 0,       -x*z_recip };
            double dy_dp[] = { 0,       z_recip, -y*z_recip };
            for( int j = 0; j < 3; j++ )
            {
                double dr2_dp = 2*x*dx_dp[j] + 2*y*dy_dp[j];
                double dcdist_dp = k[0]*dr2_dp + 2*k[1]*r2*dr2_dp + 3*k[4]*r4*dr2_dp;
                double dicdist2_dp = -icdist2*icdist2*(k[5]*dr2_dp + 2*k[6]*r2*dr2_dp + 3*k[7]*r4*dr2_dp);
                double da1_dp = 2*(x*dy_dp[j] + y*dx_dp[j]);
                double dmx_dp = (dx_dp[j]*cdist*icdist2 + x*dcdist_dp*icdist2 + x*cdist*dicdist2_dp +
                                k[2]*da1_dp + k[3]*(dr2_dp + 4*x*dx_dp[j]) + k[8]*dr2_dp + 2*r2*k[9]*dr2_dp);
                double dmy_dp = (dy_dp[j]*cdist*icdist2 + y*dcdist_dp*icdist2 + y*cdist*dicdist2_dp +
                                k[2]*(dr2_dp + 4*y*dy_dp[j]) + k[3]*da1_dp + k[10]*dr2_dp + 2*r2*k[11]*dr2_dp);
                dq_dp[i*2 + 0].xyz[j] = fx*dmx_dp;
                dq_dp[i*2 + 1].xyz[j] = fy*dmy_dp;
            }
        }
        if( dq_dintrinsics_nocore )
        {
            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 0] = fx*x*icdist2*r2;
            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 0] = fy*(y*icdist2*r2);

            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 1] = fx*x*icdist2*r4;
            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 1] = fy*y*icdist2*r4;

            if( Nintrinsics-4 > 2 )
            {
                dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 2] = fx*a1;
                dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 2] = fy*a3;
                dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 3] = fx*a2;
                dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 3] = fy*a1;
                if( Nintrinsics-4 > 4 )
                {
                    dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 4] = fx*x*icdist2*r6;
                    dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 4] = fy*y*icdist2*r6;

                    if( Nintrinsics-4 > 5 )
                    {
                        dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 5] = fx*x*cdist*(-icdist2)*icdist2*r2;
                        dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 5] = fy*y*cdist*(-icdist2)*icdist2*r2;
                        dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 6] = fx*x*cdist*(-icdist2)*icdist2*r4;
                        dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 6] = fy*y*cdist*(-icdist2)*icdist2*r4;
                        dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 7] = fx*x*cdist*(-icdist2)*icdist2*r6;
                        dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 7] = fy*y*cdist*(-icdist2)*icdist2*r6;
                        if( Nintrinsics-4 > 8 )
                        {
                            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 8]  = fx*r2; //s1
                            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 8]  = fy*0;  //s1
                            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 9]  = fx*r4; //s2
                            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 9]  = fy*0;  //s2
                            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 10] = fx*0;  //s3
                            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 10] = fy*r2; //s3
                            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 0) + 11] = fx*0;  //s4
                            dq_dintrinsics_nocore[(Nintrinsics-4)*(2*i + 1) + 11] = fy*r4; //s4
                        }
                    }
                }
            }
        }
    }
}
