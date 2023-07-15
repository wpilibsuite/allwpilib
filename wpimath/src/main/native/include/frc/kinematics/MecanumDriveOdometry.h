// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/timestamp.h>

#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/MecanumDriveKinematics.h"
#include "frc/kinematics/MecanumDriveWheelPositions.h"
#include "frc/kinematics/MecanumDriveWheelSpeeds.h"
#include "frc/kinematics/Odometry.h"
#include "units/time.h"

namespace frc {

/**
 * Class for mecanum drive odometry. Odometry allows you to track the robot's
 * position on the field over a course of a match using readings from your
 * mecanum wheel encoders.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
class WPILIB_DLLEXPORT MecanumDriveOdometry
    : public Odometry<MecanumDriveWheelSpeeds, MecanumDriveWheelPositions> {
 public:
  /**
   * Constructs a MecanumDriveOdometry object.
   *
   * @param kinematics The mecanum drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current distances measured by each wheel.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit MecanumDriveOdometry(
      MecanumDriveKinematics kinematics, const Rotation2d& gyroAngle,
      const MecanumDriveWheelPositions& wheelPositions,
      const Pose2d& initialPose = Pose2d{});

 private:
  MecanumDriveKinematics m_kinematicsImpl;
};

}  // namespace frc
