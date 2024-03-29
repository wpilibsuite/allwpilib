// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/apriltag/AprilTagPoseEstimate.h"

#include <algorithm>

using namespace frc;

double AprilTagPoseEstimate::GetAmbiguity() const {
  auto min = (std::min)(error1, error2);
  auto max = (std::max)(error1, error2);

  if (max > 0) {
    return min / max;
  } else {
    return -1;
  }
}
