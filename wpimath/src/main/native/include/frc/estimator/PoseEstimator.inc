// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/estimator/PoseEstimator.h"

namespace frc {

template <typename WheelSpeeds, typename WheelPositions>
PoseEstimator<WheelSpeeds, WheelPositions>::PoseEstimator(
    Kinematics<WheelSpeeds, WheelPositions>& kinematics,
    Odometry<WheelSpeeds, WheelPositions>& odometry,
    const wpi::array<double, 3>& stateStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs)
    : m_odometry(odometry) {
  for (size_t i = 0; i < 3; ++i) {
    m_q[i] = stateStdDevs[i] * stateStdDevs[i];
  }

  SetVisionMeasurementStdDevs(visionMeasurementStdDevs);
}

template <typename WheelSpeeds, typename WheelPositions>
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

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator<WheelSpeeds, WheelPositions>::ResetPosition(
    const Rotation2d& gyroAngle, const WheelPositions& wheelPositions,
    const Pose2d& pose) {
  // Reset state estimate and error covariance
  m_odometry.ResetPosition(gyroAngle, wheelPositions, pose);
  m_odometryPoseBuffer.Clear();
  m_visionUpdates.clear();
}

template <typename WheelSpeeds, typename WheelPositions>
Pose2d PoseEstimator<WheelSpeeds, WheelPositions>::GetEstimatedPosition()
    const {
  if (m_visionUpdates.empty()) {
    return m_odometry.GetPose();
  }
  auto visionUpdate = m_visionUpdates.rbegin()->second;
  return visionUpdate.Compensate(m_odometry.GetPose());
}

template <typename WheelSpeeds, typename WheelPositions>
std::optional<Pose2d> PoseEstimator<WheelSpeeds, WheelPositions>::SampleAt(
    units::second_t timestamp) const {
  if (m_odometryPoseBuffer.GetInternalBuffer().empty()) {
    // No odometry updates, so nothing to sample
    return std::nullopt;
  }
  // Make sure timestamp matches the sample from the odometry pose buffer
  units::second_t oldestOdometryTimestamp =
      m_odometryPoseBuffer.GetInternalBuffer().front().first;
  units::second_t newestOdometryTimestamp =
      m_odometryPoseBuffer.GetInternalBuffer().back().first;
  timestamp =
      std::clamp(timestamp, oldestOdometryTimestamp, newestOdometryTimestamp);
  if (m_visionUpdates.empty() || timestamp < m_visionUpdates.begin()->first) {
    // No vision update from before the requested timestamp to apply
    return m_odometryPoseBuffer.Sample(timestamp);
  }
  // Find first one past timestamp, then go back one
  // Note that upper_bound() won't return begin() because we check begin()
  // earlier
  auto floorIter = m_visionUpdates.upper_bound(timestamp);
  --floorIter;
  auto visionUpdate = floorIter->second;
  auto odometryEstimate = m_odometryPoseBuffer.Sample(timestamp);
  // TODO Replace with std::optional::transform() in C++23
  if (odometryEstimate) {
    return visionUpdate.Compensate(*odometryEstimate);
  }
  return std::nullopt;
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator<WheelSpeeds, WheelPositions>::CleanUpVisionUpdates() {
  if (m_odometryPoseBuffer.GetInternalBuffer().empty()) {
    // No odometry updates, so no vision updates as well
    return;
  }
  // Find the oldest timestamp that needs a vision update before or at it
  units::second_t oldestOdometryTimestamp =
      m_odometryPoseBuffer.GetInternalBuffer().front().first;
  if (m_visionUpdates.empty() ||
      oldestOdometryTimestamp < m_visionUpdates.begin()->first) {
    // No vision updates before the oldest odometry update, so no entries to
    // clear
    return;
  }
  // Find first one past oldest odometry timestamp, then go back one
  // Note that upper_bound() won't return begin() because we check begin()
  // earlier
  auto newestNeededVisionUpdate =
      m_visionUpdates.upper_bound(oldestOdometryTimestamp);
  --newestNeededVisionUpdate;
  // Remove all entries strictly before the newest timestamp we need
  m_visionUpdates.erase(m_visionUpdates.begin(), newestNeededVisionUpdate);
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator<WheelSpeeds, WheelPositions>::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  // Step 0: If this measurement is old enough to be outside the pose buffer's
  // timespan, skip.
  if (m_odometryPoseBuffer.GetInternalBuffer().empty() ||
      m_odometryPoseBuffer.GetInternalBuffer().front().first - kBufferDuration >
          timestamp) {
    return;
  }

  // Step 1: Clean up any old entries
  CleanUpVisionUpdates();

  // Step 2: Get the pose measured by odometry at the moment the vision
  // measurement was made.
  auto odometrySample = m_odometryPoseBuffer.Sample(timestamp);

  if (!odometrySample) {
    return;
  }

  // Step 3: Get the vision-compensated pose estimate at the moment the vision
  // measurement was made.
  auto visionSample = SampleAt(timestamp);

  if (!visionSample) {
    return;
  }

  // Step 4: Measure the twist between the old pose estimate and the vision
  // pose.
  auto twist = visionSample.value().Log(visionRobotPose);

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

  // Step 7: Calculate and record the vision update.
  VisionUpdate visionUpdate{visionSample->Exp(scaledTwist), *odometrySample};
  m_visionUpdates[timestamp] = visionUpdate;

  // Step 8: Remove later vision measurements. (Matches previous behavior)
  auto firstAfter = m_visionUpdates.upper_bound(timestamp);
  m_visionUpdates.erase(firstAfter, m_visionUpdates.end());
}

template <typename WheelSpeeds, typename WheelPositions>
Pose2d PoseEstimator<WheelSpeeds, WheelPositions>::Update(
    const Rotation2d& gyroAngle, const WheelPositions& wheelPositions) {
  return UpdateWithTime(wpi::math::MathSharedStore::GetTimestamp(), gyroAngle,
                        wheelPositions);
}

template <typename WheelSpeeds, typename WheelPositions>
Pose2d PoseEstimator<WheelSpeeds, WheelPositions>::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    const WheelPositions& wheelPositions) {
  auto odometryEstimate = m_odometry.Update(gyroAngle, wheelPositions);

  m_odometryPoseBuffer.AddSample(currentTime, odometryEstimate);

  return GetEstimatedPosition();
}

}  // namespace frc
