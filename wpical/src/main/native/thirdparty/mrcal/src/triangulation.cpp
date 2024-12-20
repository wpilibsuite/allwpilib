// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "autodiff.hh"

extern "C" {
#include "triangulation.h"
}

template <int NGRAD>
static
bool
triangulate_assume_intersect( // output
                             vec_withgrad_t<NGRAD,3>& m,

                             // inputs. camera-0 coordinates
                             const vec_withgrad_t<NGRAD,3>& v0,
                             const vec_withgrad_t<NGRAD,3>& v1,
                             const vec_withgrad_t<NGRAD,3>& t01)
{
    // I take two 3D rays that are assumed to intersect, and return the
    // intersection point. Results are undefined if these rays actually
    // don't intersect

    // Each pixel observation represents a ray in 3D:
    //
    //   k0 v0 = t01 + k1 v1
    //
    //   t01 = [v0 -v1] k
    //
    // This is over-determined: k has 2DOF, but I have 3 equations. I know that
    // the vectors intersect, so I can use 2 axes only, which makes the problem
    // uniquely determined. Let's pick the 2 axes to use. The "forward"
    // direction (z) is dominant, so let's use that. For the second axis, let's
    // use whichever is best numerically: biggest abs(det), so that I divide by
    // something as far away from 0 as possible. I have
    //
    double fabs_det_xz = fabs(-v0.v[0].x*v1.v[2].x + v0.v[2].x*v1.v[0].x);
    double fabs_det_yz = fabs(-v0.v[1].x*v1.v[2].x + v0.v[2].x*v1.v[1].x);

    // If using xz, I have:
    //
    //   k = 1/(-v0[0]*v1[2] + v0[2]*v1[0]) * [-v1[2]     v1[0] ] t01
    //                                        [-v0[2]     v0[0] ]
    // [0] -> [1] if using yz
    val_withgrad_t<NGRAD> k0;
    if(fabs_det_xz > fabs_det_yz)
    {
        // xz
        if(fabs_det_xz <= 1e-10)
            return false;

        val_withgrad_t<NGRAD> det = v1.v[0]*v0.v[2] - v0.v[0]*v1.v[2];
        k0 = (t01.v[2]*v1.v[0] - t01.v[0]*v1.v[2]) / det;
        if(k0.x <= 0.0)
            return false;
        bool k1_negative = (t01.v[2].x*v0.v[0].x > t01.v[0].x*v0.v[2].x) ^ (det.x > 0);
        if(k1_negative)
            return false;

#if 0
        val_withgrad_t<NGRAD> k1 = (t01.v[2]*v0.v[0] - t01.v[0]*v0.v[2]) / det;
        vec_withgrad_t<NGRAD,3> m2 = v1*k1 + t01;
        m2 -= m;
        double d2 = m2.v[0].x*m2.v[0].x + m2.v[1].x*m2.v[1].x + m2.v[2].x*m2.v[2].x;
        fprintf(stderr, "diff: %f\n", d2);
#endif
    }
    else
    {
        // yz
        if(fabs_det_yz <= 1e-10)
            return false;

        val_withgrad_t<NGRAD> det = v1.v[1]*v0.v[2] - v0.v[1]*v1.v[2];
        k0 = (t01.v[2]*v1.v[1] - t01.v[1]*v1.v[2]) / det;
        if(k0.x <= 0.0)
            return false;
        bool k1_negative = (t01.v[2].x*v0.v[1].x > t01.v[1].x*v0.v[2].x) ^ (det.x > 0);
        if(k1_negative)
            return false;

#if 0
    val_withgrad_t<NGRAD> k1 = (t01.v[2]*v0.v[1] - t01.v[1]*v0.v[2]) / det;
    vec_withgrad_t<NGRAD,3> m2 = v1*k1 + t01;
    m2 -= m;
    double d2 = m2.v[1].x*m2.v[1].x + m2.v[1].x*m2.v[1].x + m2.v[2].x*m2.v[2].x;
    fprintf(stderr, "diff: %f\n", d2);
#endif
    }

    m = v0 * k0;

    return true;
}


// #warning "These all have NGRAD=9, which is inefficient: some/all of the requested gradients could be NULL"

