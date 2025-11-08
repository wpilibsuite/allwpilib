// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveOdometry3d.hpp"

#include "wpi/math/util/MathShared.hpp"

using namespace wpi::math;

DifferentialDriveOdometry3d::DifferentialDriveOdometry3d(
    const Rotation3d& gyroAngle, wpi::units::meter_t leftDistance,
    wpi::units::meter_t rightDistance, const Pose3d& initialPose)
    : Odometry3d(m_kinematicsImpl, gyroAngle, {leftDistance, rightDistance},
                 initialPose) {
  wpi::math::MathSharedStore::ReportUsage("DifferentialDriveOdometry3d", "");
}
