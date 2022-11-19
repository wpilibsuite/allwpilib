// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/MecanumDriveOdometry.h"

#include "wpimath/MathShared.h"

using namespace frc;

MecanumDriveOdometry::MecanumDriveOdometry(
    MecanumDriveKinematics kinematics, const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose2d& initialPose)
    : m_kinematics(kinematics),
      m_pose(initialPose),
      m_previousWheelPositions(wheelPositions) {
  m_previousAngle = m_pose.Rotation();
  m_gyroOffset = m_pose.Rotation() - gyroAngle;
  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kOdometry_MecanumDrive, 1);
}

const Pose2d& MecanumDriveOdometry::Update(
    const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions) {
  auto angle = gyroAngle + m_gyroOffset;

  MecanumDriveWheelPositions wheelDeltas{
      wheelPositions.frontLeft - m_previousWheelPositions.frontLeft,
      wheelPositions.frontRight - m_previousWheelPositions.frontRight,
      wheelPositions.rearLeft - m_previousWheelPositions.rearLeft,
      wheelPositions.rearRight - m_previousWheelPositions.rearRight,
  };

  auto twist = m_kinematics.ToTwist2d(wheelDeltas);
  twist.dtheta = (angle - m_previousAngle).Radians();

  auto newPose = m_pose.Exp(twist);

  m_previousAngle = angle;
  m_previousWheelPositions = wheelPositions;
  m_pose = {newPose.Translation(), angle};

  return m_pose;
}
