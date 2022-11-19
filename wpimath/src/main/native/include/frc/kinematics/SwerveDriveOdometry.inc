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
    : m_kinematics(kinematics),
      m_pose(initialPose),
      m_previousModulePositions(modulePositions) {
  m_previousAngle = m_pose.Rotation();
  m_gyroOffset = m_pose.Rotation() - gyroAngle;
  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kOdometry_SwerveDrive, 1);
}

template <size_t NumModules>
void SwerveDriveOdometry<NumModules>::ResetPosition(
    const Rotation2d& gyroAngle,
    const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
    const Pose2d& pose) {
  m_pose = pose;
  m_previousAngle = pose.Rotation();
  m_gyroOffset = m_pose.Rotation() - gyroAngle;
  m_previousModulePositions = modulePositions;
}

template <size_t NumModules>
const Pose2d& frc::SwerveDriveOdometry<NumModules>::Update(
    const Rotation2d& gyroAngle,
    const wpi::array<SwerveModulePosition, NumModules>& modulePositions) {
  auto moduleDeltas =
      wpi::array<SwerveModulePosition, NumModules>(wpi::empty_array);
  for (size_t index = 0; index < modulePositions.size(); index++) {
    auto lastPosition = m_previousModulePositions[index];
    auto currentPosition = modulePositions[index];
    moduleDeltas[index] = {currentPosition.distance - lastPosition.distance,
                           currentPosition.angle};
  }

  auto angle = gyroAngle + m_gyroOffset;

  auto twist = m_kinematics.ToTwist2d(moduleDeltas);
  twist.dtheta = (angle - m_previousAngle).Radians();

  auto newPose = m_pose.Exp(twist);

  m_previousAngle = angle;
  m_pose = {newPose.Translation(), angle};
  m_previousModulePositions = modulePositions;

  return m_pose;
}
}  // namespace frc
