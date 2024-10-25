// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Apparently I need this in MSVC to get constants
#define _USE_MATH_DEFINES

#include <math.h>
#include <float.h>

#include "mrcal.h"
#include "minimath/minimath.h"
#include "util.h"

// The equivalent function in Python is _rectified_resolution_python() in
// stereo.py
//
// Documentation is in the docstring of mrcal.rectified_resolution()
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
                                 const mrcal_lensmodel_type_t rectification_model_type)
{
    // Get the rectified image resolution
    if( *pixels_per_deg_az < 0 ||
        *pixels_per_deg_el < 0)
    {
        const mrcal_point2_t azel0 = {.x = azel0_deg->x * M_PI/180.,
                                      .y = azel0_deg->y * M_PI/180. };

        // I need to compute the resolution of the rectified images. I try to
        // match the resolution of the cameras. I just look at camera0. If your
        // two cameras are different, pass in the pixels_per_deg yourself
        //
        // I look at the center of the stereo field of view. There I have q =
        // project(v) where v is a unit projection vector. I compute dq/dth where
        // th is an angular perturbation applied to v.
        double v[3];
        double dv_dazel[3*2];
        if(rectification_model_type == MRCAL_LENSMODEL_LATLON)
            mrcal_unproject_latlon((mrcal_point3_t*)v, (mrcal_point2_t*)dv_dazel,
                                   &azel0,
                                   1,
                                   (double[]){1.,1.,0.,0.});
        else if(rectification_model_type == MRCAL_LENSMODEL_LONLAT)
            mrcal_unproject_lonlat((mrcal_point3_t*)v, (mrcal_point2_t*)dv_dazel,
                                   &azel0,
                                   1,
                                   (double[]){1.,1.,0.,0.});
        else if(rectification_model_type == MRCAL_LENSMODEL_PINHOLE)
        {

            mrcal_point2_t q0_normalized = {.x = tan(azel0.x),
                                            .y = tan(azel0.y)};
            mrcal_unproject_pinhole((mrcal_point3_t*)v, (mrcal_point2_t*)dv_dazel,
                                    &q0_normalized,
                                    1,
                                    (double[]){1.,1.,0.,0.});
            // dq/dth = dtanth/dth = 1/cos^2(th)
            double cos_az0 = cos(azel0.x);
            double cos_el0 = cos(azel0.y);

            for(int i=0; i<3; i++)
            {
                dv_dazel[2*i + 0] /= cos_az0*cos_az0;
                dv_dazel[2*i + 1] /= cos_el0*cos_el0;
            }
        }
        else
        {
            MSG("Unsupported rectification model");
            return false;

        }

        double v0[3];
        mrcal_rotate_point_R(v0, NULL,NULL,
                             R_cam0_rect0,
                             v);

        // dv0_dazel  = nps.matmult(R_cam0_rect0, dv_dazel)
        double dv0_daz[3] = {0.0, 0.0, 0.0};
        double dv0_del[3] = {0.0, 0.0, 0.0};
        for(int j=0; j<3; j++)
            for(int k=0; k<3; k++)
            {
                dv0_daz[j] += R_cam0_rect0[j*3+k]*dv_dazel[2*k + 0];
                dv0_del[j] += R_cam0_rect0[j*3+k]*dv_dazel[2*k + 1];
            }

        mrcal_point2_t qdummy;
        mrcal_point3_t dq_dv0[2];
        // _,dq_dv0,_ = mrcal.project(v0, *model.intrinsics(), get_gradients = True)
        mrcal_project(&qdummy,dq_dv0,NULL,
                      (const mrcal_point3_t*)v0, 1, lensmodel, intrinsics);

        // More complex method that's probably not any better
        //
        // if False:
        //     // I rotate my v to a coordinate system where u = rotate(v) is [0,0,1].
        //     // Then u = [a,b,0] are all orthogonal to v. So du/dth = [cos, sin, 0].
        //     // I then have dq/dth = dq/dv dv/du [cos, sin, 0]t
        //     // ---> dq/dth = dq/dv dv/du[:,:2] [cos, sin]t = M [cos,sin]t
        //     //
        //     // norm2(dq/dth) = [cos,sin] MtM [cos,sin]t is then an ellipse with the
        //     // eigenvalues of MtM giving me the best and worst sensitivities. I can
        //     // use mrcal.worst_direction_stdev() to find the densest direction. But I
        //     // actually know the directions I care about, so I evaluate them
        //     // independently for the az and el directions
        //     Ruv = mrcal.R_aligned_to_vector(v0);
        //     M = nps.matmult(dq_dv0, nps.transpose(Ruv[:2,:]))
        //     // I pick the densest direction: highest |dq/dth|
        //     pixels_per_rad = mrcal.worst_direction_stdev( nps.matmult( nps.transpose(M),M) );

        // dq_dazel = nps.matmult(dq_dv0, dv0_dazel)
        double dq_daz[2] =
            { dq_dv0[0].x*dv0_daz[0] + dq_dv0[0].y*dv0_daz[1] + dq_dv0[0].z*dv0_daz[2],
              dq_dv0[1].x*dv0_daz[0] + dq_dv0[1].y*dv0_daz[1] + dq_dv0[1].z*dv0_daz[2] };
        double dq_del[2] =
            { dq_dv0[0].x*dv0_del[0] + dq_dv0[0].y*dv0_del[1] + dq_dv0[0].z*dv0_del[2],
              dq_dv0[1].x*dv0_del[0] + dq_dv0[1].y*dv0_del[1] + dq_dv0[1].z*dv0_del[2] };

        if(*pixels_per_deg_az < 0)
        {
            double dq_daz_norm2 = 0.;
            for(int i=0; i<2; i++) dq_daz_norm2 += dq_daz[i]*dq_daz[i];
            double pixels_per_deg_az_have = sqrt(dq_daz_norm2)*M_PI/180.;
            *pixels_per_deg_az *= -pixels_per_deg_az_have;
        }

        if(*pixels_per_deg_el < 0)
        {
            double dq_del_norm2 = 0.;
            for(int i=0; i<2; i++) dq_del_norm2 += dq_del[i]*dq_del[i];
            double pixels_per_deg_el_have = sqrt(dq_del_norm2)*M_PI/180.;
            *pixels_per_deg_el *= -pixels_per_deg_el_have;
        }
    }

    // I now have the desired pixels_per_deg
    //
    // With LENSMODEL_LATLON or LENSMODEL_LONLAT we have even angular spacing, so
    // q = f th + c -> dq/dth = f everywhere. I can thus compute the rectified
    // image size and adjust the resolution accordingly
    //
    // With LENSMODEL_PINHOLE this is much more complex, so this function just
    // leaves the desired pixels_per_deg as it is
    if(rectification_model_type == MRCAL_LENSMODEL_LATLON ||
       rectification_model_type == MRCAL_LENSMODEL_LONLAT)
    {
        int Naz = (int)round(azel_fov_deg->x * (*pixels_per_deg_az));
        int Nel = (int)round(azel_fov_deg->y * (*pixels_per_deg_el));

        *pixels_per_deg_az = (double)Naz/azel_fov_deg->x;
        *pixels_per_deg_el = (double)Nel/azel_fov_deg->y;
    }

    return true;
}

