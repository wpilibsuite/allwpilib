// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*
 * Copyright (C) Photon Vision.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
