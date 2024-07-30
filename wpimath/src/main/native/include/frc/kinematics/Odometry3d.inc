// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/kinematics/Odometry3d.h"

namespace frc {
template <typename WheelSpeeds, typename WheelPositions>
Odometry3d<WheelSpeeds, WheelPositions>::Odometry3d(
    const Kinematics<WheelSpeeds, WheelPositions>& kinematics,
    const Rotation2d& gyroAngle, const WheelPositions& wheelPositions,
    const Pose2d& initialPose)
    : Odometry3d{kinematics, Rotation3d{0_rad, 0_rad, gyroAngle.Radians()},
                 wheelPositions, Pose3d{initialPose}} {}

template <typename WheelSpeeds, typename WheelPositions>
Odometry3d<WheelSpeeds, WheelPositions>::Odometry3d(
    const Kinematics<WheelSpeeds, WheelPositions>& kinematics,
    const Rotation3d& gyroAngle, const WheelPositions& wheelPositions,
    const Pose3d& initialPose)
    : m_kinematics(kinematics),
      m_pose(initialPose),
      m_previousWheelPositions(wheelPositions) {
  m_previousAngle = m_pose.Rotation();
  m_gyroOffset = m_pose.Rotation() - gyroAngle;
}

template <typename WheelSpeeds, typename WheelPositions>
const Pose2d& Odometry3d<WheelSpeeds, WheelPositions>::Update(
    const Rotation2d& gyroAngle, const WheelPositions& wheelPositions) {
  Update(Rotation3d{0_rad, 0_rad, gyroAngle.Radians()}, wheelPositions);
  return GetPose();
}

template <typename WheelSpeeds, typename WheelPositions>
const Pose3d& Odometry3d<WheelSpeeds, WheelPositions>::Update(
    const Rotation3d& gyroAngle, const WheelPositions& wheelPositions) {
  auto angle = gyroAngle + m_gyroOffset;
  auto angle_difference =
      (angle - m_previousAngle).GetQuaternion().ToRotationVector();

  auto twist2d =
      m_kinematics.ToTwist2d(m_previousWheelPositions, wheelPositions);
  Twist3d twist{twist2d.dx,
                twist2d.dy,
                0_m,
                units::radian_t{angle_difference(0)},
                units::radian_t{angle_difference(1)},
                units::radian_t{angle_difference(2)}};

  auto newPose = m_pose.Exp(twist);

  m_previousWheelPositions = wheelPositions;
  m_previousAngle = angle;
  m_pose = {newPose.Translation(), angle};
  m_pose2d = m_pose.ToPose2d();

  return m_pose;
}
}  // namespace frc