// Basic closest-approach-in-3D routine
extern "C"
mrcal_point3_t
mrcal_triangulate_geometric(// outputs
                            // These all may be NULL
                            mrcal_point3_t* _dm_dv0,
                            mrcal_point3_t* _dm_dv1,
                            mrcal_point3_t* _dm_dt01,

                            // inputs

                            // not-necessarily normalized vectors in the camera-0
                            // coord system
                            const mrcal_point3_t* _v0,
                            const mrcal_point3_t* _v1,
                            const mrcal_point3_t* _t01)
{
    // This is the basic 3D-geometry routine. I find the point in 3D that
    // minimizes the distance to each of the observation rays. This is simple,
    // but not as accurate as we'd like. All the other methods have lower
    // biases. See the Lee-Civera papers for details:
    //
    //   Paper that compares all methods implemented here:
    //   "Triangulation: Why Optimize?", Seong Hun Lee and Javier Civera.
    //   https://arxiv.org/abs/1907.11917
    //
    //   Earlier paper that doesn't have mid2 or wmid2:
    //   "Closed-Form Optimal Two-View Triangulation Based on Angular Errors",
    //   Seong Hun Lee and Javier Civera. ICCV 2019.
    //
    // Each pixel observation represents a ray in 3D. The best
    // estimate of the 3d position of the point being observed
    // is the point nearest to both these rays
    //
    // Let's say I have a ray from the origin to v0, and another ray from t01
    // to v1 (v0 and v1 aren't necessarily normal). Let the nearest points on
    // each ray be k0 and k1 along each ray respectively: E = norm2(t01 + k1*v1
    // - k0*v0):
    //
    //   dE/dk0 = 0 = inner(t01 + k1*v1 - k0*v0, -v0)
    //   dE/dk1 = 0 = inner(t01 + k1*v1 - k0*v0,  v1)
    //
    // ->    t01.v0 + k1 v0.v1 = k0 v0.v0
    //      -t01.v1 + k0 v0.v1 = k1 v1.v1
    //
    // -> [  v0.v0   -v0.v1] [k0] = [ t01.v0]
    //    [ -v0.v1    v1.v1] [k1] = [-t01.v1]
    //
    // -> [k0] = 1/(v0.v0 v1.v1 -(v0.v1)**2) [ v1.v1   v0.v1][ t01.v0]
    //    [k1]                               [ v0.v1   v0.v0][-t01.v1]
    //
    // I return the midpoint:
    //
    //   x = (k0 v0 + t01 + k1 v1)/2
    vec_withgrad_t<9,3> v0 (_v0 ->xyz, 0);
    vec_withgrad_t<9,3> v1 (_v1 ->xyz, 3);
    vec_withgrad_t<9,3> t01(_t01->xyz, 6);

    val_withgrad_t<9> dot_v0v0 = v0.norm2();
    val_withgrad_t<9> dot_v1v1 = v1.norm2();
    val_withgrad_t<9> dot_v0v1 = v0.dot(v1);
    val_withgrad_t<9> dot_v0t  = v0.dot(t01);
    val_withgrad_t<9> dot_v1t  = v1.dot(t01);

    val_withgrad_t<9> denom = dot_v0v0*dot_v1v1 - dot_v0v1*dot_v0v1;
    if(-1e-10 <= denom.x && denom.x <= 1e-10)
        return (mrcal_point3_t){0};

    val_withgrad_t<9> denom_recip = val_withgrad_t<9>(1.)/denom;
    val_withgrad_t<9> k0 = denom_recip * (dot_v1v1*dot_v0t - dot_v0v1*dot_v1t);
    if(k0.x <= 0.0) return (mrcal_point3_t){0};
    val_withgrad_t<9> k1 = denom_recip * (dot_v0v1*dot_v0t - dot_v0v0*dot_v1t);
    if(k1.x <= 0.0) return (mrcal_point3_t){0};

    vec_withgrad_t<9,3> m = (v0*k0 + v1*k1 + t01) * 0.5;

    mrcal_point3_t _m;
    m.extract_value(_m.xyz);

    if(_dm_dv0 != NULL)
        m.extract_grad (_dm_dv0->xyz,  0,3, 0,
                         3*sizeof(double), sizeof(double),
                         3);
    if(_dm_dv1 != NULL)
        m.extract_grad (_dm_dv1->xyz,  3,3, 0,
                         3*sizeof(double), sizeof(double),
                         3);
    if(_dm_dt01 != NULL)
        m.extract_grad (_dm_dt01->xyz, 6,3, 0,
                         3*sizeof(double), sizeof(double),
                         3);

#if 0
    MSG("intersecting...");
    MSG("v0 = (%.20f,%.20f,%.20f)", v0[0].x,v0[1].x,v0[2].x);
    MSG("t01 = (%.20f,%.20f,%.20f)", t01[0].x,t01[1].x,t01[2].x);
    MSG("v1 = (%.20f,%.20f,%.20f)", v1[0].x,v1[1].x,v1[2].x);
    MSG("intersection = (%.20f,%.20f,%.20f) dist %f",
        m.v[0].x,m.v[1].x,m.v[2].x,
        sqrt( m.dot(m).x));
#endif

    return _m;
}

