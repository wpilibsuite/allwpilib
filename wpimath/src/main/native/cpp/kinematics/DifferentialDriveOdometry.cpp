// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/DifferentialDriveOdometry.h"

#include "wpimath/MathShared.h"

using namespace frc;

DifferentialDriveOdometry::DifferentialDriveOdometry(
    const Rotation2d& gyroAngle, units::meter_t leftDistance,
    units::meter_t rightDistance, const Pose2d& initialPose)
    : Odometry(m_kinematicsImpl, gyroAngle, {leftDistance, rightDistance},
               initialPose) {
  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kOdometry_DifferentialDrive, 1);
}
