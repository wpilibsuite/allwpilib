// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/MecanumDriveOdometry3d.hpp"

#include "wpi/math/util/MathShared.hpp"

using namespace wpi::math;

MecanumDriveOdometry3d::MecanumDriveOdometry3d(
    MecanumDriveKinematics kinematics, const Rotation3d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose3d& initialPose)
    : Odometry3d(gyroAngle, initialPose),
      m_kinematics(kinematics),
      m_previousWheelPositions(wheelPositions) {
  wpi::math::MathSharedStore::ReportUsage("MecanumDriveOdometry3d", "");
}
