/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/StateSpaceUtil.h>
#include <frc/geometry/Pose2d.h>

/**
 * This dummy class represents a global measurement sensor, such as a computer
 * vision solution.
 */
class ExampleGlobalMeasurementSensor {
 public:
  static frc::Pose2d GetEstimatedGlobalPose(frc::Pose2d estimatedRobotPose) {
    auto randVec = frc::MakeWhiteNoiseVector(0.1, 0.1, 0.1);
    return frc::Pose2d(estimatedRobotPose.X() + units::meter_t(randVec(0)),
                       estimatedRobotPose.Y() + units::meter_t(randVec(1)),
                       estimatedRobotPose.Rotation() +
                           frc::Rotation2d(units::radian_t(randVec(3))));
  }
};
