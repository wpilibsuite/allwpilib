// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/estimator/MecanumDrivePoseEstimator.h"

#include <wpi/timestamp.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"

using namespace frc;

frc::MecanumDrivePoseEstimator::MecanumDrivePoseEstimator(
    const Rotation2d& gyroAngle, const Pose2d& initialPose,
    MecanumDriveKinematics kinematics,
    const wpi::array<double, 3>& stateStdDevs,
    const wpi::array<double, 1>& localMeasurementStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs,
    units::second_t nominalDt)
    : m_observer(
          [](const Eigen::Vector<double, 3>& x,
             const Eigen::Vector<double, 3>& u) { return u; },
          [](const Eigen::Vector<double, 3>& x,
             const Eigen::Vector<double, 3>& u) { return x.block<1, 1>(2, 0); },
          stateStdDevs, localMeasurementStdDevs, frc::AngleMean<3, 3>(2),
          frc::AngleMean<1, 3>(0), frc::AngleResidual<3>(2),
          frc::AngleResidual<1>(0), frc::AngleAdd<3>(2), nominalDt),
      m_kinematics(kinematics),
      m_nominalDt(nominalDt) {
  SetVisionMeasurementStdDevs(visionMeasurementStdDevs);

  // Create vision correction mechanism.
  m_visionCorrect = [&](const Eigen::Vector<double, 3>& u,
                        const Eigen::Vector<double, 3>& y) {
    m_observer.Correct<3>(
        u, y,
        [](const Eigen::Vector<double, 3>& x, const Eigen::Vector<double, 3>&) {
          return x;
        },
        m_visionContR, frc::AngleMean<3, 3>(2), frc::AngleResidual<3>(2),
        frc::AngleResidual<3>(2), frc::AngleAdd<3>(2));
  };

  // Set initial state.
  m_observer.SetXhat(PoseTo3dVector(initialPose));

  // Calculate offsets.
  m_gyroOffset = initialPose.Rotation() - gyroAngle;
  m_previousAngle = initialPose.Rotation();
}

void frc::MecanumDrivePoseEstimator::SetVisionMeasurementStdDevs(
    const wpi::array<double, 3>& visionMeasurmentStdDevs) {
  // Create R (covariances) for vision measurements.
  m_visionContR = frc::MakeCovMatrix(visionMeasurmentStdDevs);
}

void frc::MecanumDrivePoseEstimator::ResetPosition(
    const Pose2d& pose, const Rotation2d& gyroAngle) {
  // Reset state estimate and error covariance
  m_observer.Reset();
  m_poseBuffer.Clear();

  m_observer.SetXhat(PoseTo3dVector(pose));

  m_gyroOffset = pose.Rotation() - gyroAngle;
  m_previousAngle = pose.Rotation();
}

Pose2d frc::MecanumDrivePoseEstimator::GetEstimatedPosition() const {
  return Pose2d(m_observer.Xhat(0) * 1_m, m_observer.Xhat(1) * 1_m,
                Rotation2d(units::radian_t{m_observer.Xhat(2)}));
}

void frc::MecanumDrivePoseEstimator::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  if (auto sample = m_poseBuffer.Sample(timestamp)) {
    m_visionCorrect(Eigen::Vector<double, 3>::Zero(),
                    PoseTo3dVector(GetEstimatedPosition().TransformBy(
                        visionRobotPose - sample.value())));
  }
}

Pose2d frc::MecanumDrivePoseEstimator::Update(
    const Rotation2d& gyroAngle, const MecanumDriveWheelSpeeds& wheelSpeeds) {
  return UpdateWithTime(units::microsecond_t(wpi::Now()), gyroAngle,
                        wheelSpeeds);
}

Pose2d frc::MecanumDrivePoseEstimator::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    const MecanumDriveWheelSpeeds& wheelSpeeds) {
  auto dt = m_prevTime >= 0_s ? currentTime - m_prevTime : m_nominalDt;
  m_prevTime = currentTime;

  auto angle = gyroAngle + m_gyroOffset;
  auto omega = (angle - m_previousAngle).Radians() / dt;

  auto chassisSpeeds = m_kinematics.ToChassisSpeeds(wheelSpeeds);
  auto fieldRelativeVelocities =
      Translation2d(chassisSpeeds.vx * 1_s, chassisSpeeds.vy * 1_s)
          .RotateBy(angle);

  Eigen::Vector<double, 3> u{fieldRelativeVelocities.X().value(),
                             fieldRelativeVelocities.Y().value(),
                             omega.value()};

  Eigen::Vector<double, 1> localY{angle.Radians().value()};
  m_previousAngle = angle;

  m_poseBuffer.AddSample(currentTime, GetEstimatedPosition());

  m_observer.Predict(u, dt);
  m_observer.Correct(u, localY);

  return GetEstimatedPosition();
}
