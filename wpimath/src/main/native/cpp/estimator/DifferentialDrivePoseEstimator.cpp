/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/estimator/DifferentialDrivePoseEstimator.h"

#include <wpi/timestamp.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"

using namespace frc;

DifferentialDrivePoseEstimator::DifferentialDrivePoseEstimator(
    const Rotation2d& gyroAngle, const Pose2d& initialPose,
    const std::array<double, 5>& stateStdDevs,
    const std::array<double, 3>& localMeasurementStdDevs,
    const std::array<double, 3>& visionMeasurmentStdDevs,
    units::second_t nominalDt)
    : m_observer(
          &DifferentialDrivePoseEstimator::F,
          [](const Eigen::Matrix<double, 5, 1>& x,
             const Eigen::Matrix<double, 3, 1>& u) {
            return frc::MakeMatrix<3, 1>(x(3, 0), x(4, 0), x(2, 0));
          },
          stateStdDevs, localMeasurementStdDevs, frc::AngleMean<5, 5>(2),
          frc::AngleMean<3, 5>(2), frc::AngleResidual<5>(2),
          frc::AngleResidual<3>(2), frc::AngleAdd<5>(2), nominalDt),
      m_nominalDt(nominalDt) {
  // Create R (covariances) for vision measurements.
  Eigen::Matrix<double, 3, 3> visionContR =
      frc::MakeCovMatrix(visionMeasurmentStdDevs);
  m_visionDiscR = frc::DiscretizeR<3>(visionContR, m_nominalDt);

  // Create correction mechanism for vision measurements.
  m_visionCorrect = [&](const Eigen::Matrix<double, 3, 1>& u,
                        const Eigen::Matrix<double, 3, 1>& y) {
    m_observer.Correct<3>(
        u, y,
        [](const Eigen::Matrix<double, 5, 1>& x,
           const Eigen::Matrix<double, 3, 1>&) { return x.block<3, 1>(0, 0); },
        m_visionDiscR, frc::AngleMean<3, 5>(2), frc::AngleResidual<3>(2),
        frc::AngleResidual<5>(2), frc::AngleAdd<5>(2));
  };

  m_gyroOffset = initialPose.Rotation() - gyroAngle;
  m_previousAngle = initialPose.Rotation();
  m_observer.SetXhat(FillStateVector(initialPose, 0_m, 0_m));
}

void DifferentialDrivePoseEstimator::ResetPosition(
    const Pose2d& pose, const Rotation2d& gyroAngle) {
  m_previousAngle = pose.Rotation();
  m_gyroOffset = GetEstimatedPosition().Rotation() - gyroAngle;
  m_observer.SetXhat(FillStateVector(pose, 0_m, 0_m));
}

Pose2d DifferentialDrivePoseEstimator::GetEstimatedPosition() const {
  return Pose2d(units::meter_t(m_observer.Xhat(0)),
                units::meter_t(m_observer.Xhat(1)),
                Rotation2d(units::radian_t(m_observer.Xhat(2))));
}

void DifferentialDrivePoseEstimator::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  m_latencyCompensator.ApplyPastMeasurement<3>(&m_observer, m_nominalDt,
                                               PoseTo3dVector(visionRobotPose),
                                               m_visionCorrect, timestamp);
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

  auto u = frc::MakeMatrix<3, 1>(
      (wheelSpeeds.left + wheelSpeeds.right).to<double>() / 2.0, 0.0,
      omega.to<double>());

  m_previousAngle = angle;

  auto localY = frc::MakeMatrix<3, 1>(leftDistance.to<double>(),
                                      rightDistance.to<double>(),
                                      angle.Radians().to<double>());

  m_latencyCompensator.AddObserverState(m_observer, u, localY, currentTime);
  m_observer.Predict(u, dt);
  m_observer.Correct(u, localY);

  return GetEstimatedPosition();
}

Eigen::Matrix<double, 5, 1> DifferentialDrivePoseEstimator::F(
    const Eigen::Matrix<double, 5, 1>& x,
    const Eigen::Matrix<double, 3, 1>& u) {
  // Apply a rotation matrix. Note that we do not add x because Runge-Kutta does
  // that for us.
  auto& theta = x(2, 0);
  Eigen::Matrix<double, 5, 5> toFieldRotation = frc::MakeMatrix<5, 5>(
      // clang-format off
    std::cos(theta), -std::sin(theta), 0.0, 0.0, 0.0,
    std::sin(theta), std::cos(theta), 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0);  // clang-format on
  return toFieldRotation *
         frc::MakeMatrix<5, 1>(u(0, 0), u(1, 0), u(2, 0), u(0, 0), u(1, 0));
}

template <int Dim>
std::array<double, Dim> DifferentialDrivePoseEstimator::StdDevMatrixToArray(
    const Eigen::Matrix<double, Dim, 1>& stdDevs) {
  std::array<double, Dim> array;
  for (size_t i = 0; i < Dim; ++i) {
    array[i] = stdDevs(i);
  }
  return array;
}

Eigen::Matrix<double, 5, 1> DifferentialDrivePoseEstimator::FillStateVector(
    const Pose2d& pose, units::meter_t leftDistance,
    units::meter_t rightDistance) {
  return frc::MakeMatrix<5, 1>(
      pose.Translation().X().to<double>(), pose.Translation().Y().to<double>(),
      pose.Rotation().Radians().to<double>(), leftDistance.to<double>(),
      rightDistance.to<double>());
}