// The equivalent function in Python is _rectified_system_python() in stereo.py
//
// Documentation is in the docstring of mrcal.rectified_system()
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
                            bool   el_fov_deg_autodetect)
{
    if(el0_deg_autodetect)
    {
        MSG("el0_deg_autodetect is unsupported");
        return false;
    }
    if(az_fov_deg_autodetect)
    {
        MSG("az_fov_deg_autodetect is unsupported");
        return false;
    }
    if(el_fov_deg_autodetect)
    {
        MSG("el_fov_deg_autodetect is unsupported");
        return false;
    }

    if( !(rectification_model_type == MRCAL_LENSMODEL_LATLON ||
          rectification_model_type == MRCAL_LENSMODEL_PINHOLE) )
    {
        MSG("Unsupported rectification model '%s'. Only LENSMODEL_LATLON and LENSMODEL_PINHOLE are supported",
            mrcal_lensmodel_name_unconfigured( &(mrcal_lensmodel_t){.type = rectification_model_type}));
        return false;
    }

    mrcal_lensmodel_metadata_t meta =
        mrcal_lensmodel_metadata( lensmodel0 );
    if(meta.noncentral)
    {
        if(lensmodel0->type == MRCAL_LENSMODEL_CAHVORE)
        {
            // CAHVORE is generally noncentral, but if E=0, then it is
            const int Nintrinsics = mrcal_lensmodel_num_params(lensmodel0);
            for(int i=Nintrinsics-3; i<Nintrinsics; i++)
                if(intrinsics0[i] != 0)
                {
                    MSG("Stereo rectification is only possible with a central projection. Please centralize your models. This is CAHVORE, so set E=0 to centralize. This will ignore all noncentral effects near the lens");
                    return false;
                }
        }
        else
        {
            MSG("Stereo rectification is only possible with a central projection. Please centralize your models");
            return false;
        }
    }

    ///// TODAY this C implementation supports MRCAL_LENSMODEL_LATLON only. This
    ///// isn't a design choice, I just don't want to do the extra work yet. The
    ///// API already is general enough to support both rectification schemes.
    if( rectification_model_type != MRCAL_LENSMODEL_LATLON )
    {
        MSG("Today this C implementation supports MRCAL_LENSMODEL_LATLON only.");
        return false;
    }


    if(*pixels_per_deg_az == 0)
    {
        MSG("pixels_per_deg_az == 0 is illegal. Must be >0 if we're trying to specify a value, or <0 to autodetect");
        return false;
    }
    if(*pixels_per_deg_el == 0)
    {
        MSG("pixels_per_deg_el == 0 is illegal. Must be >0 if we're trying to specify a value, or <0 to autodetect");
        return false;
    }
    if( azel_fov_deg->x <= 0. ||
        azel_fov_deg->y <= 0.)
    {
        MSG("az_fov_deg, el_fov_deg must be > 0. No auto-detection implemented yet");
        return false;
    }


    // Compute the geometry of the rectified stereo system. This is a
    // rotation, centered at camera0. More or less we have axes:
    //
    // x: from camera0 to camera1
    // y: completes the system from x,z
    // z: component of the cameras' viewing direction
    //    normal to the baseline
    double Rt_cam0_ref[4*3];
    double Rt_cam1_ref[4*3];
    mrcal_Rt_from_rt(Rt_cam0_ref, NULL, rt_cam0_ref);
    mrcal_Rt_from_rt(Rt_cam1_ref, NULL, rt_cam1_ref);

    double Rt01[4*3];
    double Rt_ref_cam1[4*3];
    mrcal_invert_Rt(Rt_ref_cam1,Rt_cam1_ref);
    mrcal_compose_Rt(Rt01, Rt_cam0_ref, Rt_ref_cam1);

    // Rotation relating camera0 coords to the rectified camera coords. I fill in
    // each row separately
    double Rt_rect0_cam0[4*3] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double* R_rect0_cam0 = Rt_rect0_cam0;

    // Axes of the rectified system, in the cam0 coord system
    double* right       = &R_rect0_cam0[0*3 + 0];
    double* down        = &R_rect0_cam0[1*3 + 0];
    double* forward     = &R_rect0_cam0[2*3 + 0];

    // "right" of the rectified coord system: towards the origin of camera1 from
    // camera0, in camera0 coords
    for(int i=0; i<3; i++) right[i] = Rt01[3*3 + i];
    *baseline = 0.0;
    for(int i=0; i<3; i++) *baseline += right[i]*right[i];
    *baseline = sqrt(*baseline);

    if(*baseline < 1e-6)
    {
        MSG("The stereo pair has a unnaturally small baseline. Did you accidentally pass the same model for the two cameras?");
        return false;
    }

    for(int i=0; i<3; i++) right[i] /= (*baseline);

    // "forward" of the rectified coord system, in camera0 coords. The mean
    // optical-axis direction of the two cameras: component orthogonal to "right"
    double forward01[3] =
        {
            Rt01[0*3 + 2],
            Rt01[1*3 + 2],
            Rt01[2*3 + 2] + 1.,
        };
    double forward01_proj_right = 0.0;
    for(int i=0; i<3; i++) forward01_proj_right += forward01[i]*right[i];
    for(int i=0; i<3; i++) forward[i] = forward01[i] - forward01_proj_right*right[i];
    double norm2_forward = 0.;
    for(int i=0; i<3; i++) norm2_forward += forward[i]*forward[i];
    for(int i=0; i<3; i++) forward[i] /= sqrt(norm2_forward);

    // "down" of the rectified coord system, in camera0 coords. Completes the
    // right,down,forward coordinate system
    // down = cross(forward,right)
    down[0] = forward[1]*right[2] - forward[2]*right[1];
    down[1] = forward[2]*right[0] - forward[0]*right[2];
    down[2] = forward[0]*right[1] - forward[1]*right[0];

    // Done with the geometry! Now to get the az/el grid. I need to figure
    // out the resolution and the extents

    mrcal_point2_t azel0 = { .x = azel0_deg->x * M_PI/180.,
                             .y = azel0_deg->y * M_PI/180. };
    if(az0_deg_autodetect)
    {
        // In the rectified system az=0 sits perpendicular to the baseline.
        // Normally the cameras are looking out perpendicular to the baseline
        // also, so I center my azimuth samples around 0 to match the cameras'
        // field of view. But what if the geometry isn't square, and one camera
        // is behind the other? Like this:
        //
        //    camera
        //     view
        //       ^
        //       |
        //     \ | /
        //      \_/
        //        .    /
        //         .  /az=0
        //          ./
        //           .
        //  baseline  .
        //             .
        //            \   /
        //             \_/
        //
        // Here the center-of-view axis of each camera is not at all
        // perpendicular to the baseline. Thus I compute the mean "forward"
        // direction of the cameras in the rectified system, and set that as the
        // center azimuth az0.
        double norm2_forward01 = 0.0;
        for(int i=0; i<3; i++) norm2_forward01 += forward01[i]*forward01[i];
        azel0.x = asin( forward01_proj_right / sqrt(norm2_forward01) );
        azel0_deg->x = azel0.x * 180./M_PI;
    }

    double R_cam0_rect0[3*3];
    mrcal_invert_R(R_cam0_rect0, R_rect0_cam0);

    if(!mrcal_rectified_resolution( // output
                                    pixels_per_deg_az,
                                    pixels_per_deg_el,
                                    // input
                                    lensmodel0,
                                    intrinsics0,
                                    azel_fov_deg,
                                    azel0_deg,
                                    R_cam0_rect0,
                                    rectification_model_type))
        return false;

    // How do we apply the desired pixels_per_deg?
    //
    // With LENSMODEL_LATLON we have even angular spacing, so q = f th + c ->
    // dq/dth = f everywhere.
    //
    // With LENSMODEL_PINHOLE the angular resolution changes across the image: q
    // = f tan(th) + c -> dq/dth = f/cos^2(th). So at the center, th=0 and we
    // have the maximum resolution
    fxycxy_rectified[0] = *pixels_per_deg_az / M_PI*180.;
    fxycxy_rectified[1] = *pixels_per_deg_el / M_PI*180.;

    // if rectification_model == 'LENSMODEL_LATLON':
    //     # The angular resolution is consistent everywhere, so fx,fy are already
    //     # set. Let's set cx,cy such that
    //     # (az0,el0) = unproject(imager center)
    //     Naz = round(az_fov_deg*pixels_per_deg_az)
    //     Nel = round(el_fov_deg*pixels_per_deg_el)
    imagersize_rectified[0] = (int)round(azel_fov_deg->x * (*pixels_per_deg_az));
    imagersize_rectified[1] = (int)round(azel_fov_deg->y * (*pixels_per_deg_el));

    // fxycxy[2:] =
    //     np.array(((Naz-1.)/2.,(Nel-1.)/2.)) -
    //     np.array((az0,el0)) * fxycxy[:2]
    fxycxy_rectified[2] = ((double)(imagersize_rectified[0] - 1)) / 2 - azel0.x * fxycxy_rectified[0];
    fxycxy_rectified[3] = ((double)(imagersize_rectified[1] - 1)) / 2 - azel0.y * fxycxy_rectified[1];

    if(imagersize_rectified[1] <= 0)
    {
        MSG("Resulting stereo geometry has Nel=%d. This is nonsensical. You should examine the geometry or adjust the elevation bounds or pixels-per-deg",
            imagersize_rectified[1]);
        return false;
    }

    // The geometry
    double Rt_rect0_ref[4*3];
    mrcal_compose_Rt(Rt_rect0_ref,
                     Rt_rect0_cam0, Rt_cam0_ref);
    mrcal_rt_from_Rt(rt_rect0_ref, NULL, Rt_rect0_ref);

    return true;
}

