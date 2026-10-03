// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <span>
#include <utility>
#include <vector>

#include <mrcal.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <wpi/util/SymbolExports.hpp>

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
      : success{success_},
        intrinsics{std::move(intrinsics_)},
        rms_error{rms_error_},
        residuals{std::move(residuals_)},
        calobject_warp{calobject_warp_},
        Noutliers_board{Noutliers_board_} {}
  mrcal_result(mrcal_result&&) = delete;
  ~mrcal_result() = default;
};

/**
 * Gets the seed pose for a board.
 *
 * @param c_observations_board_pool The corners in image space.
 * @param boardSize The size of the corner grid.
 * @param imagerSize The size of the image in pixels.
 * @param squareSize The size of the squares in a physical distance unit.
 * @param focal_len_guess A focal length guess in pixels.
 */
WPILIB_DLLEXPORT mrcal_pose_t
getSeedPose(const mrcal_point3_t* c_observations_board_pool, cv::Size boardSize,
            cv::Size imagerSize, double squareSize, double focal_len_guess);

WPILIB_DLLEXPORT std::unique_ptr<mrcal_result> mrcal_main(
    // List, depth is ordered array observation[N frames, object_height,
    // object_width] = [x,y, weight] weight<0 means ignored)
    std::span<mrcal_point3_t> observations_board,
    // [out] RT transform from camera to object
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

WPILIB_DLLEXPORT bool undistort_mrcal(
    cv::Mat* dst, const cv::Mat* cameraMat, const cv::Mat* distCoeffs,
    CameraLensModel lensModel,
    // Extra stuff for splined stereographic models
    uint16_t order, uint16_t Nx, uint16_t Ny, uint16_t fov_x_deg);