// Minimize L2 pinhole reprojection error. Described in "Triangulation Made
// Easy", Peter Lindstrom, IEEE Conference on Computer Vision and Pattern
// Recognition, 2010.
extern "C"
mrcal_point3_t
mrcal_triangulate_lindstrom(// outputs
                      // These all may be NULL
                      mrcal_point3_t* _dm_dv0,
                      mrcal_point3_t* _dm_dv1,
                      mrcal_point3_t* _dm_dRt01,

                      // inputs

                      // not-necessarily normalized vectors in the LOCAL
                      // coordinate system. This is different from the other
                      // triangulation routines
                      const mrcal_point3_t* _v0_local,
                      const mrcal_point3_t* _v1_local,
                      const mrcal_point3_t* _Rt01)
{
    // This is an implementation of the algorithm described in "Triangulation
    // Made Easy", Peter Lindstrom, IEEE Conference on Computer Vision and
    // Pattern Recognition, 2010. A copy of this paper is available in this repo
    // in docs/TriangulationLindstrom.pdf. The implementation here is the niter2
    // routine in Listing 3. There's a higher-level implemented-in-python test
    // in analyses/triangulation.py
    //
    // A simpler, but less-accurate way of doing is lives in
    // triangulate_direct()

    // I'm looking at wikipedia for the Essential matrix definition:
    //
    //   https://en.wikipedia.org/wiki/Essential_matrix
    //
    // and at Lindstrom's paper. Note that THEY HAVE DIFFERENT DEFINITIONS OF E
    //
    // I stick to Lindstrom's convention here. He has (section 2, equation 3)
    //
    //   E = cross(t) R
    //   transpose(x0) E x1 = 0
    //
    // What are R and t?
    //
    //   x0' cross(t) R x1 = 0
    //   x0' cross(t) R (R10 x0 + t10) = 0
    //
    // So Lindstrom has R = R01 ->
    //
    //   x0' cross(t) R01 (R10 x0 + t10) = 0
    //   x0' cross(t) (x0 + R01 t10)     = 0
    //   x0' cross(t) R01 t10            = 0
    //
    // This holds if Lindstrom has R01 t10 = +- t
    //
    // Note that if   x1 = R10 x0 + t10   then   x0 = R01 x1 - R01 t10
    //
    // So I let t = t01
    //
    // Thus he's multiplying cross(t01) and R01:
    //
    //   E = cross(t01) R01
    //     = cross(t01) R10'

    // cross(t01) = np.array(((0,       -t01[2],  t01[1]),
    //                        ( t01[2],  0,      -t01[0]),
    //                        (-t01[1],  t01[0],  0)));

    vec_withgrad_t<18,3> v0 (_v0_local->xyz, 0);
    vec_withgrad_t<18,3> v1 (_v1_local->xyz, 3);
    vec_withgrad_t<18,9> R01(_Rt01    ->xyz, 6);
    vec_withgrad_t<18,3> t01(_Rt01[3]  .xyz, 15);

    val_withgrad_t<18> E[9] = { R01[6]*t01[1] - R01[3]*t01[2],
                                R01[7]*t01[1] - R01[4]*t01[2],
                                R01[8]*t01[1] - R01[5]*t01[2],

                                R01[0]*t01[2] - R01[6]*t01[0],
                                R01[1]*t01[2] - R01[7]*t01[0],
                                R01[2]*t01[2] - R01[8]*t01[0],

                                R01[3]*t01[0] - R01[0]*t01[1],
                                R01[4]*t01[0] - R01[1]*t01[1],
                                R01[5]*t01[0] - R01[2]*t01[1] };

    // Paper says to rescale x0,x1 such that their last element is 1.0.
    // I don't even store it
    val_withgrad_t<18> x0[2] = { v0[0]/v0[2], v0[1]/v0[2] };
    val_withgrad_t<18> x1[2] = { v1[0]/v1[2], v1[1]/v1[2] };

    // for debugging
#if 0
    {
        fprintf(stderr, "E:\n");
        for(int i=0; i<3; i++)
            fprintf(stderr, "%f %f %f\n", E[0 + 3*i].x, E[1 + 3*i].x, E[2 + 3*i].x);
        double Ex1[3] = { E[0].x*x1[0].x + E[1].x*x1[1].x + E[2].x,
                          E[3].x*x1[0].x + E[4].x*x1[1].x + E[5].x,
                          E[6].x*x1[0].x + E[7].x*x1[1].x + E[8].x };
        double x0Ex1 = Ex1[0]*x0[0].x + Ex1[1]*x0[1].x + Ex1[2];
        fprintf(stderr, "conj before: %f\n", x0Ex1);
    }
#endif

    // Now I implement the algorithm. x0 here is x in the paper; x1 here
    // is x' in the paper

    // Step 1. n = nps.matmult(x1, nps.transpose(E))[:2]
    val_withgrad_t<18> n[2];
    n[0] = E[0]*x1[0] + E[1]*x1[1] + E[2];
    n[1] = E[3]*x1[0] + E[4]*x1[1] + E[5];


    // Step 2. nn = nps.matmult(x0, E)[:2]
    val_withgrad_t<18> nn[2];
    nn[0] = E[0]*x0[0] + E[3]*x0[1] + E[6];
    nn[1] = E[1]*x0[0] + E[4]*x0[1] + E[7];

    // Step 3. a = nps.matmult( n, E[:2,:2], nps.transpose(nn) ).ravel()
    val_withgrad_t<18> a =
        n[0]*E[0]*nn[0] +
        n[0]*E[1]*nn[1] +
        n[1]*E[3]*nn[0] +
        n[1]*E[4]*nn[1];

    // Step 4. b = 0.5*( nps.inner(n,n) + nps.inner(nn,nn) )
    val_withgrad_t<18> b = (n [0]*n [0] + n [1]*n [1] +
                            nn[0]*nn[0] + nn[1]*nn[1]) * 0.5;

    // Step 5. c  = nps.matmult(x0, E, nps.transpose(x1)).ravel()
    val_withgrad_t<18> n_2 =
        E[6]*x1[0] +
        E[7]*x1[1] +
        E[8];
    val_withgrad_t<18> c =
        n[0]*x0[0] +
        n[1]*x0[1] +
        n_2;

    // Step 6. d  = np.sqrt( b*b - a*c )
    val_withgrad_t<18> d = (b*b - a*c).sqrt();

    // Step 7. l = c / (b+d)
    val_withgrad_t<18> l = c / (b + d);

    // Step 8. dx  = l*n
    val_withgrad_t<18> dx[2] = { l * n[0], l * n[1] };

    // Step 9. dxx = l*nn
    val_withgrad_t<18> dxx[2] = { l * nn[0], l * nn[1] };

    // Step 10. n -= nps.matmult(dxx, nps.transpose(E[:2,:2]))
    n[0] = n[0] - E[0]*dxx[0] - E[1]*dxx[1] ;
    n[1] = n[1] - E[3]*dxx[0] - E[4]*dxx[1] ;

    // Step 11. nn -= nps.matmult(dx,  E[:2,:2])
    nn[0] = nn[0] - E[0]*dx[0] - E[3]*dx[1] ;
    nn[1] = nn[1] - E[1]*dx[0] - E[4]*dx[1] ;

    // Step 12. l *= 2*d/( nps.inner(n,n) + nps.inner(nn,nn) )
    val_withgrad_t<18> bb = (n [0]*n [0] + n [1]*n [1] +
                             nn[0]*nn[0] + nn[1]*nn[1]) * 0.5;
    l = l/d * bb;

    // Step 13. dx  = l*n
    dx[0] = l * n[0];
    dx[1] = l * n[1];

    // Step 14. dxx = l*nn
    dxx[0] = l * nn[0];
    dxx[1] = l * nn[1];

    // Step 15
    v0.v[0] = x0[0] - dx[0];
    v0.v[1] = x0[1] - dx[1];
    v0.v[2] = val_withgrad_t<18>(1.0);

    // Step 16
    v1.v[0] = x1[0] - dxx[0];
    v1.v[1] = x1[1] - dxx[1];
    v1.v[2] = val_withgrad_t<18>(1.0);

    // for debugging
#if 0
    {
        double Ex1[3] = { E[0].x*v1[0].x + E[1].x*v1[1].x + E[2].x,
                          E[3].x*v1[0].x + E[4].x*v1[1].x + E[5].x,
                          E[6].x*v1[0].x + E[7].x*v1[1].x + E[8].x };
        double x0Ex1 = Ex1[0]*v0[0].x + Ex1[1]*v0[1].x + Ex1[2];
        fprintf(stderr, "conj after: %f\n", x0Ex1);
    }
#endif

    // Construct v0, v1 in a common coord system
    vec_withgrad_t<18,3> Rv1;
    Rv1.v[0] = R01.v[0]*v1.v[0] + R01.v[1]*v1.v[1] + R01.v[2]*v1.v[2];
    Rv1.v[1] = R01.v[3]*v1.v[0] + R01.v[4]*v1.v[1] + R01.v[5]*v1.v[2];
    Rv1.v[2] = R01.v[6]*v1.v[0] + R01.v[7]*v1.v[1] + R01.v[8]*v1.v[2];

    // My two 3D rays now intersect exactly, and I use compute the intersection
    // with that assumption
    vec_withgrad_t<18,3> m;
    if(!triangulate_assume_intersect(m, v0, Rv1, t01))
        return (mrcal_point3_t){0};

    mrcal_point3_t _m;
    m.extract_value(_m.xyz);

    if(_dm_dv0 != NULL)
        m.extract_grad (_dm_dv0->xyz,  0,3, 0,
                        3*sizeof(double), sizeof(double),
                        3);
    if(_dm_dv1 != NULL)
        m.extract_grad (_dm_dv1->xyz,  3,3, 0,
                        3*sizeof(double), sizeof(double),
                        3);
    if(_dm_dRt01 != NULL)
        m.extract_grad (_dm_dRt01->xyz, 6,12,0,
                        12*sizeof(double), sizeof(double),
                        3);
    return _m;
}

