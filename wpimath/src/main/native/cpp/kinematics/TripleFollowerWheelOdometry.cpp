// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/TripleFollowerWheelOdometry.hpp"

#include "wpi/math/util/MathShared.hpp"

using namespace wpi::math;

TripleFollowerWheelOdometry::TripleFollowerWheelOdometry(
    TripleFollowerWheelKinematics kinematics, const Rotation2d& gyroAngle,
    const TripleFollowerWheelPositions& wheelPositions,
    const Pose2d& initialPose)
    : Odometry(m_kinematicsImpl, gyroAngle, wheelPositions, initialPose),
      m_kinematicsImpl(kinematics) {
  wpi::math::MathSharedStore::ReportUsage("TripleFollowerWheelOdometry", "");
}
