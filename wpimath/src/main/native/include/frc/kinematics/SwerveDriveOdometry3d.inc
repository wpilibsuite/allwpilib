// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/kinematics/SwerveDriveOdometry3d.h"
#include "wpimath/MathShared.h"

namespace frc {
template <size_t NumModules>
SwerveDriveOdometry3d<NumModules>::SwerveDriveOdometry3d(
    SwerveDriveKinematics<NumModules> kinematics, const Rotation2d& gyroAngle,
    const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
    const Pose2d& initialPose)
    : SwerveDriveOdometry3d::Odometry3d(m_kinematicsImpl, gyroAngle,
                                        modulePositions, initialPose),
      m_kinematicsImpl(kinematics) {
  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kOdometry_SwerveDrive, 1);
}

template <size_t NumModules>
SwerveDriveOdometry3d<NumModules>::SwerveDriveOdometry3d(
    SwerveDriveKinematics<NumModules> kinematics, const Rotation3d& gyroAngle,
    const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
    const Pose3d& initialPose)
    : SwerveDriveOdometry3d::Odometry3d(m_kinematicsImpl, gyroAngle,
                                        modulePositions, initialPose),
      m_kinematicsImpl(kinematics) {
  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kOdometry_SwerveDrive, 1);
}
}  // namespace frc
