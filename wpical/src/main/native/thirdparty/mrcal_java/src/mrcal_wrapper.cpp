/*
 * Copyright (C) Photon Vision.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "mrcal_wrapper.h"


#include <stdint.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <random>
#include <span>
#include <stdexcept>
#include <vector>

using namespace cv;

class CholmodCtx {
public:
  cholmod_common Common, *cc;
  CholmodCtx() {
    cc = &Common;
    cholmod_l_start(cc);
  }

  ~CholmodCtx() { cholmod_l_finish(cc); }
};
static CholmodCtx cctx;

#define BARF(...) std::fprintf(stderr, __VA_ARGS__)

// forward declarations for functions borrowed from mrcal-pywrap
static mrcal_problem_selections_t construct_problem_selections(
    int do_optimize_intrinsics_core, int do_optimize_intrinsics_distortions,
    int do_optimize_extrinsics, int do_optimize_frames,
    int do_optimize_calobject_warp, int do_apply_regularization,
    int do_apply_outlier_rejection, int Ncameras_intrinsics,
    int Ncameras_extrinsics, int Nframes, int Nobservations_board);

bool lensmodel_one_validate_args(mrcal_lensmodel_t *mrcal_lensmodel,
                                 std::vector<double> intrinsics,
                                 bool do_check_layout);

mrcal_point3_t* observations_point = nullptr;
mrcal_pose_t*
    extrinsics_rt_fromref = nullptr; // Always zero for single camera, it seems?
mrcal_point3_t* points = nullptr;     // Seems to always to be None for single camera?

static std::unique_ptr<mrcal_result> mrcal_calibrate(
    // List, depth is ordered array observation[N frames, object_height,
    // object_width] = [x,y, weight] weight<0 means ignored)
    std::span<mrcal_point3_t> observations_board,
    // RT transform from camera to object
    std::span<mrcal_pose_t> frames_rt_toref,
    // Chessboard size, in corners (not squares)
    Size calobjectSize, double calibration_object_spacing,
    // res, pixels
    Size cameraRes,
    // solver options
    mrcal_problem_selections_t problem_selections,
    // seed intrinsics/cameramodel to optimize for
    mrcal_lensmodel_t mrcal_lensmodel, std::vector<double> intrinsics) {
  // Number of board observations we've got. List of boards. in python, it's
  // (number of chessboard pictures) x (rows) x (cos) x (3)
  // hard-coded to 8, since that's what I've got below
  int Nobservations_board = frames_rt_toref.size();

  // Looks like this is hard-coded to 0 in Python for initial single-camera
  // solve?
  int Nobservations_point = 0;

  int calibration_object_width_n =
      calobjectSize.width; // Object width, in # of corners
  int calibration_object_height_n =
      calobjectSize.height; // Object height, in # of corners

  // TODO set sizes and populate
  int imagersize[] = {cameraRes.width, cameraRes.height};

  mrcal_calobject_warp_t calobject_warp = {0, 0};

  // int Nobservations_point_triangulated = 0; // no clue what this is

  int Npoints = 0;       // seems like this is also unused? whack
  int Npoints_fixed = 0; // seems like this is also unused? whack

  // Number of cameras to solve for intrinsics
  int Ncameras_intrinsics = 1;

  // Hard-coded to match out 8 frames from above (borrowed from python)
  std::vector<mrcal_point3_t> indices_frame_camintrinsics_camextrinsics;
  // Frame index, camera number, (camera number)-1???
  for (int i = 0; i < Nobservations_board; i++) {
    indices_frame_camintrinsics_camextrinsics.push_back(
        {static_cast<double>(i), 0, -1});
  }

  // Pool is the raw observation backing array
  mrcal_point3_t *c_observations_board_pool = (observations_board.data());
  mrcal_point3_t *c_observations_point_pool = observations_point;

  // Copy from board/point pool above, using some code borrowed from
  // mrcal-pywrap
  std::vector<mrcal_observation_board_t> observations_board_data(Nobservations_board);
  auto c_observations_board = observations_board_data.data();
  // Try to make sure we don't accidentally make a zero-length array or
  // something stupid
  std::vector<mrcal_observation_point_t>
      observations_point_data(std::max(Nobservations_point, 1));
  mrcal_observation_point_t*
      c_observations_point = observations_point_data.data();

  for (int i_observation = 0; i_observation < Nobservations_board;
       i_observation++) {
    int32_t iframe =
        indices_frame_camintrinsics_camextrinsics.at(i_observation).x;
    int32_t icam_intrinsics =
        indices_frame_camintrinsics_camextrinsics.at(i_observation).y;
    int32_t icam_extrinsics =
        indices_frame_camintrinsics_camextrinsics.at(i_observation).z;

    c_observations_board[i_observation].icam.intrinsics = icam_intrinsics;
    c_observations_board[i_observation].icam.extrinsics = icam_extrinsics;
    c_observations_board[i_observation].iframe = iframe;
  }
  for (int i_observation = 0; i_observation < Nobservations_point;
       i_observation++) {
    int32_t i_point =
        indices_frame_camintrinsics_camextrinsics.at(i_observation).x;
    int32_t icam_intrinsics =
        indices_frame_camintrinsics_camextrinsics.at(i_observation).y;
    int32_t icam_extrinsics =
        indices_frame_camintrinsics_camextrinsics.at(i_observation).z;

    c_observations_point[i_observation].icam.intrinsics = icam_intrinsics;
    c_observations_point[i_observation].icam.extrinsics = icam_extrinsics;
    c_observations_point[i_observation].i_point = i_point;
  }

  int Ncameras_extrinsics = 0; // Seems to always be zero for single camera
  int Nframes =
      frames_rt_toref.size(); // Number of pictures of the object we've got
  mrcal_observation_point_triangulated_t *observations_point_triangulated = NULL;
  //     NULL;

  if (!lensmodel_one_validate_args(&mrcal_lensmodel, intrinsics, false)) {
    auto ret = std::make_unique<mrcal_result>();
    ret->success = false;
    return ret;
  }

  int Nstate = mrcal_num_states(
      Ncameras_intrinsics, Ncameras_extrinsics, Nframes, Npoints, Npoints_fixed,
      Nobservations_board, problem_selections, &mrcal_lensmodel);

  int Nmeasurements = mrcal_num_measurements(
      Nobservations_board, Nobservations_point,
      observations_point_triangulated,
      0, // hard-coded to 0
      calibration_object_width_n, calibration_object_height_n,
      Ncameras_intrinsics, Ncameras_extrinsics, Nframes, Npoints, Npoints_fixed,
      problem_selections, &mrcal_lensmodel);

  // OK, now we should have everything ready! Just some final setup and then
  // call optimize

  // Residuals
  std::vector<double> b_packed_final(Nstate);
  auto c_b_packed_final = b_packed_final.data();

  std::vector<double> x_final(Nmeasurements);
  auto c_x_final = x_final.data();

  // Seeds
  double *c_intrinsics = intrinsics.data();
  mrcal_pose_t *c_extrinsics = extrinsics_rt_fromref;
  mrcal_pose_t *c_frames = frames_rt_toref.data();
  mrcal_point3_t *c_points = points;
  mrcal_calobject_warp_t *c_calobject_warp = &calobject_warp;

  // in
  int *c_imagersizes = imagersize;
  auto point_min_range = -1.0, point_max_range = -1.0;
  mrcal_problem_constants_t problem_constants = {
      .point_min_range = point_min_range, .point_max_range = point_max_range};
  int verbose = 0;

  auto stats = mrcal_optimize(
      NULL, -1, c_x_final, Nmeasurements * sizeof(double), c_intrinsics,
      c_extrinsics, c_frames, c_points, c_calobject_warp, Ncameras_intrinsics,
      Ncameras_extrinsics, Nframes, Npoints, Npoints_fixed,
      c_observations_board, c_observations_point, Nobservations_board,
      Nobservations_point,
      observations_point_triangulated, -1,
      c_observations_board_pool, c_observations_point_pool, &mrcal_lensmodel, c_imagersizes,
      problem_selections, &problem_constants, calibration_object_spacing,
      calibration_object_width_n, calibration_object_height_n, verbose, false);

  std::vector<double> residuals = {c_x_final, c_x_final + Nmeasurements};
  return std::make_unique<mrcal_result>(
      true, intrinsics, stats.rms_reproj_error__pixels, residuals,
      calobject_warp, stats.Noutliers_board);
}

struct MrcalSolveOptions {
  // If true, we solve for the intrinsics core. Applies only to those models
  // that HAVE a core (fx,fy,cx,cy)
  int do_optimize_intrinsics_core;

  // If true, solve for the non-core lens parameters
  int do_optimize_intrinsics_distortions;

  // If true, solve for the geometry of the cameras
  int do_optimize_extrinsics;

  // If true, solve for the poses of the calibration object
  int do_optimize_frames;

  // If true, optimize the shape of the calibration object
  int do_optimize_calobject_warp;

  // If true, apply the regularization terms in the solver
  int do_apply_regularization;

  // Whether to try to find NEW outliers. The outliers given on
  // input are respected regardless
  int do_apply_outlier_rejection;
};

// lifted from mrcal-pywrap.c. Restrict a given selection to only valid options
// License for mrcal-pywrap.c:
// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
static mrcal_problem_selections_t
construct_problem_selections(MrcalSolveOptions s, int Ncameras_intrinsics,
                             int Ncameras_extrinsics, int Nframes,
                             int Nobservations_board) {
  // By default we optimize everything we can
  if (s.do_optimize_intrinsics_core < 0)
    s.do_optimize_intrinsics_core = Ncameras_intrinsics > 0;
  if (s.do_optimize_intrinsics_distortions < 0)
    s.do_optimize_intrinsics_core = Ncameras_intrinsics > 0;
  if (s.do_optimize_extrinsics < 0)
    s.do_optimize_extrinsics = Ncameras_extrinsics > 0;
  if (s.do_optimize_frames < 0)
    s.do_optimize_frames = Nframes > 0;
  if (s.do_optimize_calobject_warp < 0)
    s.do_optimize_calobject_warp = Nobservations_board > 0;
  return {
      .do_optimize_intrinsics_core =
          static_cast<bool>(s.do_optimize_intrinsics_core),
      .do_optimize_intrinsics_distortions =
          static_cast<bool>(s.do_optimize_intrinsics_distortions),
      .do_optimize_extrinsics = static_cast<bool>(s.do_optimize_extrinsics),
      .do_optimize_frames = static_cast<bool>(s.do_optimize_frames),
      .do_optimize_calobject_warp =
          static_cast<bool>(s.do_optimize_calobject_warp),
      .do_apply_regularization = static_cast<bool>(s.do_apply_regularization),
      .do_apply_outlier_rejection =
          static_cast<bool>(s.do_apply_outlier_rejection),
      // .do_apply_regularization_unity_cam01 = false
  };
}

bool lensmodel_one_validate_args(mrcal_lensmodel_t *mrcal_lensmodel,
                                 std::vector<double> intrinsics,
                                 bool do_check_layout) {
  int NlensParams = mrcal_lensmodel_num_params(mrcal_lensmodel);
  int NlensParams_have = intrinsics.size();
  if (NlensParams != NlensParams_have) {
    BARF("intrinsics.shape[-1] MUST be %d. Instead got %d", NlensParams,
         NlensParams_have);
    return false;
  }

  return true;
}

mrcal_pose_t getSeedPose(const mrcal_point3_t *c_observations_board_pool,
                         Size boardSize, Size imagerSize, double squareSize,
                         double focal_len_guess) {
  using std::vector, std::runtime_error;

  if (!c_observations_board_pool) {
    throw runtime_error("board was null");
  }

  double fx = focal_len_guess;
  double fy = fx;
  double cx = (imagerSize.width / 2.0) - 0.5;
  double cy = (imagerSize.height / 2.0) - 0.5;

  vector<Point3f> objectPoints;
  vector<Point2d> imagePoints;

  // Fill in object/image points
  for (int i = 0; i < boardSize.height; i++) {
    for (int j = 0; j < boardSize.width; j++) {
      auto &corner = c_observations_board_pool[i * boardSize.width + j];
      // weight<0 means ignored -- filter these out
      if (corner.z >= 0) {
        imagePoints.emplace_back(corner.x, corner.y);
        objectPoints.push_back(Point3f(j * squareSize, i * squareSize, 0));
      }
    }
  }

  {
    // convert from stereographic to pinhole to match python
    std::vector<mrcal_point2_t> mrcal_imagepts(imagePoints.size());
    std::transform(
        imagePoints.begin(), imagePoints.end(), mrcal_imagepts.begin(),
        [](const auto &pt) { return mrcal_point2_t{.x = pt.x, .y = pt.y}; });

    mrcal_lensmodel_t model{.type = MRCAL_LENSMODEL_STEREOGRAPHIC};
    std::vector<mrcal_point3_t> out(imagePoints.size());
    const double intrinsics[] = {fx, fy, cx, cy};
    bool ret = mrcal_unproject(out.data(), mrcal_imagepts.data(),
                               mrcal_imagepts.size(), &model, intrinsics);
    if (!ret) {
      std::cerr << "couldn't unproject!" << std::endl;
    }
    model = {.type = MRCAL_LENSMODEL_PINHOLE};
    mrcal_project(mrcal_imagepts.data(), NULL, NULL, out.data(), out.size(),
                  &model, intrinsics);

    std::transform(mrcal_imagepts.begin(), mrcal_imagepts.end(),
                   imagePoints.begin(),
                   [](const auto &pt) { return Point2d{pt.x, pt.y}; });
  }

  // Initial guess at intrinsics
  Mat cameraMatrix = (Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
  Mat distCoeffs = Mat(4, 1, CV_64FC1, Scalar(0));

  Mat_<double> rvec, tvec;
  vector<Point3f> objectPoints3;
  for (auto a : objectPoints)
    objectPoints3.push_back(Point3f(a.x, a.y, 0));

  // for (auto& o : objectPoints) std::cout << o << std::endl;
  // for (auto& i : imagePoints) std::cout << i << std::endl;
  // std::cout << "cam mat\n" << cameraMatrix << std::endl;
  // std::cout << "distortion: " << distCoeffs << std::endl;

  solvePnP(objectPoints3, imagePoints, cameraMatrix, distCoeffs, rvec, tvec,
           false, SOLVEPNP_ITERATIVE);

  return mrcal_pose_t{.r = {.x = rvec(0), .y = rvec(1), .z = rvec(2)},
                      .t = {.x = tvec(0), .y = tvec(1), .z = tvec(2)}};
}

mrcal_result::~mrcal_result() {
  // cholmod_l_free_sparse(&Jt, cctx.cc);
  return;
}

// Code taken from mrcal, license:
// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
std::unique_ptr<mrcal_result> mrcal_main(
    // List, depth is ordered array observation[N frames, object_height,
    // object_width] = [x,y, weight] weight<0 means ignored)
    std::span<mrcal_point3_t> observations_board,
    // RT transform from camera to object
    std::span<mrcal_pose_t> frames_rt_toref,
    // Chessboard size, in corners (not squares)
    Size calobjectSize, double calibration_object_spacing,
    // res, pixels
    Size cameraRes, double focal_length_guess) {

  std::unique_ptr<mrcal_result> result;

  {
    // stereographic initial guess for intrinsics
    double cx = (cameraRes.width / 2.0) - 0.5;
    double cy = (cameraRes.height / 2.0) - 0.5;
    std::vector<double> intrinsics = {focal_length_guess, focal_length_guess,
                                      cx, cy};

    std::cout << "Initial solve (geometry only)" << std::endl;

    mrcal_problem_selections_t options = construct_problem_selections(
        {.do_optimize_intrinsics_core = false,
         .do_optimize_intrinsics_distortions = false,
         .do_optimize_extrinsics = false,
         .do_optimize_frames = true,
         .do_optimize_calobject_warp = false,
         .do_apply_regularization = false,
         .do_apply_outlier_rejection = false},
        1, 0, frames_rt_toref.size(), frames_rt_toref.size());

    mrcal_lensmodel_t mrcal_lensmodel;
    mrcal_lensmodel.type = MRCAL_LENSMODEL_STEREOGRAPHIC;

    // And run calibration. This should mutate frames_rt_toref in place
    result = mrcal_calibrate(observations_board, frames_rt_toref, calobjectSize,
                             calibration_object_spacing, cameraRes, options,
                             mrcal_lensmodel, intrinsics);
  }

  {
    std::cout
        << "Initial solve (geometry and LENSMODEL_STEREOGRAPHIC core only)"
        << std::endl;
    mrcal_problem_selections_t options = construct_problem_selections(
        {.do_optimize_intrinsics_core = true,
         .do_optimize_intrinsics_distortions = true,
         .do_optimize_extrinsics = false,
         .do_optimize_frames = true,
         .do_optimize_calobject_warp = false,
         .do_apply_regularization = false,
         .do_apply_outlier_rejection = false},
        1, 0, frames_rt_toref.size(), frames_rt_toref.size());

    mrcal_lensmodel_t mrcal_lensmodel;
    mrcal_lensmodel.type = MRCAL_LENSMODEL_STEREOGRAPHIC;

    result = mrcal_calibrate(observations_board, frames_rt_toref, calobjectSize,
                             calibration_object_spacing, cameraRes, options,
                             mrcal_lensmodel, result->intrinsics);
  }

  {
    // Now we've got a seed, expand intrinsics to our target model
    // see
    // https://github.com/dkogan/mrcal/blob/33c3c50d5b7f991aca3a8e71ca52c5fffd153ef2/mrcal-calibrate-cameras#L533
    mrcal_lensmodel_t mrcal_lensmodel;
    mrcal_lensmodel.type = MRCAL_LENSMODEL_OPENCV8;
    // num distortion params
    int nparams = mrcal_lensmodel_num_params(&mrcal_lensmodel);
    std::vector<double> intrinsics(nparams);

    // copy core in
    std::copy(result->intrinsics.begin(), result->intrinsics.end(),
              intrinsics.begin());

    // and generate random distortion
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.5, 0.5);

    int nDistortion = nparams - 4;
    std::vector<double> seedDistortions(nDistortion);

    for (int j = 0; j < seedDistortions.size(); j++) {
      if (j < 5)
        seedDistortions[j] = dis(gen) * 2.0 * 1e-6;
      else
        seedDistortions[j] = dis(gen) * 2.0 * 1e-9;
    }

    // copy distortion into our big intrinsics array
    std::copy(seedDistortions.begin(), seedDistortions.end(),
              intrinsics.begin() + result->intrinsics.size());

    std::cout
        << "Optimizing everything except board warp from seeded intrinsics"
        << std::endl;
    mrcal_problem_selections_t options = construct_problem_selections(
        {.do_optimize_intrinsics_core = true,
         .do_optimize_intrinsics_distortions = true,
         .do_optimize_extrinsics = true,
         .do_optimize_frames = true,
         .do_optimize_calobject_warp = false,
         .do_apply_regularization = true,
         .do_apply_outlier_rejection = true},
        1, 0, frames_rt_toref.size(), frames_rt_toref.size());

    result = mrcal_calibrate(observations_board, frames_rt_toref, calobjectSize,
                             calibration_object_spacing, cameraRes, options,
                             mrcal_lensmodel, intrinsics);
  }

  {
    std::cout << "Final, full solve" << std::endl;
    mrcal_problem_selections_t options = construct_problem_selections(
        {.do_optimize_intrinsics_core = true,
         .do_optimize_intrinsics_distortions = true,
         .do_optimize_extrinsics = true,
         .do_optimize_frames = true,
         .do_optimize_calobject_warp = true,
         .do_apply_regularization = true,
         .do_apply_outlier_rejection = true},
        1, 0, frames_rt_toref.size(), frames_rt_toref.size());

    mrcal_lensmodel_t mrcal_lensmodel;
    mrcal_lensmodel.type = MRCAL_LENSMODEL_OPENCV8;

    result = mrcal_calibrate(observations_board, frames_rt_toref, calobjectSize,
                             calibration_object_spacing, cameraRes, options,
                             mrcal_lensmodel, result->intrinsics);
  }

  return result;
}

bool undistort_mrcal(const cv::Mat *src, cv::Mat *dst, const cv::Mat *cameraMat,
                     const cv::Mat *distCoeffs, CameraLensModel lensModel,
                     // Extra stuff for splined stereographic models
                     uint16_t order, uint16_t Nx, uint16_t Ny,
                     uint16_t fov_x_deg) {
  mrcal_lensmodel_t mrcal_lensmodel;
  switch (lensModel) {
  case CameraLensModel::LENSMODEL_OPENCV5:
    mrcal_lensmodel.type = MRCAL_LENSMODEL_OPENCV5;
    break;
  case CameraLensModel::LENSMODEL_OPENCV8:
    mrcal_lensmodel.type = MRCAL_LENSMODEL_OPENCV8;
    break;
  case CameraLensModel::LENSMODEL_STEREOGRAPHIC:
    mrcal_lensmodel.type = MRCAL_LENSMODEL_STEREOGRAPHIC;
    break;
  case CameraLensModel::LENSMODEL_SPLINED_STEREOGRAPHIC:
    mrcal_lensmodel.type = MRCAL_LENSMODEL_SPLINED_STEREOGRAPHIC;

    /* Maximum degree of each 1D polynomial. This is almost certainly 2 */
    /* (quadratic splines, C1 continuous) or 3 (cubic splines, C2 continuous) */
    mrcal_lensmodel.LENSMODEL_SPLINED_STEREOGRAPHIC__config.order = order;
    /* The horizontal field of view. Not including fov_y. It's proportional with
     * Ny and Nx */
    mrcal_lensmodel.LENSMODEL_SPLINED_STEREOGRAPHIC__config.fov_x_deg =
        fov_x_deg;
    /* We have a Nx by Ny grid of control points */
    mrcal_lensmodel.LENSMODEL_SPLINED_STEREOGRAPHIC__config.Nx = Nx;
    mrcal_lensmodel.LENSMODEL_SPLINED_STEREOGRAPHIC__config.Ny = Ny;
    break;
  default:
    std::cerr << "Unknown lensmodel\n";
    return false;
  }

  if (!(dst->cols == 2 && dst->cols == 2)) {
    std::cerr << "Bad input array size\n";
    return false;
  }
  if (!(dst->type() == CV_64FC2 && dst->type() == CV_64FC2)) {
    std::cerr << "Bad input type -- need CV_64F\n";
    return false;
  }
  if (!(dst->isContinuous() && dst->isContinuous())) {
    std::cerr << "Bad input array -- need continuous\n";
    return false;
  }

  // extract intrinsics core from opencv camera matrix
  double fx = cameraMat->at<double>(0, 0);
  double fy = cameraMat->at<double>(1, 1);
  double cx = cameraMat->at<double>(0, 2);
  double cy = cameraMat->at<double>(1, 2);

  // Core, distortion coefficients concatenated
  int NlensParams = mrcal_lensmodel_num_params(&mrcal_lensmodel);
  std::vector<double> intrinsics(NlensParams);
  intrinsics[0] = fx;
  intrinsics[1] = fy;
  intrinsics[2] = cx;
  intrinsics[3] = cy;
  for (size_t i = 0; i < distCoeffs->cols; i++) {
    intrinsics[i + 4] = distCoeffs->at<double>(i);
  }

  // input points in the distorted image pixel coordinates
  mrcal_point2_t *in = reinterpret_cast<mrcal_point2_t *>(dst->data);
  // vec3 observation vectors defined up-to-length
  std::vector<mrcal_point3_t> out(dst->rows);

  mrcal_unproject(out.data(), in, dst->rows, &mrcal_lensmodel,
                  intrinsics.data());

  // The output is defined "up-to-length"
  // Let's project through pinhole again

  // Output points in pinhole pixel coordinates
  mrcal_point2_t *pinhole_pts = reinterpret_cast<mrcal_point2_t *>(dst->data);

  size_t bound = dst->rows;
  for (size_t i = 0; i < bound; i++) {
    // from mrcal-project-internal/pinhole model
    mrcal_point3_t &p = out[i];

    double z_recip = 1. / p.z;
    double x = p.x * z_recip;
    double y = p.y * z_recip;

    pinhole_pts[i].x = x * fx + cx;
    pinhole_pts[i].y = y * fy + cy;
  }

  return true;
}
