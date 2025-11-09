// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelPositions.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelSpeeds.hpp"
#include "wpi/math/kinematics/Odometry3d.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/timestamp.h"

namespace wpi::math {

/**
 * Class for mecanum drive odometry. Odometry allows you to track the robot's
 * position on the field over a course of a match using readings from your
 * mecanum wheel encoders.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
class WPILIB_DLLEXPORT MecanumDriveOdometry3d
    : public Odometry3d<MecanumDriveWheelSpeeds, MecanumDriveWheelPositions> {
 public:
  /**
   * Constructs a MecanumDriveOdometry3d object.
   *
   * @param kinematics The mecanum drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current distances measured by each wheel.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit MecanumDriveOdometry3d(
      MecanumDriveKinematics kinematics, const Rotation3d& gyroAngle,
      const MecanumDriveWheelPositions& wheelPositions,
      const Pose3d& initialPose = Pose3d{});

 private:
  MecanumDriveKinematics m_kinematicsImpl;
};

}  // namespace wpi::math
