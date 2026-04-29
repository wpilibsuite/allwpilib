// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelPositions.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelVelocities.hpp"
#include "wpi/math/kinematics/Odometry3d.hpp"
#include "wpi/util/SymbolExports.hpp"

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
    : public Odometry3d<MecanumDriveWheelPositions> {
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

  void ResetPosition(const Rotation3d& gyroAngle,
                     const MecanumDriveWheelPositions& wheelPositions,
                     const Pose3d& pose) override {
    m_previousWheelPositions = wheelPositions;
    Odometry3d::ResetPosition(gyroAngle, pose);
  }

  const Pose3d& Update(
      const Rotation3d& gyroAngle,
      const MecanumDriveWheelPositions& wheelPositions) override {
    auto twist =
        m_kinematics.ToTwist2d(m_previousWheelPositions, wheelPositions);
    m_previousWheelPositions = wheelPositions;
    return Odometry3d::Update(gyroAngle, twist);
  }

 private:
  MecanumDriveKinematics m_kinematics;

  MecanumDriveWheelPositions m_previousWheelPositions;
};

}  // namespace wpi::math
