// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/apriltag/AprilTagPoseEstimate.hpp"

#include <algorithm>

using namespace wpi::apriltag;

double AprilTagPoseEstimate::GetAmbiguity() const {
  auto min = (std::min)(error1, error2);
  auto max = (std::max)(error1, error2);

  if (max > 0) {
    return min / max;
  } else {
    return -1;
  }
}
