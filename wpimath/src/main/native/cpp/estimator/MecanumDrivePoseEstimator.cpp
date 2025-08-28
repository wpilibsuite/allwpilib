// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/MathShared.hpp>
#include <wpi/math/StateSpaceUtil.hpp>
#include <wpi/math/estimator/AngleStatistics.hpp>
#include <wpi/math/estimator/MecanumDrivePoseEstimator.hpp>
#include <wpi/timestamp.h>

using namespace wpi::math;

wpi::math::MecanumDrivePoseEstimator::MecanumDrivePoseEstimator(
    MecanumDriveKinematics& kinematics, const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose2d& initialPose)
    : MecanumDrivePoseEstimator{kinematics,      gyroAngle,
                                wheelPositions,  initialPose,
                                {0.1, 0.1, 0.1}, {0.45, 0.45, 0.45}} {}

wpi::math::MecanumDrivePoseEstimator::MecanumDrivePoseEstimator(
    MecanumDriveKinematics& kinematics, const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose2d& initialPose,
    const wpi::array<double, 3>& stateStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs)
    : PoseEstimator(kinematics, m_odometryImpl, stateStdDevs,
                    visionMeasurementStdDevs),
      m_odometryImpl(kinematics, gyroAngle, wheelPositions, initialPose) {
  ResetPose(initialPose);
}
