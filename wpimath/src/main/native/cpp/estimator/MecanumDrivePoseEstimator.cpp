/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/estimator/MecanumDrivePoseEstimator.h"

#include <wpi/timestamp.h>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/AngleStatistics.h"

using namespace frc;

frc::MecanumDrivePoseEstimator::MecanumDrivePoseEstimator(
    const Rotation2d& gyroAngle, const Pose2d& initialPose,
    MecanumDriveKinematics kinematics,
    const std::array<double, 3>& stateStdDevs,
    const std::array<double, 1>& localMeasurementStdDevs,
    const std::array<double, 3>& visionMeasurementStdDevs,
    units::second_t nominalDt)
    : m_observer([](const Eigen::Matrix<double, 3, 1>& x,
                    const Eigen::Matrix<double, 3, 1>& u) { return u; },
                 [](const Eigen::Matrix<double, 3, 1>& x,
                    const Eigen::Matrix<double, 3, 1>& u) {
                   return x.block<1, 1>(2, 0);
                 },
                 stateStdDevs, localMeasurementStdDevs, frc::AngleMean<3, 3>(2),
                 frc::AngleMean<1, 3>(0), frc::AngleResidual<3>(2),
                 frc::AngleResidual<1>(0), frc::AngleAdd<3>(2), nominalDt),
      m_kinematics(kinematics),
      m_nominalDt(nominalDt) {
  // Construct R (covariances) matrix for vision measurements.
  Eigen::Matrix3d visionContR = frc::MakeCovMatrix<3>(visionMeasurementStdDevs);

  // Create and store discrete covariance matrix for vision measurements.
  m_visionDiscR = frc::DiscretizeR<3>(visionContR, m_nominalDt);

  // Create vision correction mechanism.
  m_visionCorrect = [&](const Eigen::Matrix<double, 3, 1>& u,
                        const Eigen::Matrix<double, 3, 1>& y) {
    m_observer.Correct<3>(
        u, y,
        [](const Eigen::Matrix<double, 3, 1>& x,
           const Eigen::Matrix<double, 3, 1>&) { return x; },
        m_visionDiscR, frc::AngleMean<3, 3>(2), frc::AngleResidual<3>(2),
        frc::AngleResidual<3>(2), frc::AngleAdd<3>(2));
  };

  // Set initial state.
  m_observer.SetXhat(PoseTo3dVector(initialPose));

  // Calculate offsets.
  m_gyroOffset = initialPose.Rotation() - gyroAngle;
  m_previousAngle = initialPose.Rotation();
}

void frc::MecanumDrivePoseEstimator::ResetPosition(
    const Pose2d& pose, const Rotation2d& gyroAngle) {
  // Set observer state.
  m_observer.SetXhat(PoseTo3dVector(pose));

  // Calculate offsets.
  m_gyroOffset = pose.Rotation() - gyroAngle;
  m_previousAngle = pose.Rotation();
}

Pose2d frc::MecanumDrivePoseEstimator::GetEstimatedPosition() const {
  return Pose2d(m_observer.Xhat(0) * 1_m, m_observer.Xhat(1) * 1_m,
                Rotation2d(units::radian_t{m_observer.Xhat(2)}));
}

void frc::MecanumDrivePoseEstimator::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  m_latencyCompensator.ApplyPastMeasurement<3>(&m_observer, m_nominalDt,
                                               PoseTo3dVector(visionRobotPose),
                                               m_visionCorrect, timestamp);
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

  auto u = frc::MakeMatrix<3, 1>(fieldRelativeVelocities.X().to<double>(),
                                 fieldRelativeVelocities.Y().to<double>(),
                                 omega.to<double>());

  auto localY = frc::MakeMatrix<1, 1>(angle.Radians().template to<double>());
  m_previousAngle = angle;

  m_latencyCompensator.AddObserverState(m_observer, u, localY, currentTime);

  m_observer.Predict(u, dt);
  m_observer.Correct(u, localY);

  return GetEstimatedPosition();
}