static
void set_rectification_map_pixel(// output
                                 float* rectification_map0,
                                 float* rectification_map1,
                                 // input

                                 const int i, const int j,
                                 const mrcal_point3_t* v,

                                 const mrcal_lensmodel_t* lensmodel0,
                                 const double*            intrinsics0,
                                 const double*            R_cam0_rect,

                                 const mrcal_lensmodel_t* lensmodel1,
                                 const double*            intrinsics1,
                                 const double*            R_cam1_rect,

                                 const unsigned int*      imagersize_rectified)
{
    mrcal_point3_t vcam;
    mrcal_point2_t q;

    vcam = *v;
    mrcal_rotate_point_R(vcam.xyz, NULL, NULL,
                         R_cam0_rect, v->xyz);
    mrcal_project(&q, NULL, NULL,
                  &vcam, 1,
                  lensmodel0, intrinsics0);
    rectification_map0[(i*imagersize_rectified[0] + j)*2 + 0] = (float)q.x;
    rectification_map0[(i*imagersize_rectified[0] + j)*2 + 1] = (float)q.y;

    vcam = *v;
    mrcal_rotate_point_R(vcam.xyz, NULL, NULL,
                         R_cam1_rect, v->xyz);
    mrcal_project(&q, NULL, NULL,
                  &vcam, 1,
                  lensmodel1, intrinsics1);
    rectification_map1[(i*imagersize_rectified[0] + j)*2 + 0] = (float)q.x;
    rectification_map1[(i*imagersize_rectified[0] + j)*2 + 1] = (float)q.y;
}

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
                              const double*                r_rect0_ref)
{
    if( ! (rectification_model_type == MRCAL_LENSMODEL_LATLON ||
           rectification_model_type == MRCAL_LENSMODEL_PINHOLE) )
    {
        MSG("%s() supports MRCAL_LENSMODEL_LATLON and MRCAL_LENSMODEL_PINHOLE only",
            __func__);
        return false;
    }

    double R_cam0_ref[3*3];
    double R_cam1_ref[3*3];
    mrcal_R_from_r(R_cam0_ref, NULL, r_cam0_ref);
    mrcal_R_from_r(R_cam1_ref, NULL, r_cam1_ref);

    double R_cam0_rect[3*3];
    double R_cam1_rect[3*3];

    double R_rect0_ref[3*3];
    mrcal_R_from_r(R_rect0_ref, NULL, r_rect0_ref);

    mul_genN3_gen33t_vout(3, R_cam0_ref, R_rect0_ref, R_cam0_rect);
    mul_genN3_gen33t_vout(3, R_cam1_ref, R_rect0_ref, R_cam1_rect);

    float* rectification_map0 = &(rectification_maps[0]);
    float* rectification_map1 = &(rectification_maps[imagersize_rectified[0]*imagersize_rectified[1]*2]);

    // I had this:
    //   for(int i=0; i<imagersize_rectified[1]; i++)
    //       for(int j=0; j<imagersize_rectified[0]; j++)
    //       {
    //           mrcal_point2_t q = {.x = j, .y = i};
    //           mrcal_point3_t v;
    //           mrcal_unproject_latlon(&v, NULL,
    //                                  &q,
    //                                  1,
    //                                  fxycxy_rectified);
    //           ....
    //
    // I'm inlining the mrcal_unproject_latlon() call, and moving some constant
    // guts outside the loops.
    //
    //  And I'm computing sin,cos incrementally:
    //   sin(x0 + dx) = sin(x0)*cos(dx) + cos(x0)*sin(dx)
    //   cos(x0 + dx) = cos(x0)*cos(dx) - sin(x0)*sin(dx)
    //
    // Since dx is constant here I can compute the sin/cos sequence very
    // quickly. One concern about this is that each computation would accumulate
    // floating-point error, which could add up. The test-rectification-maps.py
    // test explicitly checks for this, and determines that this isn't an issue
    const double fx         = fxycxy_rectified[0];
    const double fy         = fxycxy_rectified[1];

    const double fx_recip   = 1./fx;
    const double fy_recip   = 1./fy;
    const double c_over_f_x = fxycxy_rectified[2] * fx_recip;
    const double c_over_f_y = fxycxy_rectified[3] * fy_recip;

    if(rectification_model_type == MRCAL_LENSMODEL_LATLON)
    {
        double sdlon = sin(fy_recip);
        double cdlon = cos(fy_recip);
        double sdlat = sin(fx_recip);
        double cdlat = cos(fx_recip);

        double lon0 = -c_over_f_y;
        double slon0 = sin(lon0);
        double clon0 = cos(lon0);

        double lat0 = -c_over_f_x;
        double slat0 = sin(lat0);
        double clat0 = cos(lat0);

        double slon = slon0, clon = clon0;
        for(unsigned int i=0; i<imagersize_rectified[1]; i++)
        {
            double slat = slat0, clat = clat0;
            for(unsigned int j=0; j<imagersize_rectified[0]; j++)
            {
                mrcal_point3_t v =
                    (mrcal_point3_t){.x = slat,
                                     .y = clat * slon,
                                     .z = clat * clon};

                set_rectification_map_pixel( rectification_map0,
                                             rectification_map1,
                                             i,j,&v,
                                             lensmodel0,
                                             intrinsics0,
                                             R_cam0_rect,
                                             lensmodel1,
                                             intrinsics1,
                                             R_cam1_rect,
                                             imagersize_rectified);

                double _slat = slat;
                slat = _slat*cdlat +  clat*sdlat;
                clat =  clat*cdlat - _slat*sdlat;
            }
            double _slon = slon;
            slon = _slon*cdlon +  clon*sdlon;
            clon =  clon*cdlon - _slon*sdlon;
        }
    }
    else
    {
        // MRCAL_LENSMODEL_PINHOLE
        for(unsigned int i=0; i<imagersize_rectified[1]; i++)
        {
            for(unsigned int j=0; j<imagersize_rectified[0]; j++)
            {
                mrcal_point3_t v =
                    (mrcal_point3_t){.x = (double)j*fx_recip - c_over_f_x,
                                     .y = (double)i*fy_recip - c_over_f_y,
                                     .z = 1.0};

                set_rectification_map_pixel( rectification_map0,
                                             rectification_map1,
                                             i,j,&v,
                                             lensmodel0,
                                             intrinsics0,
                                             R_cam0_rect,
                                             lensmodel1,
                                             intrinsics1,
                                             R_cam1_rect,
                                             imagersize_rectified);
            }
        }
    }


    return true;
}


