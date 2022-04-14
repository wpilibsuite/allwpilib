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
    const Pose2d& initialPose)
    : m_kinematics(kinematics), m_pose(initialPose) {
  m_previousAngle = m_pose.Rotation();
  m_gyroOffset = m_pose.Rotation() - gyroAngle;
  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kOdometry_SwerveDrive, 1);
}

template <size_t NumModules>
template <typename... ModuleStates>
const Pose2d& frc::SwerveDriveOdometry<NumModules>::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    ModuleStates&&... moduleStates) {
  units::second_t deltaTime =
      (m_previousTime >= 0_s) ? currentTime - m_previousTime : 0_s;
  m_previousTime = currentTime;

  auto angle = gyroAngle + m_gyroOffset;

  auto [dx, dy, dtheta] = m_kinematics.ToChassisSpeeds(moduleStates...);
  static_cast<void>(dtheta);

  auto newPose = m_pose.Exp(
      {dx * deltaTime, dy * deltaTime, (angle - m_previousAngle).Radians()});

  m_previousAngle = angle;
  m_pose = {newPose.Translation(), angle};

  return m_pose;
}
}  // namespace frc