// Minimize L1 angle error. Described in "Closed-Form Optimal Two-View
// Triangulation Based on Angular Errors", Seong Hun Lee and Javier Civera. ICCV
// 2019.
extern "C"
mrcal_point3_t
mrcal_triangulate_leecivera_l1(// outputs
                               // These all may be NULL
                               mrcal_point3_t* _dm_dv0,
                               mrcal_point3_t* _dm_dv1,
                               mrcal_point3_t* _dm_dt01,

                               // inputs

                               // not-necessarily normalized vectors in the camera-0
                               // coord system
                               const mrcal_point3_t* _v0,
                               const mrcal_point3_t* _v1,
                               const mrcal_point3_t* _t01)
{
    // The paper has m0, m1 as the cam1-frame observation vectors. I do
    // everything in cam0-frame
    vec_withgrad_t<9,3> v0 (_v0 ->xyz, 0);
    vec_withgrad_t<9,3> v1 (_v1 ->xyz, 3);
    vec_withgrad_t<9,3> t01(_t01->xyz, 6);

    val_withgrad_t<9> dot_v0v0 = v0.norm2();
    val_withgrad_t<9> dot_v1v1 = v1.norm2();
    val_withgrad_t<9> dot_v0t  = v0.dot(t01);
    val_withgrad_t<9> dot_v1t  = v1.dot(t01);

    // I pick a bath based on which len(cross(normalize(m),t)) is larger: which
    // of m0 and m1 is most perpendicular to t. I can use a simpler dot product
    // here instead: the m that is most perpendicular to t will have the
    // smallest dot product.
    //
    // len(cross(m0/len(m0), t)) < len(cross(m1/len(m1), t)) ~
    // len(cross(v0/len(v0), t)) < len(cross(v1/len(v1), t)) ~
    // abs(dot(v0/len(v0), t)) > abs(dot(v1/len(v1), t)) ~
    // (dot(v0/len(v0), t))^2 > (dot(v1/len(v1), t))^2 ~
    // (dot(v0, t))^2 norm2(v1) > (dot(v1, t))^2 norm2(v0) ~
    if(dot_v0t.x*dot_v0t.x * dot_v1v1.x > dot_v1t.x*dot_v1t.x * dot_v0v0.x )
    {
        // Equation (12)
        vec_withgrad_t<9,3> n1 = cross<9>(v1, t01);
        v0 -= n1 * v0.dot(n1)/n1.norm2();
    }
    else
    {
        // Equation (13)
        vec_withgrad_t<9,3> n0 = cross<9>(v0, t01);
        v1 -= n0 * v1.dot(n0)/n0.norm2();
    }

    // My two 3D rays now intersect exactly, and I use compute the intersection
    // with that assumption

    vec_withgrad_t<9,3> m;
    if(!triangulate_assume_intersect(m, v0, v1, t01))
        return (mrcal_point3_t){0};

    mrcal_point3_t _m;
    m.extract_value(_m.xyz);

    if(_dm_dv0 != NULL)
        m.extract_grad (_dm_dv0->xyz,  0,3, 0,
                        3*sizeof(double), sizeof(double),
                        3);
    if(_dm_dv1 != NULL)
        m.extract_grad (_dm_dv1->xyz,  3,3, 0,
                        3*sizeof(double), sizeof(double),
                        3);
    if(_dm_dt01 != NULL)
        m.extract_grad (_dm_dt01->xyz, 6,3,0,
                        3*sizeof(double), sizeof(double),
                        3);

    return _m;
}

