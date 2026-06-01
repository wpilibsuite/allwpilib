// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <gtsam/geometry/Pose3.h>
#include <opencv2/core/mat.hpp>

#include "wpi/math/geometry/Pose3d.hpp"

namespace wpical {
gtsam::Pose3 Pose3dToGtsamPose3(wpi::math::Pose3d pose);
wpi::math::Pose3d GtsamToFrcPose3d(gtsam::Pose3 pose);
wpi::math::Pose3d ToPose3d(const cv::Mat& tvec, const cv::Mat& rvec);
}  // namespace wpical
