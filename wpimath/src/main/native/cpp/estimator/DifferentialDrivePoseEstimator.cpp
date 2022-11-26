// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/estimator/DifferentialDrivePoseEstimator.h"

#include <wpi/timestamp.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"

using namespace frc;

DifferentialDrivePoseEstimator::DifferentialDrivePoseEstimator(
    const Rotation2d& gyroAngle, units::meter_t leftDistance,
    units::meter_t rightDistance, const Pose2d& initialPose,
    const wpi::array<double, 3>& stateStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs)
    : m_odometry{gyroAngle, leftDistance, rightDistance, initialPose},
      m_prevGyroAngle{gyroAngle},
      m_prevLeftDistance{leftDistance},
      m_prevRightDistance{rightDistance} {
  for (size_t i = 0; i < 3; ++i) {
    m_q[i] = stateStdDevs[i] * stateStdDevs[i];
  }

  SetVisionMeasurementStdDevs(visionMeasurementStdDevs);
}

void DifferentialDrivePoseEstimator::SetVisionMeasurementStdDevs(
    const wpi::array<double, 3>& visionMeasurementStdDevs) {
  wpi::array<double, 3> r{wpi::empty_array};
  for (size_t i = 0; i < 3; ++i) {
    r[i] = visionMeasurementStdDevs[i] * visionMeasurementStdDevs[i];
  }

  // Solve for closed form Kalman gain for continuous Kalman filter with A = 0
  // and C = I. See wpimath/algorithms.md.
  for (size_t row = 0; row < 3; ++row) {
    if (m_q[row] == 0.0) {
      m_visionK(row, row) = 0.0;
    } else {
      m_visionK(row, row) =
          m_q[row] / (m_q[row] + std::sqrt(m_q[row] * r[row]));
    }
  }
}

void DifferentialDrivePoseEstimator::ResetPosition(const Rotation2d& gyroAngle,
                                                   units::meter_t leftDistance,
                                                   units::meter_t rightDistance,
                                                   const Pose2d& pose) {
  // Reset state estimate and error covariance
  m_odometry.ResetPosition(gyroAngle, leftDistance, rightDistance, pose);
  m_poseBuffer.Clear();

  m_prevGyroAngle = gyroAngle;
  m_prevLeftDistance = leftDistance;
  m_prevRightDistance = rightDistance;
}

Pose2d DifferentialDrivePoseEstimator::GetEstimatedPosition() const {
  return m_odometry.GetPose();
}

void DifferentialDrivePoseEstimator::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  // Step 1: Get the estimated pose from when the vision measurement was made.
  auto sample = m_poseBuffer.Sample(timestamp);

  if (!sample.has_value()) {
    return;
  }

  // Step 2: Measure the twist between the odometry pose and the vision pose
  auto twist = sample.value().Log(visionRobotPose);

  // Step 3: We should not trust the twist entirely, so instead we scale this
  // twist by a Kalman gain matrix representing how much we trust vision
  // measurements compared to our current pose.
  frc::Vectord<3> k_times_twist =
      m_visionK *
      frc::Vectord<3>{twist.dx.value(), twist.dy.value(), twist.dtheta.value()};

  // Step 4: Convert back to Twist2d
  Twist2d scaledTwist{units::meter_t{k_times_twist(0)},
                      units::meter_t{k_times_twist(1)},
                      units::radian_t{k_times_twist(2)}};

  // Step 5: Apply scaled twist to the latest pose
  auto estimatedPose = GetEstimatedPosition().Exp(scaledTwist);

  // Step 6: Apply new pose to odometry
  m_odometry.ResetPosition(m_prevGyroAngle, m_prevLeftDistance,
                           m_prevRightDistance, estimatedPose);
}

Pose2d DifferentialDrivePoseEstimator::Update(const Rotation2d& gyroAngle,
                                              units::meter_t leftDistance,
                                              units::meter_t rightDistance) {
  return UpdateWithTime(units::microsecond_t(wpi::Now()), gyroAngle,
                        leftDistance, rightDistance);
}

Pose2d DifferentialDrivePoseEstimator::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    units::meter_t leftDistance, units::meter_t rightDistance) {
  m_poseBuffer.AddSample(currentTime, GetEstimatedPosition());

  m_odometry.Update(gyroAngle, leftDistance, rightDistance);

  m_prevGyroAngle = gyroAngle;
  m_prevLeftDistance = leftDistance;
  m_prevRightDistance = rightDistance;

  return GetEstimatedPosition();
}
