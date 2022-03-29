// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/estimator/DifferentialDrivePoseEstimator.h"

#include <wpi/timestamp.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"

using namespace frc;

DifferentialDrivePoseEstimator::DifferentialDrivePoseEstimator(
    const Rotation2d& gyroAngle, const Pose2d& initialPose,
    const wpi::array<double, 5>& stateStdDevs,
    const wpi::array<double, 3>& localMeasurementStdDevs,
    const wpi::array<double, 3>& visionMeasurmentStdDevs,
    units::second_t nominalDt)
    : m_observer(
          &DifferentialDrivePoseEstimator::F,
          [](const Eigen::Vector<double, 5>& x,
             const Eigen::Vector<double, 3>& u) {
            return Eigen::Vector<double, 3>{x(3, 0), x(4, 0), x(2, 0)};
          },
          stateStdDevs, localMeasurementStdDevs, frc::AngleMean<5, 5>(2),
          frc::AngleMean<3, 5>(2), frc::AngleResidual<5>(2),
          frc::AngleResidual<3>(2), frc::AngleAdd<5>(2), nominalDt),
      m_nominalDt(nominalDt) {
  SetVisionMeasurementStdDevs(visionMeasurmentStdDevs);

  // Create correction mechanism for vision measurements.
  m_visionCorrect = [&](const Eigen::Vector<double, 3>& u,
                        const Eigen::Vector<double, 3>& y) {
    m_observer.Correct<3>(
        u, y,
        [](const Eigen::Vector<double, 5>& x, const Eigen::Vector<double, 3>&) {
          return x.block<3, 1>(0, 0);
        },
        m_visionContR, frc::AngleMean<3, 5>(2), frc::AngleResidual<3>(2),
        frc::AngleResidual<5>(2), frc::AngleAdd<5>(2));
  };

  m_gyroOffset = initialPose.Rotation() - gyroAngle;
  m_previousAngle = initialPose.Rotation();
  m_observer.SetXhat(FillStateVector(initialPose, 0_m, 0_m));
}

void DifferentialDrivePoseEstimator::SetVisionMeasurementStdDevs(
    const wpi::array<double, 3>& visionMeasurmentStdDevs) {
  // Create R (covariances) for vision measurements.
  m_visionContR = frc::MakeCovMatrix(visionMeasurmentStdDevs);
}

void DifferentialDrivePoseEstimator::ResetPosition(
    const Pose2d& pose, const Rotation2d& gyroAngle) {
  // Reset state estimate and error covariance
  m_observer.Reset();
  m_poseBuffer.Clear();

  m_observer.SetXhat(FillStateVector(pose, 0_m, 0_m));

  m_gyroOffset = GetEstimatedPosition().Rotation() - gyroAngle;
  m_previousAngle = pose.Rotation();
}

Pose2d DifferentialDrivePoseEstimator::GetEstimatedPosition() const {
  return Pose2d(units::meter_t(m_observer.Xhat(0)),
                units::meter_t(m_observer.Xhat(1)),
                Rotation2d(units::radian_t(m_observer.Xhat(2))));
}

void DifferentialDrivePoseEstimator::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  if (auto sample = m_poseBuffer.Sample(timestamp)) {
    m_visionCorrect(Eigen::Vector<double, 3>::Zero(),
                    PoseTo3dVector(GetEstimatedPosition().TransformBy(
                        visionRobotPose - sample.value())));
  }
}

Pose2d DifferentialDrivePoseEstimator::Update(
    const Rotation2d& gyroAngle,
    const DifferentialDriveWheelSpeeds& wheelSpeeds,
    units::meter_t leftDistance, units::meter_t rightDistance) {
  return UpdateWithTime(units::microsecond_t(wpi::Now()), gyroAngle,
                        wheelSpeeds, leftDistance, rightDistance);
}

Pose2d DifferentialDrivePoseEstimator::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    const DifferentialDriveWheelSpeeds& wheelSpeeds,
    units::meter_t leftDistance, units::meter_t rightDistance) {
  auto dt = m_prevTime >= 0_s ? currentTime - m_prevTime : m_nominalDt;
  m_prevTime = currentTime;

  auto angle = gyroAngle + m_gyroOffset;
  auto omega = (gyroAngle - m_previousAngle).Radians() / dt;

  auto u = Eigen::Vector<double, 3>{
      (wheelSpeeds.left + wheelSpeeds.right).value() / 2.0, 0.0, omega.value()};

  m_previousAngle = angle;

  auto localY = Eigen::Vector<double, 3>{
      leftDistance.value(), rightDistance.value(), angle.Radians().value()};

  m_poseBuffer.AddSample(currentTime, GetEstimatedPosition());
  m_observer.Predict(u, dt);
  m_observer.Correct(u, localY);

  return GetEstimatedPosition();
}

Eigen::Vector<double, 5> DifferentialDrivePoseEstimator::F(
    const Eigen::Vector<double, 5>& x, const Eigen::Vector<double, 3>& u) {
  // Apply a rotation matrix. Note that we do not add x because Runge-Kutta does
  // that for us.
  auto& theta = x(2);
  Eigen::Matrix<double, 5, 5> toFieldRotation{
      {std::cos(theta), -std::sin(theta), 0.0, 0.0, 0.0},
      {std::sin(theta), std::cos(theta), 0.0, 0.0, 0.0},
      {0.0, 0.0, 1.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 1.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 1.0}};
  return toFieldRotation *
         Eigen::Vector<double, 5>{u(0, 0), u(1, 0), u(2, 0), u(0, 0), u(1, 0)};
}

template <int Dim>
wpi::array<double, Dim> DifferentialDrivePoseEstimator::StdDevMatrixToArray(
    const Eigen::Vector<double, Dim>& stdDevs) {
  wpi::array<double, Dim> array;
  for (size_t i = 0; i < Dim; ++i) {
    array[i] = stdDevs(i);
  }
  return array;
}

Eigen::Vector<double, 5> DifferentialDrivePoseEstimator::FillStateVector(
    const Pose2d& pose, units::meter_t leftDistance,
    units::meter_t rightDistance) {
  return Eigen::Vector<double, 5>{pose.Translation().X().value(),
                                  pose.Translation().Y().value(),
                                  pose.Rotation().Radians().value(),
                                  leftDistance.value(), rightDistance.value()};
}
