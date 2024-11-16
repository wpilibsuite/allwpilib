// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/MecanumDriveOdometry3d.h"

#include "wpimath/MathShared.h"

using namespace frc;

MecanumDriveOdometry3d::MecanumDriveOdometry3d(
    MecanumDriveKinematics kinematics, const Rotation3d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose3d& initialPose)
    : Odometry3d(m_kinematicsImpl, gyroAngle, wheelPositions, initialPose),
      m_kinematicsImpl(kinematics) {
  wpi::math::MathSharedStore::ReportUsage(
      wpi::math::MathUsageId::kOdometry_MecanumDrive, 1);
}
