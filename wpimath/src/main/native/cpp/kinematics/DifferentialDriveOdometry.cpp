// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/DifferentialDriveOdometry.h"

#include "wpimath/MathShared.h"

using namespace frc;

DifferentialDriveOdometry::DifferentialDriveOdometry(
    const Rotation2d& gyroAngle, units::meter_t leftDistance,
    units::meter_t rightDistance, const Pose2d& initialPose)
    : m_pose(initialPose),
      m_prevLeftDistance(leftDistance),
      m_prevRightDistance(rightDistance) {
  m_previousAngle = m_pose.Rotation();
  m_gyroOffset = m_pose.Rotation() - gyroAngle;
  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kOdometry_DifferentialDrive, 1);
}

const Pose2d& DifferentialDriveOdometry::Update(const Rotation2d& gyroAngle,
                                                units::meter_t leftDistance,
                                                units::meter_t rightDistance) {
  auto deltaLeftDistance = leftDistance - m_prevLeftDistance;
  auto deltaRightDistance = rightDistance - m_prevRightDistance;

  m_prevLeftDistance = leftDistance;
  m_prevRightDistance = rightDistance;

  auto averageDeltaDistance = (deltaLeftDistance + deltaRightDistance) / 2.0;
  auto angle = gyroAngle + m_gyroOffset;

  auto newPose = m_pose.Exp(
      {averageDeltaDistance, 0_m, (angle - m_previousAngle).Radians()});

  m_previousAngle = angle;
  m_pose = {newPose.Translation(), angle};

  return m_pose;
}
