// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>
#include <frc/geometry/Pose3d.h>

struct Pose3WithVariance {
  frc::Pose3d pose;
  // in order rx ry rz tx ty tz. Units are SI units
  std::array<double, 6> covariance;

  static Pose3WithVariance FromEigen(frc::Pose3d pose,
                                     Eigen::MatrixXd covariance) {
    std::array<double, 6> cov;

    for (size_t i = 0; i < 6; i++) {
      cov[i] = covariance(i);
    }

    return {pose, cov};
  }
};

#include "Pose3WithVarianceStruct.h"