// Logic from GetColorValueFromFormula() in src/getcolor.c in the gnuplot
// sources
static bool
gnuplot_color_formula(uint8_t* out,
                      int formula, float x)
{
    /* the input gray x is supposed to be in interval [0,1] */
    if (formula < 0) {		/* negate the value for negative formula */
	x = 1.f - x;
	formula = -formula;
    }

    switch (formula) {
    case 0:
	x = 0.f;
        break;
    case 1:
	x = 0.5f;
        break;
    case 2:
	x = 1.f;
        break;
    case 3:			/* x = x */
	break;
    case 4:
	x = x * x;
	break;
    case 5:
	x = x * x * x;
	break;
    case 6:
	x = x * x * x * x;
	break;
    case 7:
	x = sqrtf(x);
	break;
    case 8:
	x = sqrtf(sqrtf(x));
	break;
    case 9:
	x = sinf(90.f * x * M_PI/180.f);
	break;
    case 10:
	x = cosf(90.f * x * M_PI/180.f);
	break;
    case 11:
	x = fabsf(x - 0.5f);
	break;
    case 12:
	x = (2.f * x - 1.f) * (2.0 * x - 1.f);
	break;
    case 13:
	x = sinf(180.f * x * M_PI/180.f);
	break;
    case 14:
	x = fabsf(cosf(180.f * x * M_PI/180.f));
	break;
    case 15:
	x = sinf(360.f * x * M_PI/180.f);
	break;
    case 16:
	x = cosf(360.f * x * M_PI/180.f);
	break;
    case 17:
	x = fabsf(sinf(360.f * x * M_PI/180.f));
	break;
    case 18:
	x = fabsf(cosf(360.f * x * M_PI/180.f));
	break;
    case 19:
	x = fabsf(sinf(720.f * x * M_PI/180.f));
	break;
    case 20:
	x = fabsf(cosf(720.f * x * M_PI/180.f));
	break;
    case 21:
	x = 3.f * x;
	break;
    case 22:
	x = 3.f * x - 1.f;
	break;
    case 23:
	x = 3.f * x - 2.f;
	break;
    case 24:
	x = fabsf(3.f * x - 1.f);
	break;
    case 25:
	x = fabsf(3.f * x - 2.f);
	break;
    case 26:
	x = (1.5f * x - 0.5f);
	break;
    case 27:
	x = (1.5f * x - 1.f);
	break;
    case 28:
	x = fabsf(1.5f * x - 0.5f);
	break;
    case 29:
	x = fabsf(1.5f * x - 1.f);
	break;
    case 30:
	if (x <= 0.25f)
	    x = 0.f;
	else if (x >= 0.57f)
	    x = 1.f;
	else
            x = x / 0.32f - 0.78125f;
	break;
    case 31:
	if (x <= 0.42f)
	    x = 0.f;
	else if (x >= 0.92f)
	    x = 1.f;
	else
            x = 2.f * x - 0.84f;
	break;
    case 32:
	if (x <= 0.42f)
	    x *= 4.f;
	else
	    x = (x <= 0.92f) ? -2.f * x + 1.84f : x / 0.08f - 11.5f;
	break;
    case 33:
	x = fabsf(2.f * x - 0.5f);
	break;
    case 34:
	x = 2.f * x;
	break;
    case 35:
	x = 2.f * x - 0.5f;
	break;
    case 36:
	x = 2.f * x - 1.f;
	break;
    default:
        return false;
    }
    if      (x <= 0.f) x = 0.f;
    else if (x >= 1.f) x = 1.f;

    // round to nearest integer
    *out = (uint8_t)(0.5f + 255.0f*x);
    return true;
}

