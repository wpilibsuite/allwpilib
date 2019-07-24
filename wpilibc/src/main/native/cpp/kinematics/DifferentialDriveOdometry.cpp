/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/kinematics/DifferentialDriveOdometry.h"

using namespace frc;

DifferentialDriveOdometry::DifferentialDriveOdometry(
    DifferentialDriveKinematics kinematics, const Pose2d& initialPose)
    : m_kinematics(kinematics), m_pose(initialPose) {
  m_previousAngle = m_pose.Rotation();
}

const Pose2d& DifferentialDriveOdometry::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& angle,
    const DifferentialDriveWheelSpeeds& wheelSpeeds) {
  units::second_t deltaTime =
      (m_previousTime >= 0_s) ? currentTime - m_previousTime : 0_s;
  m_previousTime = currentTime;

  auto [dx, dy, dtheta] = m_kinematics.ToChassisSpeeds(wheelSpeeds);
  static_cast<void>(dtheta);

  auto newPose = m_pose.Exp(
      {dx * deltaTime, dy * deltaTime, (angle - m_previousAngle).Radians()});

  m_previousAngle = angle;
  m_pose = {newPose.Translation(), angle};

  return m_pose;
}
