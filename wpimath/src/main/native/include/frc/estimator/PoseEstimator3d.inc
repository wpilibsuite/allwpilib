// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/estimator/PoseEstimator3d.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Translation2d.h"

namespace frc {

template <typename WheelSpeeds, typename WheelPositions>
PoseEstimator3d<WheelSpeeds, WheelPositions>::PoseEstimator3d(
    Kinematics<WheelSpeeds, WheelPositions>& kinematics,
    Odometry3d<WheelSpeeds, WheelPositions>& odometry,
    const wpi::array<double, 3>& stateStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs)
    : PoseEstimator3d{kinematics,
                      odometry,
                      {stateStdDevs[0], stateStdDevs[1], 0.0, stateStdDevs[2]},
                      {visionMeasurementStdDevs[0], visionMeasurementStdDevs[1],
                       0.0, visionMeasurementStdDevs[2]}} {}

template <typename WheelSpeeds, typename WheelPositions>
PoseEstimator3d<WheelSpeeds, WheelPositions>::PoseEstimator3d(
    Kinematics<WheelSpeeds, WheelPositions>& kinematics,
    Odometry3d<WheelSpeeds, WheelPositions>& odometry,
    const wpi::array<double, 4>& stateStdDevs,
    const wpi::array<double, 4>& visionMeasurementStdDevs)
    : m_odometry(odometry), m_poseEstimate(m_odometry.GetPose3d()) {
  for (size_t i = 0; i < 4; ++i) {
    m_q[i] = stateStdDevs[i] * stateStdDevs[i];
  }

  SetVisionMeasurementStdDevs(visionMeasurementStdDevs);
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::SetVisionMeasurementStdDevs(
    const wpi::array<double, 3>& visionMeasurementStdDevs) {
  SetVisionMeasurementStdDevs({visionMeasurementStdDevs[0],
                               visionMeasurementStdDevs[1], 0.0,
                               visionMeasurementStdDevs[2]});
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::SetVisionMeasurementStdDevs(
    const wpi::array<double, 4>& visionMeasurementStdDevs) {
  wpi::array<double, 4> r{wpi::empty_array};
  for (size_t i = 0; i < 4; ++i) {
    r[i] = visionMeasurementStdDevs[i] * visionMeasurementStdDevs[i];
  }

  // Solve for closed form Kalman gain for continuous Kalman filter with A = 0
  // and C = I. See wpimath/algorithms.md.
  for (size_t row = 0; row < 4; ++row) {
    if (m_q[row] == 0.0) {
      m_visionK(row, row) = 0.0;
    } else {
      m_visionK(row, row) =
          m_q[row] / (m_q[row] + std::sqrt(m_q[row] * r[row]));
    }
  }
  double angle_gain = m_visionK(3, 3);
  m_visionK(4, 4) = angle_gain;
  m_visionK(5, 5) = angle_gain;
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::ResetPosition(
    const Rotation2d& gyroAngle, const WheelPositions& wheelPositions,
    const Pose2d& pose) {
  ResetPosition(Rotation3d{0_rad, 0_rad, gyroAngle.Radians()}, wheelPositions,
                Pose3d{pose});
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::ResetPosition(
    const Rotation3d& gyroAngle, const WheelPositions& wheelPositions,
    const Pose3d& pose) {
  // Reset state estimate and error covariance
  m_odometry.ResetPosition(gyroAngle, wheelPositions, pose);
  m_odometryPoseBuffer.Clear();
  m_visionUpdates.clear();
  m_poseEstimate = m_odometry.GetPose3d();
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::ResetPose(
    const Pose2d& pose) {
  ResetPose(Pose3d{pose});
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::ResetPose(
    const Pose3d& pose) {
  m_odometry.ResetPose(pose);
  m_odometryPoseBuffer.Clear();
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::ResetTranslation(
    const Translation2d& translation) {
  ResetTranslation(Translation3d{translation.X(), translation.Y(), 0_m});
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::ResetTranslation(
    const Translation3d& translation) {
  m_odometry.ResetTranslation(translation);
  m_odometryPoseBuffer.Clear();
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::ResetRotation(
    const Rotation2d& rotation) {
  ResetRotation(Rotation3d{0_rad, 0_rad, rotation.Radians()});
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::ResetRotation(
    const Rotation3d& rotation) {
  m_odometry.ResetTranslation(rotation);
  m_odometryPoseBuffer.Clear();
}

template <typename WheelSpeeds, typename WheelPositions>
Pose2d PoseEstimator3d<WheelSpeeds, WheelPositions>::GetEstimatedPosition()
    const {
  return GetEstimatedPosition3d().ToPose2d();
}

template <typename WheelSpeeds, typename WheelPositions>
Pose3d PoseEstimator3d<WheelSpeeds, WheelPositions>::GetEstimatedPosition3d()
    const {
  return m_poseEstimate;
  if (m_visionUpdates.empty()) {
    return m_odometry.GetPose3d();
  }
  auto visionUpdate = m_visionUpdates.rbegin()->second;
  return visionUpdate.Compensate(m_odometry.GetPose3d());
}

template <typename WheelSpeeds, typename WheelPositions>
std::optional<Pose2d> PoseEstimator3d<WheelSpeeds, WheelPositions>::SampleAt(
    units::second_t timestamp) const {
  auto sample = SampleAt3d(timestamp);
  // TODO Replace with std::optional::transform() in C++23
  if (sample) {
    return sample->ToPose2d();
  } else {
    return std::nullopt;
  }
}

template <typename WheelSpeeds, typename WheelPositions>
std::optional<Pose3d> PoseEstimator3d<WheelSpeeds, WheelPositions>::SampleAt3d(
    units::second_t timestamp) const {
  // Step 0: If there are no odometry updates to sample, skip.
  if (m_odometryPoseBuffer.GetInternalBuffer().empty()) {
    return std::nullopt;
  }

  // Step 1: Make sure timestamp matches the sample from the odometry pose
  // buffer. (When sampling, the buffer will always use a timestamp
  // between the first and last timestamps)
  units::second_t oldestOdometryTimestamp =
      m_odometryPoseBuffer.GetInternalBuffer().front().first;
  units::second_t newestOdometryTimestamp =
      m_odometryPoseBuffer.GetInternalBuffer().back().first;
  timestamp =
      std::clamp(timestamp, oldestOdometryTimestamp, newestOdometryTimestamp);

  // Step 2: If there are no applicable vision updates, use the odometry-only
  // information.
  if (m_visionUpdates.empty() || timestamp < m_visionUpdates.begin()->first) {
    return m_odometryPoseBuffer.Sample(timestamp);
  }

  // Step 3: Get the latest vision update from before or at the timestamp to
  // sample at.
  // First, find the iterator past the sample timestamp, then go back one. Note
  // that upper_bound() won't return begin() because we check begin() earlier.
  auto floorIter = m_visionUpdates.upper_bound(timestamp);
  --floorIter;
  auto visionUpdate = floorIter->second;

  // Step 4: Get the pose measured by odometry at the time of the sample.
  auto odometryEstimate = m_odometryPoseBuffer.Sample(timestamp);

  // Step 5: Apply the vision compensation to the odometry pose.
  // TODO Replace with std::optional::transform() in C++23
  if (odometryEstimate) {
    return visionUpdate.Compensate(*odometryEstimate);
  }
  return std::nullopt;
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::CleanUpVisionUpdates() {
  // Step 0: If there are no odometry samples, skip.
  if (m_odometryPoseBuffer.GetInternalBuffer().empty()) {
    return;
  }

  // Step 1: Find the oldest timestamp that needs a vision update.
  units::second_t oldestOdometryTimestamp =
      m_odometryPoseBuffer.GetInternalBuffer().front().first;

  // Step 2: If there are no vision updates before that timestamp, skip.
  if (m_visionUpdates.empty() ||
      oldestOdometryTimestamp < m_visionUpdates.begin()->first) {
    return;
  }

  // Step 3: Find the newest vision update timestamp before or at the oldest
  // timestamp.
  // First, find the iterator past the oldest odometry timestamp, then go
  // back one. Note that upper_bound() won't return begin() because we check
  // begin() earlier.
  auto newestNeededVisionUpdate =
      m_visionUpdates.upper_bound(oldestOdometryTimestamp);
  --newestNeededVisionUpdate;

  // Step 4: Remove all entries strictly before the newest timestamp we need.
  m_visionUpdates.erase(m_visionUpdates.begin(), newestNeededVisionUpdate);
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  AddVisionMeasurement(Pose3d{visionRobotPose}, timestamp);
}

template <typename WheelSpeeds, typename WheelPositions>
void PoseEstimator3d<WheelSpeeds, WheelPositions>::AddVisionMeasurement(
    const Pose3d& visionRobotPose, units::second_t timestamp) {
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
  auto visionSample = SampleAt3d(timestamp);

  if (!visionSample) {
    return;
  }

  // Step 4: Measure the twist between the old pose estimate and the vision
  // pose.
  auto twist = visionSample.value().Log(visionRobotPose);

  // Step 5: We should not trust the twist entirely, so instead we scale this
  // twist by a Kalman gain matrix representing how much we trust vision
  // measurements compared to our current pose.
  frc::Vectord<6> k_times_twist =
      m_visionK * frc::Vectord<6>{twist.dx.value(), twist.dy.value(),
                                  twist.dz.value(), twist.rx.value(),
                                  twist.ry.value(), twist.rz.value()};

  // Step 6: Convert back to Twist3d.
  Twist3d scaledTwist{
      units::meter_t{k_times_twist(0)},  units::meter_t{k_times_twist(1)},
      units::meter_t{k_times_twist(2)},  units::radian_t{k_times_twist(3)},
      units::radian_t{k_times_twist(4)}, units::radian_t{k_times_twist(5)}};

  // Step 7: Calculate and record the vision update.
  VisionUpdate visionUpdate{visionSample->Exp(scaledTwist), *odometrySample};
  m_visionUpdates[timestamp] = visionUpdate;

  // Step 8: Remove later vision measurements. (Matches previous behavior)
  auto firstAfter = m_visionUpdates.upper_bound(timestamp);
  m_visionUpdates.erase(firstAfter, m_visionUpdates.end());

  // Step 9: Update latest pose estimate. Since we cleared all updates after
  // this vision update, it's guaranteed to be the latest vision update.
  m_poseEstimate = visionUpdate.Compensate(m_odometry.GetPose3d());
}

template <typename WheelSpeeds, typename WheelPositions>
Pose2d PoseEstimator3d<WheelSpeeds, WheelPositions>::Update(
    const Rotation2d& gyroAngle, const WheelPositions& wheelPositions) {
  return Update(Rotation3d{0_rad, 0_rad, gyroAngle.Radians()}, wheelPositions)
      .ToPose2d();
}

template <typename WheelSpeeds, typename WheelPositions>
Pose3d PoseEstimator3d<WheelSpeeds, WheelPositions>::Update(
    const Rotation3d& gyroAngle, const WheelPositions& wheelPositions) {
  return UpdateWithTime(wpi::math::MathSharedStore::GetTimestamp(), gyroAngle,
                        wheelPositions);
}

template <typename WheelSpeeds, typename WheelPositions>
Pose2d PoseEstimator3d<WheelSpeeds, WheelPositions>::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    const WheelPositions& wheelPositions) {
  return UpdateWithTime(currentTime,
                        Rotation3d{0_rad, 0_rad, gyroAngle.Radians()},
                        wheelPositions)
      .ToPose2d();
}

template <typename WheelSpeeds, typename WheelPositions>
Pose3d PoseEstimator3d<WheelSpeeds, WheelPositions>::UpdateWithTime(
    units::second_t currentTime, const Rotation3d& gyroAngle,
    const WheelPositions& wheelPositions) {
  auto odometryEstimate = m_odometry.Update(gyroAngle, wheelPositions);

  m_odometryPoseBuffer.AddSample(currentTime, odometryEstimate);

  if (m_visionUpdates.empty()) {
    m_poseEstimate = odometryEstimate;
  } else {
    auto visionUpdate = m_visionUpdates.rbegin()->second;
    m_poseEstimate = visionUpdate.Compensate(odometryEstimate);
  }

  return GetEstimatedPosition3d();
}

}  // namespace frc