// Color-code an array

// I use gnuplot's color-mapping functions to do this. gnuplot's "show palette"
// help message displays the current gnuplot settings and "test palette"
// displays the palette. The function definitions are given by "show palette
// rgbformulae":
//
//     > show palette rgbformulae
//      * there are 37 available rgb color mapping formulae:
//         0: 0               1: 0.5             2: 1
//         3: x               4: x^2             5: x^3
//         6: x^4             7: sqrt(x)         8: sqrt(sqrt(x))
//         9: sin(90x)       10: cos(90x)       11: |x-0.5|
//        12: (2x-1)^2       13: sin(180x)      14: |cos(180x)|
//        15: sin(360x)      16: cos(360x)      17: |sin(360x)|
//        18: |cos(360x)|    19: |sin(720x)|    20: |cos(720x)|
//        21: 3x             22: 3x-1           23: 3x-2
//        24: |3x-1|         25: |3x-2|         26: (3x-1)/2
//        27: (3x-2)/2       28: |(3x-1)/2|     29: |(3x-2)/2|
//        30: x/0.32-0.78125 31: 2*x-0.84       32: 4x;1;-2x+1.84;x/0.08-11.5
//        33: |2*x - 0.5|    34: 2*x            35: 2*x - 0.5
//        36: 2*x - 1
//      * negative numbers mean inverted=negative colour component
//      * thus the ranges in `set pm3d rgbformulae' are -36..36
#define DEFINE_mrcal_apply_color_map(T,Tname,T_MIN,T_MAX)               \
    bool mrcal_apply_color_map_##Tname(                                 \
        mrcal_image_bgr_t*    out,                                      \
        const mrcal_image_##Tname##_t* in,                              \
                                                                        \
        /* If true, I set in_min/in_max from the */                     \
        /* min/max of the input data */                                 \
        const bool auto_min,                                            \
        const bool auto_max,                                            \
                                                                        \
        /* If true, I implement gnuplot's default 7,5,15 mapping. */    \
        /* This is a reasonable default choice. */                      \
        /* function_red/green/blue are ignored if true */               \
        const bool auto_function,                                       \
                                                                        \
        /* min/max input values to use if not */                        \
        /* auto_min/auto_max */                                         \
        T in_min, /* will map to 0 */                                   \
        T in_max, /* will map to 255 */                                 \
                                                                        \
        /* The color mappings to use. If !auto_function */              \
        int function_red,                                               \
        int function_green,                                             \
        int function_blue)                                              \
{                                                                       \
    const int w = in->width;                                            \
    const int h = in->height;                                           \
    if(!(w == out->width && h == out->height))                          \
    {                                                                   \
        MSG("%s(): input and output images MUST have the same dimensions", \
            __func__);                                                  \
        return false;                                                   \
    }                                                                   \
                                                                        \
    if(auto_min || auto_max)                                            \
    {                                                                   \
        if(auto_min) in_min = T_MAX;                                    \
        if(auto_max) in_max = T_MIN;                                    \
                                                                        \
        for(int y=0; y<h; y++)                                          \
            for(int x=0; x<w; x++)                                      \
            {                                                           \
                const T v = *mrcal_image_##Tname##_at_const(in,  x,y);  \
                if(auto_min && v < in_min) in_min = v;                  \
                if(auto_max && v > in_max) in_max = v;                  \
            }                                                           \
    }                                                                   \
                                                                        \
    if(auto_function)                                                   \
    {                                                                   \
        function_red   = 7;                                             \
        function_green = 5;                                             \
        function_blue  = 15;                                            \
    }                                                                   \
                                                                        \
    for(int y=0; y<h; y++)                                              \
    {                                                                   \
        for(int x=0; x<w; x++)                                          \
        {                                                               \
            const T* in_T = mrcal_image_##Tname##_at_const(in,  x,y);   \
                                                                        \
            mrcal_bgr_t* out_bgr = mrcal_image_bgr_at(     out, x,y);   \
                                                                        \
            float x;                                                    \
            if     (*in_T <= in_min)                                    \
                x = 0.0f;                                               \
            else if(*in_T >= in_max)                                    \
                x = 1.0f;                                               \
            else                                                        \
                x = (float)(*in_T - in_min) / (float)(in_max - in_min); \
                                                                        \
            if(!gnuplot_color_formula(&out_bgr->bgr[2], function_red,   x) || \
               !gnuplot_color_formula(&out_bgr->bgr[1], function_green, x) || \
               !gnuplot_color_formula(&out_bgr->bgr[0], function_blue,  x)) \
                return false;                                           \
        }                                                               \
    }                                                                   \
                                                                        \
    return true;                                                        \
}

