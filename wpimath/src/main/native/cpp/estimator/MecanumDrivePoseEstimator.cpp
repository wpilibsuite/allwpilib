// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/estimator/MecanumDrivePoseEstimator.h"

#include <wpi/timestamp.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"

using namespace frc;

frc::MecanumDrivePoseEstimator::MecanumDrivePoseEstimator(
    const MecanumDriveKinematics& kinematics, const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose2d& initialPose,
    const wpi::array<double, 3>& stateStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs)
    : m_odometry{kinematics, gyroAngle, wheelPositions, initialPose},
      m_prevGyroAngle(gyroAngle),
      m_prevWheelPositions{
          wheelPositions.frontLeft,
          wheelPositions.frontRight,
          wheelPositions.rearLeft,
          wheelPositions.rearRight,
      } {
  for (size_t i = 0; i < 3; ++i) {
    m_q[i] = stateStdDevs[i] * stateStdDevs[i];
  }

  SetVisionMeasurementStdDevs(visionMeasurementStdDevs);
}

void frc::MecanumDrivePoseEstimator::SetVisionMeasurementStdDevs(
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

void frc::MecanumDrivePoseEstimator::ResetPosition(
    const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose2d& pose) {
  // Reset state estimate and error covariance
  m_odometry.ResetPosition(gyroAngle, wheelPositions, pose);
  m_poseBuffer.Clear();

  m_prevGyroAngle = gyroAngle;
  m_prevWheelPositions.frontLeft = wheelPositions.frontLeft;
  m_prevWheelPositions.frontRight = wheelPositions.frontRight;
  m_prevWheelPositions.rearLeft = wheelPositions.rearLeft;
  m_prevWheelPositions.rearRight = wheelPositions.rearRight;
}

Pose2d frc::MecanumDrivePoseEstimator::GetEstimatedPosition() const {
  return m_odometry.GetPose();
}

void frc::MecanumDrivePoseEstimator::AddVisionMeasurement(
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
  m_odometry.ResetPosition(m_prevGyroAngle, m_prevWheelPositions,
                           estimatedPose);
}

Pose2d frc::MecanumDrivePoseEstimator::Update(
    const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions) {
  return UpdateWithTime(units::microsecond_t(wpi::Now()), gyroAngle,
                        wheelPositions);
}

Pose2d frc::MecanumDrivePoseEstimator::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions) {
  m_poseBuffer.AddSample(currentTime, GetEstimatedPosition());
  m_odometry.Update(gyroAngle, wheelPositions);

  m_prevGyroAngle = gyroAngle;
  m_prevWheelPositions.frontLeft = wheelPositions.frontLeft;
  m_prevWheelPositions.frontRight = wheelPositions.frontRight;
  m_prevWheelPositions.rearLeft = wheelPositions.rearLeft;
  m_prevWheelPositions.rearRight = wheelPositions.rearRight;

  return GetEstimatedPosition();
}
