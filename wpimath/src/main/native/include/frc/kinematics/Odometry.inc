// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/kinematics/Odometry.h"

namespace frc {
template <typename WheelSpeeds, WheelPositions WheelPositions>
Odometry<WheelSpeeds, WheelPositions>::Odometry(
    const Kinematics<WheelSpeeds, WheelPositions>& kinematics,
    const Rotation2d& gyroAngle, const WheelPositions& wheelPositions,
    const Pose2d& initialPose)
    : m_kinematics(kinematics),
      m_pose(initialPose),
      m_previousWheelPositions(wheelPositions) {
  m_previousAngle = m_pose.Rotation();
  m_gyroOffset = m_pose.Rotation() - gyroAngle;
}

template <typename WheelSpeeds, WheelPositions WheelPositions>
const Pose2d& Odometry<WheelSpeeds, WheelPositions>::Update(
    const Rotation2d& gyroAngle, const WheelPositions& wheelPositions) {
  auto angle = gyroAngle + m_gyroOffset;

  auto twist = m_kinematics.ToTwist2d(m_previousWheelPositions, wheelPositions);
  twist.dtheta = (angle - m_previousAngle).Radians();

  auto newPose = m_pose.Exp(twist);

  m_previousAngle = angle;
  m_previousWheelPositions = wheelPositions;
  m_pose = {newPose.Translation(), angle};

  return m_pose;
}
}  // namespace frc
