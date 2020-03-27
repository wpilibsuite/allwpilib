/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/estimator/DifferentialDrivePoseEstimator.h"

#include "frc/StateSpaceUtil.h"
#include "frc2/Timer.h"

using namespace frc;

DifferentialDrivePoseEstimator::DifferentialDrivePoseEstimator(
    const Rotation2d& gyroAngle, const Pose2d& initialPose,
    const Vector<3>& stateStdDevs, const Vector<3>& measurementStdDevs,
    units::second_t nominalDt)
    : m_observer(GetObserverSystem(), StdDevMatrixToArray(stateStdDevs),
                 StdDevMatrixToArray(measurementStdDevs), nominalDt),
      m_nominalDt(nominalDt) {
  m_gyroOffset = initialPose.Rotation() - gyroAngle;
  m_previousAngle = initialPose.Rotation();
  m_observer.SetXhat(PoseToVector(initialPose));
}

void DifferentialDrivePoseEstimator::ResetPosition(
    const Pose2d& pose, const Rotation2d& gyroAngle) {
  m_previousAngle = pose.Rotation();
  m_gyroOffset = GetEstimatedPosition().Rotation() - gyroAngle;
}

Pose2d DifferentialDrivePoseEstimator::GetEstimatedPosition() const {
  return Pose2d(units::meter_t(m_observer.Xhat(0)),
                units::meter_t(m_observer.Xhat(1)),
                Rotation2d(units::radian_t(m_observer.Xhat(2))));
}

void DifferentialDrivePoseEstimator::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  m_latencyCompensator.ApplyPastMeasurement(
      m_observer, m_nominalDt, PoseToVector(visionRobotPose), timestamp);
}

Pose2d DifferentialDrivePoseEstimator::Update(
    const Rotation2d& gyroAngle, units::meters_per_second_t leftVelocity,
    units::meters_per_second_t rightVelocity) {
  return UpdateWithTime(frc2::Timer::GetFPGATimestamp(), gyroAngle,
                        leftVelocity, rightVelocity);
}

Pose2d DifferentialDrivePoseEstimator::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    units::meters_per_second_t leftVelocity,
    units::meters_per_second_t rightVelocity) {
  auto dt = m_prevTime >= 0_s ? currentTime - m_prevTime : m_nominalDt;
  m_prevTime = currentTime;

  auto angle = gyroAngle + m_gyroOffset;
  auto omega = (gyroAngle - m_previousAngle).Radians() / dt;

  auto velocity = (leftVelocity + rightVelocity) / 2.0;

  auto u = frc::MakeMatrix<3, 1>(velocity.to<double>() * angle.Cos(),
                                 velocity.to<double>() * angle.Sin(),
                                 omega.to<double>());

  m_latencyCompensator.AddObserverState(m_observer, u, currentTime);
  m_observer.Predict(u, dt);

  return GetEstimatedPosition();
}

LinearSystem<3, 3, 3>& DifferentialDrivePoseEstimator::GetObserverSystem() {
  static LinearSystem<3, 3, 3> observerSystem{
      Eigen::MatrixXd::Zero(3, 3),
      Eigen::MatrixXd::Identity(3, 3),
      Eigen::MatrixXd::Identity(3, 3),
      Eigen::MatrixXd::Zero(3, 3),
      frc::MakeMatrix<3, 1>(-std::numeric_limits<double>::max(),
                            -std::numeric_limits<double>::max(),
                            -std::numeric_limits<double>::max()),
      frc::MakeMatrix<3, 1>(std::numeric_limits<double>::max(),
                            std::numeric_limits<double>::max(),
                            std::numeric_limits<double>::max())};
  return observerSystem;
}

std::array<double, 3> DifferentialDrivePoseEstimator::StdDevMatrixToArray(
    const Vector<3>& stdDevs) {
  return std::array<double, 3>{stdDevs(0, 0), stdDevs(1, 0), stdDevs(2, 0)};
}