// Minimize L-infinity angle error. Described in "Closed-Form Optimal Two-View
// Triangulation Based on Angular Errors", Seong Hun Lee and Javier Civera. ICCV
// 2019.
extern "C"
mrcal_point3_t
mrcal_triangulate_leecivera_linf(// outputs
                                 // These all may be NULL
                                 mrcal_point3_t* _dm_dv0,
                                 mrcal_point3_t* _dm_dv1,
                                 mrcal_point3_t* _dm_dt01,

                                 // inputs

                                 // not-necessarily normalized vectors in the camera-0
                                 // coord system
                                 const mrcal_point3_t* _v0,
                                 const mrcal_point3_t* _v1,
                                 const mrcal_point3_t* _t01)
{
    // The paper has m0, m1 as the cam1-frame observation vectors. I do
    // everything in cam0-frame
    vec_withgrad_t<9,3> v0 (_v0 ->xyz, 0);
    vec_withgrad_t<9,3> v1 (_v1 ->xyz, 3);
    vec_withgrad_t<9,3> t01(_t01->xyz, 6);

    v0 /= v0.mag();
    v1 /= v1.mag();

    vec_withgrad_t<9,3> na = cross<9>(v0 + v1, t01);
    vec_withgrad_t<9,3> nb = cross<9>(v0 - v1, t01);

    vec_withgrad_t<9,3>& n =
        ( na.norm2().x > nb.norm2().x ) ?
        na : nb;

    v0 -= n * v0.dot(n)/n.norm2();
    v1 -= n * v1.dot(n)/n.norm2();

    // My two 3D rays now intersect exactly, and I use compute the intersection
    // with that assumption
    vec_withgrad_t<9,3> m;
    if(!triangulate_assume_intersect(m, v0, v1, t01))
        return (mrcal_point3_t){0};

    mrcal_point3_t _m;
    m.extract_value(_m.xyz);

    if(_dm_dv0 != NULL)
        m.extract_grad (_dm_dv0->xyz,  0,3, 0,
                        3*sizeof(double), sizeof(double),
                        3);
    if(_dm_dv1 != NULL)
        m.extract_grad (_dm_dv1->xyz,  3,3, 0,
                        3*sizeof(double), sizeof(double),
                        3);
    if(_dm_dt01 != NULL)
        m.extract_grad (_dm_dt01->xyz, 6,3,0,
                        3*sizeof(double), sizeof(double),
                        3);

    return _m;
}

// This is called "cheirality" in Lee and Civera's papers
template <int NGRAD>
static bool chirality(// output
                      val_withgrad_t<NGRAD  >& improvement0,
                      val_withgrad_t<NGRAD  >& improvement1,
                      val_withgrad_t<NGRAD  >& improvement01,

                      // input
                      const val_withgrad_t<NGRAD  >& l0,
                      const vec_withgrad_t<NGRAD,3>& v0,
                      const val_withgrad_t<NGRAD  >& l1,
                      const vec_withgrad_t<NGRAD,3>& v1,
                      const vec_withgrad_t<NGRAD,3>& t01)
{
    double len2_nominal = 0.0;
    double len2;

    improvement0  = val_withgrad_t<NGRAD>();
    improvement1  = val_withgrad_t<NGRAD>();
    improvement01 = val_withgrad_t<NGRAD>();

    for(int i=0; i<3; i++)
    {
        val_withgrad_t<NGRAD> x_nominal = ( l1*v1.v[i] + t01.v[i]) - l0*v0.v[i];
        val_withgrad_t<NGRAD> x0        = ( l1*v1.v[i] + t01.v[i]) + l0*v0.v[i];
        val_withgrad_t<NGRAD> x1        = (-l1*v1.v[i] + t01.v[i]) - l0*v0.v[i];
        val_withgrad_t<NGRAD> x01       = (-l1*v1.v[i] + t01.v[i]) + l0*v0.v[i];

        improvement0  += x0 *x0  - x_nominal*x_nominal;
        improvement1  += x1 *x1  - x_nominal*x_nominal;
        improvement01 += x01*x01 - x_nominal*x_nominal;
    }

    return
      improvement0.x  > 0.0 &&
      improvement1.x  > 0.0 &&
      improvement01.x > 0.0;
}

