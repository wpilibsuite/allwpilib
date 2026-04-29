// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveOdometry.hpp"

#include "wpi/math/util/MathShared.hpp"

using namespace wpi::math;

DifferentialDriveOdometry::DifferentialDriveOdometry(
    const Rotation2d& gyroAngle, wpi::units::meter_t leftDistance,
    wpi::units::meter_t rightDistance, const Pose2d& initialPose)
    : Odometry(gyroAngle, initialPose),
      m_previousWheelPositions{leftDistance, rightDistance} {
  wpi::math::MathSharedStore::ReportUsage("DifferentialDriveOdometry", "");
}
