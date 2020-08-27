/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/estimator/MecanumDrivePoseEstimator.h"

#include <limits>

#include "frc/StateSpaceUtil.h"

using namespace frc;

frc::MecanumDrivePoseEstimator::MecanumDrivePoseEstimator(
    const Rotation2d& gyroAngle, const Pose2d& initialPose,
    MecanumDriveKinematics kinematics, const Vector<3>& stateStdDevs,
    const Vector<1>& localMeasurementStdDevs,
    const Vector<3>& visionMeasurementStdDevs, units::second_t nominalDt)
    : m_observer(
          &MecanumDrivePoseEstimator::F,
          [](const Vector<4>& x, const Vector<3>& u) {
            return x.block<2, 1>(2, 0);
          },
          StdDevMatrixToArray<4>(frc::MakeMatrix<4, 1>(
              stateStdDevs(0), stateStdDevs(1), std::cos(stateStdDevs(2)),
              std::sin(stateStdDevs(2)))),
          StdDevMatrixToArray<2>(
              frc::MakeMatrix<2, 1>(std::cos(localMeasurementStdDevs(0)),
                                    std::sin(localMeasurementStdDevs(0)))),
          nominalDt),
      m_kinematics(kinematics),
      m_nominalDt(nominalDt) {
  // Construct R (covariances) matrix for vision measurements.
  Eigen::Matrix4d visionContR =
      frc::MakeCovMatrix<4>(StdDevMatrixToArray<4>(frc::MakeMatrix<4, 1>(
          visionMeasurementStdDevs(0), visionMeasurementStdDevs(1),
          std::cos(visionMeasurementStdDevs(2)),
          std::sin(visionMeasurementStdDevs(2)))));

  // Create and store discrete covariance matrix for vision measurements.
  m_visionDiscR = frc::DiscretizeR<4>(visionContR, m_nominalDt);

  // Create vision correction mechanism.
  m_visionCorrect = [&](const Vector<3>& u, const Vector<4>& y) {
    m_observer.Correct<4>(
        u, y, [](const Vector<4>& x, const Vector<3>& u) { return x; },
        m_visionDiscR);
  };

  // Set initial state.
  m_observer.SetXhat(PoseTo4dVector(initialPose));

  // Calculate offsets.
  m_gyroOffset = initialPose.Rotation() - gyroAngle;
  m_previousAngle = initialPose.Rotation();
}

void frc::MecanumDrivePoseEstimator::ResetPosition(
    const Pose2d& pose, const Rotation2d& gyroAngle) {
  // Set observer state.
  m_observer.SetXhat(PoseTo4dVector(pose));

  // Calculate offsets.
  m_gyroOffset = pose.Rotation() - gyroAngle;
  m_previousAngle = pose.Rotation();
}

Pose2d frc::MecanumDrivePoseEstimator::GetEstimatedPosition() const {
  return Pose2d(m_observer.Xhat(0) * 1_m, m_observer.Xhat(1) * 1_m,
                Rotation2d(m_observer.Xhat(2), m_observer.Xhat(3)));
}

void frc::MecanumDrivePoseEstimator::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  m_latencyCompensator.ApplyPastMeasurement<4>(&m_observer, m_nominalDt,
                                               PoseTo4dVector(visionRobotPose),
                                               m_visionCorrect, timestamp);
}

Pose2d frc::MecanumDrivePoseEstimator::Update(
    const Rotation2d& gyroAngle, const MecanumDriveWheelSpeeds& wheelSpeeds) {
  return UpdateWithTime(frc2::Timer::GetFPGATimestamp(), gyroAngle,
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

  auto localY =
      frc::MakeMatrix<2, 1>(std::cos(angle.Radians().template to<double>()),
                            std::sin(angle.Radians().template to<double>()));
  m_previousAngle = angle;

  m_latencyCompensator.AddObserverState(m_observer, u, localY, currentTime);

  m_observer.Predict(u, dt);
  m_observer.Correct(u, localY);

  return GetEstimatedPosition();
}

Vector<4> frc::MecanumDrivePoseEstimator::F(const Vector<4>& x,
                                            const Vector<3>& u) {
  return frc::MakeMatrix<4, 1>(u(0), u(1), -x(3) * u(2), x(2) * u(2));
}