// version without reporting the improvement values
template <int NGRAD>
static bool chirality(const val_withgrad_t<NGRAD  >& l0,
                      const vec_withgrad_t<NGRAD,3>& v0,
                      const val_withgrad_t<NGRAD  >& l1,
                      const vec_withgrad_t<NGRAD,3>& v1,
                      const vec_withgrad_t<NGRAD,3>& t01)
{
    val_withgrad_t<NGRAD> improvement0;
    val_withgrad_t<NGRAD> improvement1;
    val_withgrad_t<NGRAD> improvement01;
    return chirality(improvement0, improvement1, improvement01,
                     l0,v0,l1,v1,t01);
}

// The "Mid2" method in "Triangulation: Why Optimize?", Seong Hun Lee and Javier
// Civera. https://arxiv.org/abs/1907.11917
extern "C"
mrcal_point3_t
mrcal_triangulate_leecivera_mid2(// outputs
                                 // These all may be NULL
                                 mrcal_point3_t* _dm_dv0,
                                 mrcal_point3_t* _dm_dv1,
                                 mrcal_point3_t* _dm_dt01,

                                 // inputs

                                 // not-necessarily normalized vectors in the camera-0
                                 // coord system
                                 const mrcal_point3_t* _v0,
                                 const mrcal_point3_t* _v1,
                                 const mrcal_point3_t* _t01)
{
    // The paper has m0, m1 as the cam1-frame observation vectors. I do
    // everything in cam0-frame
    vec_withgrad_t<9,3> v0 (_v0 ->xyz, 0);
    vec_withgrad_t<9,3> v1 (_v1 ->xyz, 3);
    vec_withgrad_t<9,3> t01(_t01->xyz, 6);

    val_withgrad_t<9> p_norm2_recip = val_withgrad_t<9>(1.0) / cross_norm2<9>(v0, v1);

    val_withgrad_t<9> l0 = (cross_norm2<9>(v1, t01) * p_norm2_recip).sqrt();
    val_withgrad_t<9> l1 = (cross_norm2<9>(v0, t01) * p_norm2_recip).sqrt();

    if(!chirality(l0, v0, l1, v1, t01))
        return (mrcal_point3_t){0};

    vec_withgrad_t<9,3> m = (v0*l0 + t01+v1*l1) / 2.0;

    mrcal_point3_t _m;
    m.extract_value(_m.xyz);

    if(_dm_dv0 != NULL)
        m.extract_grad (_dm_dv0->xyz,  0,3, 0,
                        3*sizeof(double), sizeof(double),
                        3);
    if(_dm_dv1 != NULL)
        m.extract_grad (_dm_dv1->xyz,  3,3, 0,
                        3*sizeof(double), sizeof(double),
                        3);
    if(_dm_dt01 != NULL)
        m.extract_grad (_dm_dt01->xyz, 6,3,0,
                        3*sizeof(double), sizeof(double),
                        3);

    return _m;
}

extern "C"
bool
_mrcal_triangulate_leecivera_mid2_is_convergent(// inputs

                                                // not-necessarily normalized vectors in the camera-0
                                                // coord system
                                                const mrcal_point3_t* _v0,
                                                const mrcal_point3_t* _v1,
                                                const mrcal_point3_t* _t01)
{
    mrcal_point3_t p = mrcal_triangulate_leecivera_mid2(NULL,NULL,NULL,
                                                        _v0,_v1,_t01);
    return !(p.x == 0.0 &&
             p.y == 0.0 &&
             p.z == 0.0);
}

// The "wMid2" method in "Triangulation: Why Optimize?", Seong Hun Lee and
// Javier Civera. https://arxiv.org/abs/1907.11917
extern "C"
mrcal_point3_t
mrcal_triangulate_leecivera_wmid2(// outputs
                                  // These all may be NULL
                                  mrcal_point3_t* _dm_dv0,
                                  mrcal_point3_t* _dm_dv1,
                                  mrcal_point3_t* _dm_dt01,

                                  // inputs

                                  // not-necessarily normalized vectors in the camera-0
                                  // coord system
                                  const mrcal_point3_t* _v0,
                                  const mrcal_point3_t* _v1,
                                  const mrcal_point3_t* _t01)
{
    // The paper has m0, m1 as the cam1-frame observation vectors. I do
    // everything in cam0-frame
    vec_withgrad_t<9,3> v0 (_v0 ->xyz, 0);
    vec_withgrad_t<9,3> v1 (_v1 ->xyz, 3);
    vec_withgrad_t<9,3> t01(_t01->xyz, 6);

    // Unlike Mid2 I need to normalize these here to make the math work. l0 and
    // l1 now have units of m, and I weigh by 1/l0 and 1/l1
    v0 /= v0.mag();
    v1 /= v1.mag();

    val_withgrad_t<9> p_mag_recip = val_withgrad_t<9>(1.0) / cross_mag<9>(v0, v1);

    val_withgrad_t<9> l0 = cross_mag<9>(v1, t01) * p_mag_recip;
    val_withgrad_t<9> l1 = cross_mag<9>(v0, t01) * p_mag_recip;

    if(!chirality(l0, v0, l1, v1, t01))
        return (mrcal_point3_t){0};

    vec_withgrad_t<9,3> m = (v0*l0*l1 + t01*l0 + v1*l0*l1) / (l0 + l1);

    mrcal_point3_t _m;
    m.extract_value(_m.xyz);

    if(_dm_dv0 != NULL)
        m.extract_grad (_dm_dv0->xyz,  0,3, 0,
                        3*sizeof(double), sizeof(double),
                        3);
    if(_dm_dv1 != NULL)
        m.extract_grad (_dm_dv1->xyz,  3,3, 0,
                        3*sizeof(double), sizeof(double),
                        3);
    if(_dm_dt01 != NULL)
        m.extract_grad (_dm_dt01->xyz, 6,3,0,
                        3*sizeof(double), sizeof(double),
                        3);

    return _m;
}

