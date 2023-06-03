// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/kinematics/Odometry.h"

namespace frc {
template <WheelPositions T>
Odometry<T>::Odometry(const Kinematics<T>& kinematics, const Rotation2d& gyroAngle, const T& wheelPositions, const Pose2d& initialPose)
    : m_kinematics(kinematics),
      m_pose(initialPose),
      m_previousWheelPositions(wheelPositions) {
  m_previousAngle = m_pose.Rotation();
  m_gyroOffset = m_pose.Rotation() - gyroAngle;
}

template <WheelPositions T>
const Pose2d& Odometry<T>::Update(
    const Rotation2d& gyroAngle,
    const T& wheelPositions) {
  auto angle = gyroAngle + m_gyroOffset;

  auto wheelDeltas = wheelPositions - m_previousWheelPositions;

  auto twist = m_kinematics.ToTwist2d(wheelDeltas);
  twist.dtheta = (angle - m_previousAngle).Radians();

  auto newPose = m_pose.Exp(twist);

  m_previousAngle = angle;
  m_previousWheelPositions = wheelPositions;
  m_pose = {newPose.Translation(), angle};

  return m_pose;
}
}  // namespace frc
