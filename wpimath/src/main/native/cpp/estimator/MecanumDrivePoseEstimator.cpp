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
    MecanumDriveWheelPositions wheelPositions,
    MecanumDriveKinematics kinematics,
    const wpi::array<double, 7>& stateStdDevs,
    const wpi::array<double, 5>& localMeasurementStdDevs,
    const wpi::array<double, 3>& visionMeasurementStdDevs,
    units::second_t nominalDt)
    : m_observer([](const Vectord<7>& x, const Vectord<7>& u) { return u; },
                 [](const Vectord<7>& x, const Vectord<7>& u) {
                   return x.block<5, 1>(2, 0);
                 },
                 stateStdDevs, localMeasurementStdDevs, frc::AngleMean<7, 7>(2),
                 frc::AngleMean<5, 7>(0), frc::AngleResidual<7>(2),
                 frc::AngleResidual<5>(0), frc::AngleAdd<7>(2), nominalDt),
      m_kinematics(kinematics),
      m_nominalDt(nominalDt) {
  SetVisionMeasurementStdDevs(visionMeasurementStdDevs);

  // Create vision correction mechanism.
  m_visionCorrect = [&](const Vectord<7>& u, const Vectord<3>& y) {
    m_observer.Correct<3>(
        u, y,
        [](const Vectord<7>& x, const Vectord<7>& u) {
          return x.template block<3, 1>(0, 0);
        },
        m_visionContR, frc::AngleMean<3, 7>(2), frc::AngleResidual<3>(2),
        frc::AngleResidual<7>(2), frc::AngleAdd<7>(2));
  };

  // Set initial state.
  auto poseVec = PoseTo3dVector(initialPose);
  auto xhat = Vectord<7>{
      poseVec(0),
      poseVec(1),
      poseVec(2),
      wheelPositions.frontLeft.value(),
      wheelPositions.frontRight.value(),
      wheelPositions.rearLeft.value(),
      wheelPositions.rearRight.value(),
  };

  m_observer.SetXhat(xhat);

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
    const Pose2d& pose, const Rotation2d& gyroAngle,
    MecanumDriveWheelPositions wheelPositions) {
  // Reset state estimate and error covariance
  m_observer.Reset();
  m_poseBuffer.Clear();

  auto poseVec = PoseTo3dVector(pose);
  auto xhat = Vectord<7>{
      poseVec(0),
      poseVec(1),
      poseVec(2),
      wheelPositions.frontLeft.value(),
      wheelPositions.frontRight.value(),
      wheelPositions.rearLeft.value(),
      wheelPositions.rearRight.value(),
  };

  m_observer.SetXhat(xhat);

  m_prevTime = -1_s;

  m_gyroOffset = pose.Rotation() - gyroAngle;
  m_previousAngle = pose.Rotation();
}

Pose2d frc::MecanumDrivePoseEstimator::GetEstimatedPosition() const {
  return Pose2d{m_observer.Xhat(0) * 1_m, m_observer.Xhat(1) * 1_m,
                units::radian_t{m_observer.Xhat(2)}};
}

void frc::MecanumDrivePoseEstimator::AddVisionMeasurement(
    const Pose2d& visionRobotPose, units::second_t timestamp) {
  if (auto sample = m_poseBuffer.Sample(timestamp)) {
    m_visionCorrect(Vectord<7>::Zero(),
                    PoseTo3dVector(GetEstimatedPosition().TransformBy(
                        visionRobotPose - sample.value())));
  }
}

Pose2d frc::MecanumDrivePoseEstimator::Update(
    const Rotation2d& gyroAngle, const MecanumDriveWheelSpeeds& wheelSpeeds,
    const MecanumDriveWheelPositions& wheelPositions) {
  return UpdateWithTime(units::microsecond_t(wpi::Now()), gyroAngle,
                        wheelSpeeds, wheelPositions);
}

Pose2d frc::MecanumDrivePoseEstimator::UpdateWithTime(
    units::second_t currentTime, const Rotation2d& gyroAngle,
    const MecanumDriveWheelSpeeds& wheelSpeeds,
    const MecanumDriveWheelPositions& wheelPositions) {
  auto dt = m_prevTime >= 0_s ? currentTime - m_prevTime : m_nominalDt;
  m_prevTime = currentTime;

  auto angle = gyroAngle + m_gyroOffset;
  auto omega = (angle - m_previousAngle).Radians() / dt;

  auto chassisSpeeds = m_kinematics.ToChassisSpeeds(wheelSpeeds);
  auto fieldRelativeVelocities =
      Translation2d{chassisSpeeds.vx * 1_s, chassisSpeeds.vy * 1_s}.RotateBy(
          angle);

  Vectord<7> u{fieldRelativeVelocities.X().value(),
               fieldRelativeVelocities.Y().value(),
               omega.value(),
               wheelSpeeds.frontLeft.value(),
               wheelSpeeds.frontRight.value(),
               wheelSpeeds.rearLeft.value(),
               wheelSpeeds.rearRight.value()};

  Vectord<5> localY{angle.Radians().value(), wheelPositions.frontLeft.value(),
                    wheelPositions.frontRight.value(),
                    wheelPositions.rearLeft.value(),
                    wheelPositions.rearRight.value()};
  m_previousAngle = angle;

  m_poseBuffer.AddSample(currentTime, GetEstimatedPosition());

  m_observer.Predict(u, dt);
  m_observer.Correct(u, localY);

  return GetEstimatedPosition();
}
