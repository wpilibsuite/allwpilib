// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/estimator/DifferentialDrivePoseEstimator.h"

#include <vector>

using namespace frc;

DifferentialDrivePoseEstimator::DifferentialDrivePoseEstimator(
    DifferentialDriveKinematics& kinematics, const Rotation2d& gyroAngle,
    units::meter_t leftDistance, units::meter_t rightDistance,
    const Pose2d& initialPose)
    : DifferentialDrivePoseEstimator{
          kinematics,  gyroAngle,          leftDistance,   rightDistance,
          initialPose, {0.02, 0.02, 0.01}, {0.1, 0.1, 0.1}} {}

DifferentialDrivePoseEstimator::DifferentialDrivePoseEstimator(
    DifferentialDriveKinematics& kinematics, const Rotation2d& gyroAngle,
    units::meter_t leftDistance, units::meter_t rightDistance,
    const Pose2d& initialPose, const wpi::array<double, 3>& stateStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs)
    : PoseEstimator(kinematics, m_odometryImpl, stateStdDevs,
                    visionMeasurementStdDevs),
      m_odometryImpl{gyroAngle, leftDistance, rightDistance, initialPose} {
  ResetPose(initialPose);
}
