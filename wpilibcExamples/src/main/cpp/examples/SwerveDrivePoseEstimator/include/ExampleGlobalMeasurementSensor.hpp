// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/math/geometry/Pose2d.hpp"

/**
 * This dummy class represents a global measurement sensor, such as a computer
 * vision solution.
 */
class ExampleGlobalMeasurementSensor {
 public:
  static frc::Pose2d GetEstimatedGlobalPose(
      const frc::Pose2d& estimatedRobotPose) {
    auto randVec = frc::MakeWhiteNoiseVector(0.1, 0.1, 0.1);
    return frc::Pose2d{estimatedRobotPose.X() + units::meter_t{randVec(0)},
                       estimatedRobotPose.Y() + units::meter_t{randVec(1)},
                       estimatedRobotPose.Rotation() +
                           frc::Rotation2d{units::radian_t{randVec(2)}}};
  }
};
