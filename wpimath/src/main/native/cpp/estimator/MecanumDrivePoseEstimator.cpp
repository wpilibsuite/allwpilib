// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/estimator/MecanumDrivePoseEstimator.h"

#include <wpi/timestamp.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"

using namespace frc;

MecanumDrivePoseEstimator::MecanumDrivePoseEstimator(
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
          stateStdDevs, localMeasurementStdDevs, AngleMean<3, 3>(2),
          AngleMean<1, 3>(0), AngleResidual<3>(2), AngleResidual<1>(0),
          AngleAdd<3>(2), nominalDt),
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
        m_visionContR, AngleMean<3, 3>(2), AngleResidual<3>(2),
        AngleResidual<3>(2), AngleAdd<3>(2));
  };

  // Set initial state.
  m_observer.SetXhat(PoseTo3dVector(initialPose));

  // Calculate offsets.
  m_gyroOffset = initialPose.Rotation() - gyroAngle;
  m_previousAngle = initialPose.Rotation();
}

void MecanumDrivePoseEstimator::SetVisionMeasurementStdDevs(
    const wpi::array<double, 3>& visionMeasurmentStdDevs) {
  // Create R (covariances) for vision measurements.
  m_visionContR = MakeCovMatrix(visionMeasurmentStdDevs);
}

void MecanumDrivePoseEstimator::ResetPosition(const Pose2d& pose,
                                              const Rotation2d& gyroAngle) {
  // Reset state estimate and error covariance
  m_observer.Reset();
  m_latencyCompensator.Reset();

  m_observer.SetXhat(PoseTo3dVector(pose));

  m_gyroOffset = pose.Rotation() - gyroAngle;
  m_previousAngle = pose.Rotation();
}

Pose2d MecanumDrivePoseEstimator::GetEstimatedPosition() const {
  return Pose2d(m_observer.Xhat(0) * 1_m, m_observer.Xhat(1) * 1_m,
                Rotation2d(units::radian_t{m_observer.Xhat(2)}));
}

void MecanumDrivePoseEstimator::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  m_visionPose = visionRobotPose;
  m_visionLatency = m_prevTime - timestamp;
  m_latencyCompensator.ApplyPastGlobalMeasurement<3>(
      &m_observer, m_nominalDt, PoseTo3dVector(visionRobotPose),
      m_visionCorrect, timestamp);
}

Pose2d MecanumDrivePoseEstimator::Update(
    const Rotation2d& gyroAngle, const MecanumDriveWheelSpeeds& wheelSpeeds) {
  return UpdateWithTime(units::microsecond_t(wpi::Now()), gyroAngle,
                        wheelSpeeds);
}

Pose2d MecanumDrivePoseEstimator::UpdateWithTime(
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

  m_latencyCompensator.AddObserverState(m_observer, u, localY, currentTime);

  m_observer.Predict(u, dt);
  m_observer.Correct(u, localY);

  m_estimatedPose = GetEstimatedPosition();
  return m_estimatedPose;
}

void MecanumDrivePoseEstimator::InitSendable(wpi::SendableBuilder& builder) {
  m_observer.InitSendable(builder);
  builder.SetSmartDashboardType("MecanumDrivePoseEstimator");
  builder.AddDoubleProperty(
      "VisionPoseMetersX", [&] { return m_visionPose.X().value(); }, nullptr);
  builder.AddDoubleProperty(
      "VisionPoseMetersY", [&] { return m_visionPose.Y().value(); }, nullptr);
  builder.AddDoubleProperty(
      "VisionPoseDegrees",
      [&] { return m_visionPose.Rotation().Degrees().value(); }, nullptr);
  builder.AddDoubleProperty(
      "VisionLatencyMs",
      [&] { return units::millisecond_t{m_visionLatency}.value(); }, nullptr);
  builder.AddDoubleProperty(
      "EstimatedPoseMetersX", [&] { return m_estimatedPose.X().value(); },
      nullptr);
  builder.AddDoubleProperty(
      "EstimatedPoseMetersY", [&] { return m_estimatedPose.Y().value(); },
      nullptr);
  builder.AddDoubleProperty(
      "EstimatedPoseDegrees",
      [&] { return m_estimatedPose.Rotation().Degrees().value(); }, nullptr);
}
