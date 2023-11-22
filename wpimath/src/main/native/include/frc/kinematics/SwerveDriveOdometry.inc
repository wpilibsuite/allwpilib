// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/kinematics/SwerveDriveOdometry.h"
#include "wpimath/MathShared.h"

namespace frc {
template <size_t NumModules>
SwerveDriveOdometry<NumModules>::SwerveDriveOdometry(
    SwerveDriveKinematics<NumModules> kinematics, const Rotation2d& gyroAngle,
    const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
    const Pose2d& initialPose)
    : Odometry<SwerveDriveWheelSpeeds<NumModules>,
               SwerveDriveWheelPositions<NumModules>>(
          m_kinematicsImpl, gyroAngle, {modulePositions}, initialPose),
      m_kinematicsImpl(kinematics) {
  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kOdometry_SwerveDrive, 1);
}
}  // namespace frc
