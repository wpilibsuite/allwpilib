// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <opencv2/core/mat.hpp>

#include "frc/apriltag/AprilTagDetector.h"

namespace frc {

inline AprilTagDetector::Results AprilTagDetect(AprilTagDetector& detector,
                                                cv::Mat& image) {
  return detector.Detect(image.cols, image.rows, image.data);
}

}  // namespace frc
