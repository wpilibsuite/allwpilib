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

#pragma once

extern "C" {
// Seems to be missing C++ guards
#include <mrcal.h>

} // extern "C"

// Seems like these people don't properly extern-c their headers either
extern "C" {
#include <suitesparse/SuiteSparse_config.h>
#include <suitesparse/cholmod_core.h>
} // extern "C"

#include <memory>
#include <opencv2/opencv.hpp>
#include <span>
#include <utility>
#include <vector>

struct mrcal_result {
  bool success;
  std::vector<double> intrinsics;
  double rms_error;
  std::vector<double> residuals;
  mrcal_calobject_warp_t calobject_warp;
  int Noutliers_board;
  // TODO standard devs

  mrcal_result() = default;
  mrcal_result(bool success_, std::vector<double> intrinsics_,
               double rms_error_, std::vector<double> residuals_,
               mrcal_calobject_warp_t calobject_warp_, int Noutliers_board_)
      : success{success_}, intrinsics{std::move(intrinsics_)},
        rms_error{rms_error_}, residuals{std::move(residuals_)},
        calobject_warp{calobject_warp_}, Noutliers_board{Noutliers_board_} {}
  mrcal_result(mrcal_result &&) = delete;
  ~mrcal_result();
};

mrcal_pose_t getSeedPose(const mrcal_point3_t *c_observations_board_pool,
                         cv::Size boardSize, cv::Size imagerSize,
                         double squareSize, double focal_len_guess);

std::unique_ptr<mrcal_result> mrcal_main(
    // List, depth is ordered array observation[N frames, object_height,
    // object_width] = [x,y, weight] weight<0 means ignored)
    std::span<mrcal_point3_t> observations_board,
    // RT transform from camera to object
    std::span<mrcal_pose_t> frames_rt_toref,
    // Chessboard size, in corners (not squares)
    cv::Size calobjectSize, double boardSpacing,
    // res, pixels
    cv::Size cameraRes,
    // focal length, in pixels
    double focal_len_guess);

enum class CameraLensModel {
  LENSMODEL_OPENCV5 = 0,
  LENSMODEL_OPENCV8,
  LENSMODEL_STEREOGRAPHIC,
  LENSMODEL_SPLINED_STEREOGRAPHIC
};

bool undistort_mrcal(const cv::Mat *src, cv::Mat *dst, const cv::Mat *cameraMat,
                     const cv::Mat *distCoeffs, CameraLensModel lensModel,
                     // Extra stuff for splined stereographic models
                     uint16_t order, uint16_t Nx, uint16_t Ny,
                     uint16_t fov_x_deg);
