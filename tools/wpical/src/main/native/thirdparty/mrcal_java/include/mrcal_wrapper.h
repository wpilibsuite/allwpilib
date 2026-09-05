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

#include <mrcal.h>

#include <atomic>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <span>
#include <utility>
#include <vector>

class mrcal_cancellation_token {
 public:
  mrcal_cancellation_token() = default;

  bool stop_requested() const noexcept {
    return m_state && m_state->load(std::memory_order_relaxed);
  }

 private:
  explicit mrcal_cancellation_token(
      std::shared_ptr<std::atomic_bool> state)
      : m_state{std::move(state)} {}

  std::shared_ptr<std::atomic_bool> m_state;

  friend class mrcal_cancellation_source;
};

class mrcal_cancellation_source {
 public:
  mrcal_cancellation_source()
      : m_state{std::make_shared<std::atomic_bool>(false)} {}

  mrcal_cancellation_token get_token() const noexcept {
    return mrcal_cancellation_token{m_state};
  }

  bool request_stop() noexcept {
    return !m_state->exchange(true, std::memory_order_relaxed);
  }

 private:
  std::shared_ptr<std::atomic_bool> m_state;
};

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
mrcal_pose_t getSeedPose(const mrcal_point3_t *c_observations_board_pool,
                         cv::Size boardSize, cv::Size imagerSize,
                         double squareSize, double focal_len_guess);

std::unique_ptr<mrcal_result> mrcal_main(
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
    double focal_len_guess,
    mrcal_cancellation_token stopToken = {});

enum class CameraLensModel {
  LENSMODEL_OPENCV5 = 0,
  LENSMODEL_OPENCV8,
  LENSMODEL_STEREOGRAPHIC,
  LENSMODEL_SPLINED_STEREOGRAPHIC
};

bool undistort_mrcal(cv::Mat *dst, const cv::Mat *cameraMat,
                     const cv::Mat *distCoeffs, CameraLensModel lensModel,
                     // Extra stuff for splined stereographic models
                     uint16_t order, uint16_t Nx, uint16_t Ny,
                     uint16_t fov_x_deg);
