// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/estimator/PoseEstimator.h"

namespace frc {

template <WheelPositions T>
PoseEstimator<T>::PoseEstimator(
    Kinematics<T>& kinematics, Odometry<T>& odometry,
    const wpi::array<double, 3>& stateStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs)
    : m_kinematics(kinematics), m_odometry(odometry) {
  for (size_t i = 0; i < 3; ++i) {
    m_q[i] = stateStdDevs[i] * stateStdDevs[i];
  }

  SetVisionMeasurementStdDevs(visionMeasurementStdDevs);
}

template <WheelPositions T>
void PoseEstimator<T>::SetVisionMeasurementStdDevs(
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

template <WheelPositions T>
void PoseEstimator<T>::ResetPosition(const Rotation2d& gyroAngle,
                                     const T& wheelPositions,
                                     const Pose2d& pose) {
  // Reset state estimate and error covariance
  m_odometry.ResetPosition(gyroAngle, wheelPositions, pose);
  m_poseBuffer.Clear();
}

template <WheelPositions T>
Pose2d PoseEstimator<T>::GetEstimatedPosition() const {
  return m_odometry.GetPose();
}

template <WheelPositions T>
void PoseEstimator<T>::AddVisionMeasurement(const Pose2d& visionRobotPose,
                                            units::second_t timestamp) {
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

  // Step 2: Measure the twist between the odometry pose and the vision pose
  auto twist = sample.value().pose.Log(visionRobotPose);

  // Step 3: We should not trust the twist entirely, so instead we scale this
  // twist by a Kalman gain matrix representing how much we trust vision
  // measurements compared to our current pose.
  Vectord<3> k_times_twist =
      m_visionK *
      Vectord<3>{twist.dx.value(), twist.dy.value(), twist.dtheta.value()};

  // Step 4: Convert back to Twist2d
  Twist2d scaledTwist{units::meter_t{k_times_twist(0)},
                      units::meter_t{k_times_twist(1)},
                      units::radian_t{k_times_twist(2)}};

  // Step 5: Reset Odometry to state at sample with vision adjustment.
  m_odometry.ResetPosition(sample.value().gyroAngle,
                           sample.value().wheelPositions,
                           sample.value().pose.Exp(scaledTwist));

  // Step 6: Record the current pose to allow multiple measurements from the
  // same timestamp
  m_poseBuffer.AddSample(timestamp,
                         {GetEstimatedPosition(), sample.value().gyroAngle,
                          sample.value().wheelPositions});

  // Step 7: Replay odometry inputs between sample time and latest recorded
  // sample to update the pose buffer and correct odometry.
  auto internal_buf = m_poseBuffer.GetInternalBuffer();

  auto upper_bound =
      std::lower_bound(internal_buf.begin(), internal_buf.end(), timestamp,
                       [](const auto& pair, auto t) { return t > pair.first; });

  for (auto entry = upper_bound; entry != internal_buf.end(); entry++) {
    UpdateWithTime(entry->first, entry->second.gyroAngle,
                   entry->second.wheelPositions);
  }
}

template <WheelPositions T>
Pose2d PoseEstimator<T>::Update(const Rotation2d& gyroAngle,
                                const T& wheelPositions) {
  return UpdateWithTime(wpi::math::MathSharedStore::GetTimestamp(), gyroAngle,
                        wheelPositions);
}

template <WheelPositions T>
Pose2d PoseEstimator<T>::UpdateWithTime(units::second_t currentTime,
                                        const Rotation2d& gyroAngle,
                                        const T& wheelPositions) {
  m_odometry.Update(gyroAngle, wheelPositions);

  // Copy?
  T internalWheelPositions = wheelPositions;

  m_poseBuffer.AddSample(
      currentTime, {GetEstimatedPosition(), gyroAngle, internalWheelPositions});

  return GetEstimatedPosition();
}

template <WheelPositions T>
typename PoseEstimator<T>::InterpolationRecord
PoseEstimator<T>::InterpolationRecord::Interpolate(Kinematics<T>& kinematics,
                                                   InterpolationRecord endValue,
                                                   double i) const {
  if (i < 0) {
    return *this;
  } else if (i > 1) {
    return endValue;
  } else {
    // Find the new wheel distance measurements.
    T wheels_lerp =
        this->wheelPositions.Interpolate(endValue.wheelPositions, i);

    // Find the distance between this measurement and the
    // interpolated measurement.
    T wheels_delta = wheels_lerp - this->wheelPositions;

    // Find the new gyro angle.
    auto gyro = wpi::Lerp(this->gyroAngle, endValue.gyroAngle, i);

    // Create a twist to represent this changed based on the interpolated
    // sensor inputs.
    auto twist = kinematics.ToTwist2d(wheels_delta);
    twist.dtheta = (gyro - gyroAngle).Radians();

    return {pose.Exp(twist), gyro, wheels_lerp};
  }
}

}  // namespace frc
