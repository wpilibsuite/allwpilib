// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/estimator/PoseEstimator.h"

namespace frc {

template <typename WheelSpeeds, WheelPositions WheelPositions>
PoseEstimator<WheelSpeeds, WheelPositions>::PoseEstimator(
    Odometry<WheelSpeeds, WheelPositions>& odometry,
    const wpi::array<double, 3>& stateStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs)
    : m_odometry(odometry), m_poseEstimate{odometry.GetPose()} {
  for (size_t i = 0; i < 3; ++i) {
    m_q[i] = stateStdDevs[i] * stateStdDevs[i];
  }

  SetVisionMeasurementStdDevs(visionMeasurementStdDevs);
}

template <typename WheelSpeeds, WheelPositions WheelPositions>
void PoseEstimator<WheelSpeeds, WheelPositions>::SetVisionMeasurementStdDevs(
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

template <typename WheelSpeeds, WheelPositions WheelPositions>
void PoseEstimator<WheelSpeeds, WheelPositions>::ResetPosition(
    const Rotation2d& gyroAngle, const WheelPositions& wheelPositions,
    const Pose2d& pose) {
  // Reset state estimate and error covariance
  m_odometry.ResetPosition(gyroAngle, wheelPositions, pose);
  m_poseBuffer.Clear();
  m_poseEstimate = m_odometry.GetPose();
}

template <typename WheelSpeeds, WheelPositions WheelPositions>
Pose2d PoseEstimator<WheelSpeeds, WheelPositions>::GetEstimatedPosition()
    const {
  return m_poseEstimate;
}

template <typename WheelSpeeds, WheelPositions WheelPositions>
std::optional<Pose2d> PoseEstimator<WheelSpeeds, WheelPositions>::SampleAt(
    units::second_t timestamp) const {
  // TODO Use C++23 std::optional::transform
  std::optional<frc::Pose2d> sample = m_poseBuffer.Sample(timestamp);
  if (sample) {
    return *sample + Transform2d{m_odometry.GetPose(), m_poseEstimate};
  } else {
    return std::nullopt;
  }
}

template <typename WheelSpeeds, WheelPositions WheelPositions>
void PoseEstimator<WheelSpeeds, WheelPositions>::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  // Step 0: If this measurement is old enough to be outside the pose buffer's
  // timespan, skip.
  if (!m_poseBuffer.GetInternalBuffer().empty() &&
      m_poseBuffer.GetInternalBuffer().front().first - kBufferDuration >
          timestamp) {
    return;
  }

  // Step 1: Get the estimated pose from when the vision measurement was made.
  auto sample = m_poseBuffer.Sample(timestamp);

  if (!sample.has_value()) {
    return;
  }

  // Step 2: Record the odometry updates that have occurred since the vision
  // measurement
  Transform2d odometry_backtrack{m_odometry.GetPose(), *sample};
  Transform2d odometry_forward{*sample, m_odometry.GetPose()};

  // Step 3: Revert said odometry updates to get a state estimate from when the
  // vision measurement occurred.
  auto old_estimate = m_poseEstimate + odometry_backtrack;

  // Step 4: Measure the twist between the odometry pose and the vision pose.
  auto twist = old_estimate.Log(visionRobotPose);

  // Step 5: We should not trust the twist entirely, so instead we scale this
  // twist by a Kalman gain matrix representing how much we trust vision
  // measurements compared to our current pose.
  Eigen::Vector3d k_times_twist =
      m_visionK *
      Eigen::Vector3d{twist.dx.value(), twist.dy.value(), twist.dtheta.value()};

  // Step 6: Convert back to Twist2d.
  Twist2d scaledTwist{units::meter_t{k_times_twist(0)},
                      units::meter_t{k_times_twist(1)},
                      units::radian_t{k_times_twist(2)}};

  // Step 7: Apply this adjustment to the old estimate, then replay dometry
  // updates.
  m_poseEstimate = old_estimate.Exp(scaledTwist) + odometry_forward;
}

template <typename WheelSpeeds, WheelPositions WheelPositions>
Pose2d PoseEstimator<WheelSpeeds, WheelPositions>::Update(
    const Rotation2d& gyroAngle, const WheelPositions& wheelPositions) {
  return UpdateWithTime(wpi::math::MathSharedStore::GetTimestamp(), gyroAngle,
                        wheelPositions);
}

template <typename WheelSpeeds, WheelPositions WheelPositions>
Pose2d PoseEstimator<WheelSpeeds, WheelPositions>::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    const WheelPositions& wheelPositions) {
  auto lastOdom = m_odometry.GetPose();
  auto currOdom = m_odometry.Update(gyroAngle, wheelPositions);
  m_poseBuffer.AddSample(currentTime, currOdom);

  m_poseEstimate = m_poseEstimate.Exp(lastOdom.Log(currOdom));

  return GetEstimatedPosition();
}

}  // namespace frc
