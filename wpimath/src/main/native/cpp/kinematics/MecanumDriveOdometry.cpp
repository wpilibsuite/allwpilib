// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/MecanumDriveOdometry.hpp"

#include "wpi/math/util/MathShared.hpp"

using namespace wpi::math;

MecanumDriveOdometry::MecanumDriveOdometry(
    MecanumDriveKinematics kinematics, const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose2d& initialPose)
    : Odometry(gyroAngle, initialPose),
      m_kinematics(kinematics),
      m_previousWheelPositions(wheelPositions) {
  wpi::math::MathSharedStore::ReportUsage("MecanumDriveOdometry", "");
}