static
val_withgrad_t<6>
angle_error__assume_small(const vec_withgrad_t<6,3>& v0,
                          const vec_withgrad_t<6,3>& v1)
{
    const val_withgrad_t<6> inner00 = v0.norm2();
    const val_withgrad_t<6> inner11 = v1.norm2();
    const val_withgrad_t<6> inner01 = v0.dot(v1);

    val_withgrad_t<6>  costh  = inner01 / (inner00*inner11).sqrt();
    if(costh.x < 0.0)
        // Could happen with barely-divergent rays
        costh *= val_withgrad_t<6>(-1.0);

    // The angle is assumed small, so cos(th) ~ 1 - th*th/2.
    // -> th ~ sqrt( 2*(1 - cos(th)) )
    val_withgrad_t<6> th_sq = costh*(-2.) + 2.;


#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: temporary hack to avoid dividing by 0"
#endif
    if(th_sq.x < 1e-21)
    {
        return val_withgrad_t<6>();
    }


    if(th_sq.x < 0)
        // To handle roundoff errors
        th_sq.x = 0;

    return th_sq.sqrt();
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: look at numerical issues that will results in sqrt(<0)"
// #warning "triangulated-solve: look at behavior near 0 where dsqrt/dx -> inf"
#endif
}

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: maybe exposing the triangulated-error C function is OK? I'm already exposing the Python function"
#endif


static
double relu(double x, double knee)
{
    /* Smooth function ~ x>0 ? x : 0

       Three modes
       -        x < 0:    0
       - 0    < x < knee: k*x^2
       - knee < x:        x + eps

       At the transitions I want the function and the first derivative
       to be continuous. At the knee I want d/dx = 1. So 2*k*knee = 1 ->
       k = 1/(2*knee). k * knee^2 = knee + eps -> eps = knee * (k*knee -
       1) = -knee/2
    */
    if(x    <= 0) return 0.0;
    if(knee <= x) return x - knee/2.0;

    double k = 1. / (2*knee);
    return k * x*x;
}

static
val_withgrad_t<6> sigmoid(val_withgrad_t<6> x, double knee)
{
    /* Smooth function maps to 0..1

       Modes
       -        x < 0:    0
       - 0    < x < knee: smooth interpolation
       - knee < x:        1
    */
    if(x.x  <= 0  ) return 0.0;
    if(knee <= x.x) return 1.0;

    // transition at (x - knee/2.) < 0
    // f(x) = a x^2 + b x + c
    // f(-knee/2.)  = 0
    // f(0)         = 1/2
    // f'(-knee/2.) = 0
    if(x.x < knee/2.0)
        {
            double b = 2./knee;
            double a = b/knee;
            double c = 1./2.;
            return c + (x.x - knee/2.)*(b + (x.x - knee/2.)*a);
        }

    // transition at (x - knee/2.) > 0
    // f(x) = a x^2 + b x + c
    // f(knee/2.)  = 1
    // f'(knee/2.) = 0
    // f(0)         = 1/2
    {
        double b = 2./knee;
        double a = -b/knee;
        double c = 1./2.;
        return c + (x.x - knee/2.)*(b + (x.x - knee/2.)*a);
    }
}

