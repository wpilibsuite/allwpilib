/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/kinematics/MecanumDriveOdometry.h"

#include "wpimath/MathShared.h"

using namespace frc;

MecanumDriveOdometry::MecanumDriveOdometry(MecanumDriveKinematics kinematics,
                                           const Rotation2d& gyroAngle,
                                           const Pose2d& initialPose)
    : m_kinematics(kinematics), m_pose(initialPose) {
  m_previousAngle = m_pose.Rotation();
  m_gyroOffset = m_pose.Rotation() - gyroAngle;
  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kOdometry_MecanumDrive, 1);
}

const Pose2d& MecanumDriveOdometry::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    MecanumDriveWheelSpeeds wheelSpeeds) {
  units::second_t deltaTime =
      (m_previousTime >= 0_s) ? currentTime - m_previousTime : 0_s;
  m_previousTime = currentTime;

  auto angle = gyroAngle + m_gyroOffset;

  auto [dx, dy, dtheta] = m_kinematics.ToChassisSpeeds(wheelSpeeds);
  static_cast<void>(dtheta);

  auto newPose = m_pose.Exp(
      {dx * deltaTime, dy * deltaTime, (angle - m_previousAngle).Radians()});

  m_previousAngle = angle;
  m_pose = {newPose.Translation(), angle};

  return m_pose;
}
