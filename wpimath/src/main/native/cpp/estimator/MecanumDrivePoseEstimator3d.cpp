// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/MecanumDrivePoseEstimator3d.hpp"

#include <wpi/util/timestamp.h>

#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/math/estimator/AngleStatistics.hpp"
#include "wpi/math/util/MathShared.hpp"

using namespace frc;

frc::MecanumDrivePoseEstimator3d::MecanumDrivePoseEstimator3d(
    MecanumDriveKinematics& kinematics, const Rotation3d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose3d& initialPose)
    : MecanumDrivePoseEstimator3d{
          kinematics,           gyroAngle,
          wheelPositions,       initialPose,
          {0.1, 0.1, 0.1, 0.1}, {0.45, 0.45, 0.45, 0.45}} {}

frc::MecanumDrivePoseEstimator3d::MecanumDrivePoseEstimator3d(
    MecanumDriveKinematics& kinematics, const Rotation3d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose3d& initialPose,
    const wpi::array<double, 4>& stateStdDevs,
    const wpi::array<double, 4>& visionMeasurementStdDevs)
    : PoseEstimator3d(kinematics, m_odometryImpl, stateStdDevs,
                      visionMeasurementStdDevs),
      m_odometryImpl(kinematics, gyroAngle, wheelPositions, initialPose) {
  ResetPose(initialPose);
}
