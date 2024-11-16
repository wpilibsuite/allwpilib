// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include <stdio.h>
#include <assert.h>

#include "autodiff.hh"

extern "C" {
#include "cahvore.h"
}

template <int N>
static
bool _project_cahvore_internals( // outputs
                                 vec_withgrad_t<N,3>* pdistorted,

                                 // inputs
                                 const vec_withgrad_t<N,3>& p,
                                 const val_withgrad_t<N>&   alpha,
                                 const val_withgrad_t<N>&   beta,
                                 const val_withgrad_t<N>&   r0,
                                 const val_withgrad_t<N>&   r1,
                                 const val_withgrad_t<N>&   r2,
                                 const val_withgrad_t<N>&   e0,
                                 const val_withgrad_t<N>&   e1,
                                 const val_withgrad_t<N>&   e2,
                                 const double cahvore_linearity)
{
    // Apply a CAHVORE warp to an un-distorted point

    //  Given intrinsic parameters of a CAHVORE model and a set of
    //  camera-coordinate points, return the projected point(s)

    // This comes from cmod_cahvore_3d_to_2d_general() in
    // m-jplv/libcmod/cmod_cahvore.c
    //
    // The lack of documentation here comes directly from the lack of
    // documentation in that function.

    // I parametrize the optical axis such that
    // - o(alpha=0, beta=0) = (0,0,1) i.e. the optical axis is at the center
    //   if both parameters are 0
    // - The gradients are cartesian. I.e. do/dalpha and do/dbeta are both
    //   NOT 0 at (alpha=0,beta=0). This would happen at the poles (gimbal
    //   lock), and that would make my solver unhappy
    // So o = { s_al*c_be, s_be,  c_al*c_be }
    vec_withgrad_t<N,2> sca = alpha.sincos();
    vec_withgrad_t<N,2> scb = beta .sincos();

    vec_withgrad_t<N,3> o;
    o[0] = scb[1]*sca[0];
    o[1] = scb[0];
    o[2] = scb[1]*sca[1];

    // Note: CAHVORE is noncentral: project(p) and project(k*p) do NOT
    // project to the same point

    // What is called "omega" in the canonical CAHVOR implementation is
    // called "zeta" in the canonical CAHVORE implementation. They're the
    // same thing

    // cos( angle between p and o ) = inner(p,o) / (norm(o) * norm(p)) =
    // zeta/norm(p)
    val_withgrad_t<N> zeta = p.dot(o);

    // Basic Computations
    // Calculate initial terms
    vec_withgrad_t<N,3> u  = o*zeta;
    vec_withgrad_t<N,3> ll = p - u;
    val_withgrad_t<N>   l  = ll.mag();

    // Calculate theta using Newton's Method
    val_withgrad_t<N> theta = l.atan2(zeta);

    int inewton;
    for( inewton = 100; inewton; inewton--)
    {
        // Compute terms from the current value of theta
        vec_withgrad_t<N,2> scth = theta.sincos();

        val_withgrad_t<N> theta2  = theta * theta;
        val_withgrad_t<N> theta3  = theta * theta2;
        val_withgrad_t<N> theta4  = theta * theta3;
        val_withgrad_t<N> upsilon =
            zeta*scth[1] + l*scth[0]
            + (scth[1] -  1.0 ) * (e0 +      e1*theta2 +     e2*theta4)
            - (theta - scth[0]) * (      e1*theta*2.0  + e2*theta3*4.0);

        // Update theta
        val_withgrad_t<N> dtheta =
            (zeta*scth[0] - l*scth[1]
             - (theta - scth[0]) * (e0 + e1*theta2 + e2*theta4)) / upsilon;

        theta -= dtheta;

        // Check exit criterion from last update
        if(fabs(dtheta.x) < 1e-8)
            break;
    }
    if(inewton == 0)
    {
        fprintf(stderr, "%s(): too many iterations\n", __func__);
        return false;
    }

    // got a theta

    // Check the value of theta
    if(theta.x * fabs(cahvore_linearity) > M_PI/2.)
    {
        fprintf(stderr, "%s(): theta out of bounds\n", __func__);
        return false;
    }

    // If we aren't close enough to use the small-angle approximation ...
    if (theta.x > 1e-8)
    {
        // ... do more math!
        val_withgrad_t<N> linth = theta * cahvore_linearity;
        val_withgrad_t<N> chi;
        if (cahvore_linearity < -1e-15)
            chi = linth.sin() / cahvore_linearity;
        else if (cahvore_linearity > 1e-15)
            chi = linth.tan() / cahvore_linearity;
        else
            chi = theta;
        val_withgrad_t<N> chi2 = chi * chi;
        val_withgrad_t<N> chi3 = chi * chi2;
        val_withgrad_t<N> chi4 = chi * chi3;

        val_withgrad_t<N> zetap = l / chi;

        val_withgrad_t<N> mu = r0 + r1*chi2 + r2*chi4;

        vec_withgrad_t<N,3> uu = o*zetap;
        vec_withgrad_t<N,3> vv = ll * (mu + 1.);
        *pdistorted = uu + vv;
    }
    else
        *pdistorted = p;
    return true;
}


// Not meant to be touched by the end user. Implemented separate from mrcal.c so
// that I can get automated gradient propagation with c++
extern "C"

