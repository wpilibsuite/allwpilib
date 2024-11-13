// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/estimator/DifferentialDrivePoseEstimator3d.h"

using namespace frc;

DifferentialDrivePoseEstimator3d::DifferentialDrivePoseEstimator3d(
    DifferentialDriveKinematics& kinematics, const Rotation3d& gyroAngle,
    units::meter_t leftDistance, units::meter_t rightDistance,
    const Pose3d& initialPose)
    : DifferentialDrivePoseEstimator3d{
          kinematics,          gyroAngle,   leftDistance,
          rightDistance,       initialPose, {0.02, 0.02, 0.02, 0.01},
          {0.1, 0.1, 0.1, 0.1}} {}

DifferentialDrivePoseEstimator3d::DifferentialDrivePoseEstimator3d(
    DifferentialDriveKinematics& kinematics, const Rotation3d& gyroAngle,
    units::meter_t leftDistance, units::meter_t rightDistance,
    const Pose3d& initialPose, const wpi::array<double, 4>& stateStdDevs,
    const wpi::array<double, 4>& visionMeasurementStdDevs)
    : PoseEstimator3d(kinematics, m_odometryImpl, stateStdDevs,
                      visionMeasurementStdDevs),
      m_odometryImpl{gyroAngle, leftDistance, rightDistance, initialPose} {
  ResetPose(initialPose);
}
