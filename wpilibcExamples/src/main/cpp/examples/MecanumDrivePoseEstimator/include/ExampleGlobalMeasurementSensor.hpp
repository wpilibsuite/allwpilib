// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/random/Normal.hpp"

/**
 * This dummy class represents a global measurement sensor, such as a computer
 * vision solution.
 */
class ExampleGlobalMeasurementSensor {
 public:
  static wpi::math::Pose2d GetEstimatedGlobalPose(
      const wpi::math::Pose2d& estimatedRobotPose) {
    auto randVec = wpi::math::Normal(0.1, 0.1, 0.1);
    return wpi::math::Pose2d{
        estimatedRobotPose.X() + wpi::units::meter_t{randVec(0)},
        estimatedRobotPose.Y() + wpi::units::meter_t{randVec(1)},
        estimatedRobotPose.Rotation() +
            wpi::math::Rotation2d{wpi::units::radian_t{randVec(2)}}};
  }
};