DEFINE_mrcal_apply_color_map(uint8_t,  uint8,  0,         UINT8_MAX)
DEFINE_mrcal_apply_color_map(uint16_t, uint16, 0,         UINT16_MAX)
DEFINE_mrcal_apply_color_map(uint32_t, uint32, 0,         UINT32_MAX)
DEFINE_mrcal_apply_color_map(uint64_t, uint64, 0,         UINT64_MAX)

DEFINE_mrcal_apply_color_map(int8_t,   int8,   INT8_MIN,  INT8_MAX)
DEFINE_mrcal_apply_color_map(int16_t,  int16,  INT16_MIN, INT16_MAX)
DEFINE_mrcal_apply_color_map(int32_t,  int32,  INT32_MIN, INT32_MAX)
DEFINE_mrcal_apply_color_map(int64_t,  int64,  INT64_MIN, INT64_MAX)

DEFINE_mrcal_apply_color_map(float,    float,  FLT_MIN,   FLT_MAX)
DEFINE_mrcal_apply_color_map(double,   double, DBL_MIN,   DBL_MAX)



static bool _validate_rectification_model_type(const mrcal_lensmodel_type_t rectification_model_type)
{
    if(rectification_model_type == MRCAL_LENSMODEL_LATLON ||
       rectification_model_type == MRCAL_LENSMODEL_PINHOLE)
        return true;

    // ERROR

    const char* rectification_model_string =
        mrcal_lensmodel_name_unconfigured( &(mrcal_lensmodel_t){.type = rectification_model_type} );
    if(rectification_model_string == NULL)
    {
        MSG("Unknown rectification_model_type=%d\n", rectification_model_type);
        return false;
    }

    MSG("Invalid rectification_model_type for rectification: %s; I know about MRCAL_LENSMODEL_LATLON and MRCAL_LENSMODEL_PINHOLE\n",
        rectification_model_string);
    return false;
}

