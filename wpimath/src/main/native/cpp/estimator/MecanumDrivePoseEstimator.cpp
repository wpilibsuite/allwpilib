// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/estimator/MecanumDrivePoseEstimator.h"

#include <wpi/timestamp.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"

using namespace frc;

frc::MecanumDrivePoseEstimator::InterpolationRecord
frc::MecanumDrivePoseEstimator::InterpolationRecord::Interpolate(
    MecanumDriveKinematics& kinematics, InterpolationRecord endValue,
    double i) const {
  if (i < 0) {
    return *this;
  } else if (i > 1) {
    return endValue;
  } else {
    // Find the new wheel distance measurements.
    MecanumDriveWheelPositions wheels_lerp{
        wpi::Lerp(this->wheelPositions.frontLeft,
                  endValue.wheelPositions.frontLeft, i),
        wpi::Lerp(this->wheelPositions.frontRight,
                  endValue.wheelPositions.frontRight, i),
        wpi::Lerp(this->wheelPositions.rearLeft,
                  endValue.wheelPositions.rearLeft, i),
        wpi::Lerp(this->wheelPositions.rearRight,
                  endValue.wheelPositions.rearRight, i)};

    // Find the distance between this measurement and the
    // interpolated measurement.
    MecanumDriveWheelPositions wheels_delta{
        wheels_lerp.frontLeft - this->wheelPositions.frontLeft,
        wheels_lerp.frontRight - this->wheelPositions.frontRight,
        wheels_lerp.rearLeft - this->wheelPositions.rearLeft,
        wheels_lerp.rearRight - this->wheelPositions.rearRight};

    // Find the new gyro angle.
    auto gyro = wpi::Lerp(this->gyroAngle, endValue.gyroAngle, i);

    // Create a twist to represent this changed based on the interpolated
    // sensor inputs.
    auto twist = kinematics.ToTwist2d(wheels_delta);
    twist.dtheta = (gyro - gyroAngle).Radians();

    return {pose.Exp(twist), gyro, wheels_lerp};
  }
}

frc::MecanumDrivePoseEstimator::MecanumDrivePoseEstimator(
    MecanumDriveKinematics& kinematics, const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions, const Pose2d& initialPose,
    const wpi::array<double, 3>& stateStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs)
    : m_kinematics{kinematics},
      m_odometry{kinematics, gyroAngle, wheelPositions, initialPose} {
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
  auto twist = sample.value().pose.Log(visionRobotPose);

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

  // Step 5: Reset Odometry to state at sample with vision adjustment.
  m_odometry.ResetPosition(sample.value().gyroAngle,
                           sample.value().wheelPositions,
                           sample.value().pose.Exp(scaledTwist));

  // Step 6: Replay odometry inputs between sample time and latest recorded
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

Pose2d frc::MecanumDrivePoseEstimator::Update(
    const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions) {
  return UpdateWithTime(units::microsecond_t(wpi::Now()), gyroAngle,
                        wheelPositions);
}

Pose2d frc::MecanumDrivePoseEstimator::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    const MecanumDriveWheelPositions& wheelPositions) {
  m_odometry.Update(gyroAngle, wheelPositions);

  MecanumDriveWheelPositions internalWheelPositions{
      wheelPositions.frontLeft, wheelPositions.frontRight,
      wheelPositions.rearLeft, wheelPositions.rearRight};

  m_poseBuffer.AddSample(
      currentTime, {GetEstimatedPosition(), gyroAngle, internalWheelPositions});

  return GetEstimatedPosition();
}