bool project_cahvore_internals( // outputs
                                mrcal_point3_t* __restrict pdistorted,
                                double*         __restrict dpdistorted_dintrinsics_nocore,
                                double*         __restrict dpdistorted_dp,

                                // inputs
                                const mrcal_point3_t* __restrict p,
                                const double* __restrict intrinsics_nocore,
                                const double cahvore_linearity)
{
    if( dpdistorted_dintrinsics_nocore == NULL &&
        dpdistorted_dp                 == NULL )
    {
        // No gradients. NGRAD in all the templates is 0

        vec_withgrad_t<0,3> pdistortedg;

        vec_withgrad_t<0,8> ig;
        ig.init_vars(intrinsics_nocore,
                     0,8, // ivar0,Nvars
                     -1   // i_gradvec0
                     );

        vec_withgrad_t<0,3> pg;
        pg.init_vars(p->xyz,
                     0,3, // ivar0,Nvars
                     -1   // i_gradvec0
                     );

        if(!_project_cahvore_internals(&pdistortedg,
                                       pg,
                                       ig[0],
                                       ig[1],
                                       ig[2],
                                       ig[3],
                                       ig[4],
                                       ig[5],
                                       ig[6],
                                       ig[7],
                                       cahvore_linearity))
            return false;

        pdistortedg.extract_value(pdistorted->xyz);
        return true;
    }

    if( dpdistorted_dintrinsics_nocore == NULL &&
        dpdistorted_dp                 != NULL )
    {
        // 3 variables:  p (3 vars)
        vec_withgrad_t<3,3> pdistortedg;

        vec_withgrad_t<3,8> ig;
        ig.init_vars(intrinsics_nocore,
                     0,8, // ivar0,Nvars
                     -1   // i_gradvec0
                     );

        vec_withgrad_t<3,3> pg;
        pg.init_vars(p->xyz,
                     0,3, // ivar0,Nvars
                     0    // i_gradvec0
                     );

        if(!_project_cahvore_internals(&pdistortedg,
                                       pg,
                                       ig[0],
                                       ig[1],
                                       ig[2],
                                       ig[3],
                                       ig[4],
                                       ig[5],
                                       ig[6],
                                       ig[7],
                                       cahvore_linearity))
            return false;

        pdistortedg.extract_value(pdistorted->xyz);
        pdistortedg.extract_grad (dpdistorted_dp,
                                  0,3, // ivar0,Nvars
                                  0,   // i_gradvec0
                                  sizeof(double)*3, sizeof(double),
                                  3    // Nvars
                                  );
        return true;
    }

    if( dpdistorted_dintrinsics_nocore != NULL &&
        dpdistorted_dp                 == NULL )
    {
        // 8 variables:  alpha,beta,R,E (8 vars)
        vec_withgrad_t<8,3> pdistortedg;

        vec_withgrad_t<8,8> ig;
        ig.init_vars(intrinsics_nocore,
                     0,8, // ivar0,Nvars
                     0    // i_gradvec0
                     );

        vec_withgrad_t<8,3> pg;
        pg.init_vars(p->xyz,
                     0,3, // ivar0,Nvars
                     -1   // i_gradvec0
                     );

        if(!_project_cahvore_internals(&pdistortedg,
                                       pg,
                                       ig[0],
                                       ig[1],
                                       ig[2],
                                       ig[3],
                                       ig[4],
                                       ig[5],
                                       ig[6],
                                       ig[7],
                                       cahvore_linearity))
            return false;

        pdistortedg.extract_value(pdistorted->xyz);
        pdistortedg.extract_grad (dpdistorted_dintrinsics_nocore,
                                  0,8, // i_gradvec0,N_gradout
                                  0,   // ivar0
                                  sizeof(double)*8, sizeof(double),
                                  3    // Nvars
                                  );
        return true;
    }

    if( dpdistorted_dintrinsics_nocore != NULL &&
        dpdistorted_dp                 != NULL )
    {
        // 11 variables:  alpha,beta,R,E (8 vars)  +  p (3 vars)
        vec_withgrad_t<11,3> pdistortedg;

        vec_withgrad_t<11,8> ig;
        ig.init_vars(intrinsics_nocore,
                     0,8, // ivar0,Nvars
                     0    // i_gradvec0
                     );

        vec_withgrad_t<11,3> pg;
        pg.init_vars(p->xyz,
                     0,3, // ivar0,Nvars
                     8    // i_gradvec0
                     );

        if(!_project_cahvore_internals(&pdistortedg,
                                       pg,
                                       ig[0],
                                       ig[1],
                                       ig[2],
                                       ig[3],
                                       ig[4],
                                       ig[5],
                                       ig[6],
                                       ig[7],
                                       cahvore_linearity))
            return false;

        pdistortedg.extract_value(pdistorted->xyz);
        pdistortedg.extract_grad (dpdistorted_dintrinsics_nocore,
                                  0,8, // i_gradvec0,N_gradout
                                  0,   // ivar0
                                  sizeof(double)*8, sizeof(double),
                                  3    // Nvars
                                  );
        pdistortedg.extract_grad (dpdistorted_dp,
                                  8,3, // ivar0,Nvars
                                  0,   // i_gradvec0
                                  sizeof(double)*3, sizeof(double),
                                  3    // Nvars
                                  );
        return true;
    }

    fprintf(stderr, "Getting here is a bug. Please report\n");
    assert(0);
}
