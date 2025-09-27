// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/DifferentialDrivePoseEstimator.hpp"

#include <vector>

using namespace wpi::math;

DifferentialDrivePoseEstimator::DifferentialDrivePoseEstimator(
    DifferentialDriveKinematics& kinematics, const Rotation2d& gyroAngle,
    wpi::units::meter_t leftDistance, wpi::units::meter_t rightDistance,
    const Pose2d& initialPose)
    : DifferentialDrivePoseEstimator{
          kinematics,  gyroAngle,          leftDistance,   rightDistance,
          initialPose, {0.02, 0.02, 0.01}, {0.1, 0.1, 0.1}} {}

DifferentialDrivePoseEstimator::DifferentialDrivePoseEstimator(
    DifferentialDriveKinematics& kinematics, const Rotation2d& gyroAngle,
    wpi::units::meter_t leftDistance, wpi::units::meter_t rightDistance,
    const Pose2d& initialPose, const wpi::util::array<double, 3>& stateStdDevs,
    const wpi::util::array<double, 3>& visionMeasurementStdDevs)
    : PoseEstimator(kinematics, m_odometryImpl, stateStdDevs,
                    visionMeasurementStdDevs),
      m_odometryImpl{gyroAngle, leftDistance, rightDistance, initialPose} {
  ResetPose(initialPose);
}
