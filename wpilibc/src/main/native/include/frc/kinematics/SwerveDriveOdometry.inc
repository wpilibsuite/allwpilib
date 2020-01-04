/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/FRCUsageReporting.h>

namespace frc {
template <size_t NumModules>
SwerveDriveOdometry<NumModules>::SwerveDriveOdometry(
    SwerveDriveKinematics<NumModules> kinematics, const Rotation2d& gyroAngle,
    const Pose2d& initialPose)
    : m_kinematics(kinematics), m_pose(initialPose) {
  m_previousAngle = m_pose.Rotation();
  m_gyroOffset = m_pose.Rotation() - gyroAngle;
  HAL_Report(HALUsageReporting::kResourceType_Odometry,
             HALUsageReporting::kOdometry_SwerveDrive);
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