static double _stereo_range_one(const double                 disparity,
                                const mrcal_point2_t         qrect0,

                                // models_rectified
                                const mrcal_lensmodel_type_t rectification_model_type,
                                const double*                fxycxy_rectified,
                                const double                 baseline )
{
    // See the docstring for mrcal.stereo_range() for the derivation
    const double fx = fxycxy_rectified[0];
    const double fy = fxycxy_rectified[1];
    const double cx = fxycxy_rectified[2];
    const double cy = fxycxy_rectified[3];

    if(rectification_model_type == MRCAL_LENSMODEL_LATLON)
    {
        const double az0           = (qrect0.x - cx)/fx;
        const double disparity_rad = disparity / fx;

        const double range =
            baseline *
            cos(az0 - disparity_rad) / sin(disparity_rad);

        if(!isfinite(range)) return 0.0;
        return range;
    }

    // _validate_rectification_model_type() makes sure this is true
    // if(rectification_model_type == MRCAL_LENSMODEL_PINHOLE)
    {
        const double tanaz0        = (qrect0.x - cx) / fx;
        const double tanel         = (qrect0.y - cy) / fy;
        const double s_sq_recip    = tanel*tanel + 1.;
        const double tanaz0_tanaz1 = disparity / fx;
        const double tanaz1        = tanaz0 - tanaz0_tanaz1;

        const double range =
            baseline /
            sqrt(s_sq_recip + tanaz0*tanaz0) *
            ( (s_sq_recip + tanaz0*tanaz1) / tanaz0_tanaz1 +
              tanaz0 );

        if(!isfinite(range)) return 0.0;
        return range;
    }
}

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
                               const double                 baseline)
{
    if(!_validate_rectification_model_type(rectification_model_type))
        return false;

    if(disparity_min >= disparity_max)
    {
        MSG("Must have disparity_max > disparity_min");
        return false;
    }

    for(int i=0; i<N; i++)
    {
        if(disparity[i] <= 0.0          ||
           disparity[i] < disparity_min ||
           disparity[i] > disparity_max )
        {
            range[i] = 0.0;
        }
        else
            range[i] = _stereo_range_one(disparity[i],
                                         qrect0[i],
                                         rectification_model_type,
                                         fxycxy_rectified,
                                         baseline);
    }

    return true;
}

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
                              const double                 baseline)
{
    if(!_validate_rectification_model_type(rectification_model_type))
        return false;

    if(disparity_scaled_min >= disparity_scaled_max)
    {
        MSG("Must have disparity_scaled_max > disparity_scaled_min");
        return false;
    }

    if(range->width  != disparity_scaled->width ||
       range->height != disparity_scaled->height)
    {
        MSG("range and disparity_scaled MUST have the same dimensions. Got (W,H): (%d,%d) and (%d,%d) respectively\n",
            range->width,            range->height,
            disparity_scaled->width, disparity_scaled->height);
        return false;
    }

    const int W = range->width;
    const int H = range->height;

    for(int i=0; i<H; i++)
    {
        double*         r_row = mrcal_image_double_at(       range,           0, i);
        const uint16_t* d_row = mrcal_image_uint16_at_const(disparity_scaled, 0, i);

        for(int j=0; j<W; j++)
        {
            if(d_row[j] <= 0.0                 ||
               d_row[j] < disparity_scaled_min ||
               d_row[j] > disparity_scaled_max )
            {
                r_row[j] = 0.0;
            }
            else
                r_row[j] =
                    _stereo_range_one((double)(d_row[j]) / (double)disparity_scale,
                                      (mrcal_point2_t){.x = (double)j,
                                                       .y = (double)i},
                                      rectification_model_type,
                                      fxycxy_rectified,
                                      baseline);
        }
    }

    return true;
}