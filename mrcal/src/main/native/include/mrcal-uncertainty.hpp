// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <ranges>
#include <span>
#include <vector>

#include <Eigen/Dense>
#include <mrcal.h>
#include <opencv2/core/types.hpp>
#include <poseutils.h>
#include <suitesparse/cholmod.h>

std::vector<mrcal_point3_t> compute_uncertainty(
    std::span<mrcal_point3_t> observations_board, std::span<double> intrinsics,
    std::span<mrcal_pose_t> rt_ref_frames, mrcal_calobject_warp_t warp,
    cv::Size imagerSize, cv::Size calobjectSize, double calobjectSpacing,
    cv::Size sampleResolution);