// Internal function used in the optimization. This uses
// mrcal_triangulate_leecivera_mid2(), but contains logic in the divergent-ray
// case more appropriate for the optimization loop
extern "C"
double
_mrcal_triangulated_error(// outputs
                          mrcal_point3_t* _derr_dv1,
                          mrcal_point3_t* _derr_dt01,

                          // inputs

                          // not-necessarily normalized vectors in the camera-0
                          // coord system
                          const mrcal_point3_t* _v0,
                          const mrcal_point3_t* _v1,
                          const mrcal_point3_t* _t01)
{
    ////////////////////////// Copy of mrcal_triangulate_leecivera_mid2(). I
    ////////////////////////// extend it

    // Implementation here is a bit different: I don't propagate the gradient in
    // respect to v0

    // The paper has m0, m1 as the cam1-frame observation vectors. I do
    // everything in cam0-frame
    vec_withgrad_t<6,3> v0 (_v0 ->xyz, -1); // No gradient. Hopefully the
                                            // compiler will collapse this
                                            // aggressively
    vec_withgrad_t<6,3> v1 (_v1 ->xyz, 0);
    vec_withgrad_t<6,3> t01(_t01->xyz, 3);

    val_withgrad_t<6> p_norm2_recip = val_withgrad_t<6>(1.0) / cross_norm2<6>(v0, v1);

    val_withgrad_t<6> l0 = (cross_norm2<6>(v1, t01) * p_norm2_recip).sqrt();
    val_withgrad_t<6> l1 = (cross_norm2<6>(v0, t01) * p_norm2_recip).sqrt();

    vec_withgrad_t<6,3> m = (v0*l0 + t01+v1*l1) / 2.0;

    // I compute the angle between the triangulated point and one of the
    // observation rays, and I double this to measure from ray to ray

    // This is a fit error, which should be small. A small-angle cos()
    // approximation is valid, unless the models don't fit at all. In which
    // case a cos() error is the least of our issues
    val_withgrad_t<6> err = angle_error__assume_small( v0, m ) * 2.;

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: what happens when the rays are exactly parallel? Make sure the numerics remain happy. They don't: I divide by cross(v0,v1) ~ 0"
#endif

#if 0
    // original method
    if(!chirality(l0, v0, l1, v1, t01))
    {
        // The rays diverge. This is aphysical, but an incorrect (i.e.
        // not-yet-converged) geometry can cause this. Even if the optimization
        // has converged, this can still happen if pixel noise or an incorrect
        // feature association bumped converging rays to diverge.
        //
        // An obvious thing do wo would be to return the distance to the
        // vanishing point. This creates a yaw bias however: barely convergent
        // rays have zero effect on yaw, but barely divergent rays have a strong
        // effect on yaw
        //
        // Goals:
        //
        // - There should be no qualitative change in the cost function as rays
        //   cross over from convergent to divergent. Low-error, parallel-ish
        //   rays look out to infinity, which means that these define yaw very
        //   poorly, and would affect the pitch, roll only. Yaw is what controls
        //   divergence, so if random noise makes rays diverge, we should use
        //   the error as before, to set our pitch, roll
        //
        // - Very divergent rays are bogus, and I do apply a penalty factor
        //   based on the divergence. This penalty factor begins to kick in only
        //   past a certain point, so there's no effect at the transition. This
        //   transition point should be connected to the outlier rejection
        //   threshold.
        val_withgrad_t<6> err_to_vanishing_point = angle_error__assume_small(v0, v1);

        // I have three modes:
        //
        // - err_to_vanishing_point < THRESHOLD_DIVERGENT_LOWER
        //   I attribute the error to random noise, and simply report the
        //   reprojection error as before, ignoring the divergence. This will
        //   barely affect the yaw, but will pull the pitch and roll in the
        //   desired directions
        //
        // - err_to_vanishing_point > THRESHOLD_DIVERGENT_UPPER
        //   I add a term to pull the observation towards the vanishing point.
        //   This affects the yaw primarily, and does not touch the pitch and
        //   roll very much, since I don't have reliable information about them
        //   here
        //
        // - err_to_vanishing_point between THRESHOLD_DIVERGENT_LOWER and _UPPER
        //   I linearly the scale on this divergence term from 0 to 1

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: set reasonable thresholds"
#endif
#define THRESHOLD_DIVERGENT_LOWER 3.0e-4
#define THRESHOLD_DIVERGENT_UPPER 6.0e-4

        if(err_to_vanishing_point.x <= THRESHOLD_DIVERGENT_LOWER)
        {
            // We're barely divergent. Just do the normal thing
        }
        else if(err_to_vanishing_point.x >= THRESHOLD_DIVERGENT_UPPER)
        {
            // we're VERY divergent. Add another cost term:
            // the distance to the vanishing point
#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: temporary testing logic"
#endif
#if defined DIVERGENT_COST_ONLY && DIVERGENT_COST_ONLY
            err = err_to_vanishing_point;
#else
            err += err_to_vanishing_point;
#endif
            err.x += 200.0;
        }
        else
        {
            // linearly interpolate. As
            // err_to_vanishing_point lower->upper we
            // produce k 0->1
            val_withgrad_t<6> k =
                (err_to_vanishing_point    - THRESHOLD_DIVERGENT_LOWER) /
                (THRESHOLD_DIVERGENT_UPPER - THRESHOLD_DIVERGENT_LOWER);

#if defined ENABLE_TRIANGULATED_WARNINGS && ENABLE_TRIANGULATED_WARNINGS
// #warning "triangulated-solve: temporary testing logic"
#endif
#if defined DIVERGENT_COST_ONLY && DIVERGENT_COST_ONLY
            err = k*err_to_vanishing_point + (val_withgrad_t<6>(1.0)-k)*err;
#else
            err += k*err_to_vanishing_point;
#endif
            err.x += 100.0;
        }
    }

#else

    // new method
    val_withgrad_t<6> improvement0;
    val_withgrad_t<6> improvement1;
    val_withgrad_t<6> improvement01;
    if(!chirality(improvement0, improvement1, improvement01,
                  l0, v0, l1, v1, t01))
    {
        val_withgrad_t<6> err_to_vanishing_point = angle_error__assume_small(v0, v1);

        err +=
            err_to_vanishing_point * (sigmoid(-improvement0,  3.0) +
                                      sigmoid(-improvement1,  3.0) +
                                      sigmoid(-improvement01, 3.0));
    }

#endif

    if(_derr_dv1 != NULL)
        memcpy(_derr_dv1->xyz,
               &err.j[0],
               3*sizeof(double));
    if(_derr_dt01 != NULL)
        memcpy(_derr_dt01->xyz,
               &err.j[3],
               3*sizeof(double));

    return err.x;
}
