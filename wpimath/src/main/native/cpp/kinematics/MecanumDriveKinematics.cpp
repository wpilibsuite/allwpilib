// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/MecanumDriveKinematics.h"

using namespace frc;

MecanumDriveWheelSpeeds MecanumDriveKinematics::ToWheelSpeeds(
    const ChassisSpeeds& chassisSpeeds,
    const Translation2d& centerOfRotation) const {
  // We have a new center of rotation. We need to compute the matrix again.
  if (centerOfRotation != m_previousCoR) {
    auto fl = m_frontLeftWheel - centerOfRotation;
    auto fr = m_frontRightWheel - centerOfRotation;
    auto rl = m_rearLeftWheel - centerOfRotation;
    auto rr = m_rearRightWheel - centerOfRotation;

    SetInverseKinematics(fl, fr, rl, rr);

    m_previousCoR = centerOfRotation;
  }

  Eigen::Vector3d chassisSpeedsVector{chassisSpeeds.vx.to<double>(),
                                      chassisSpeeds.vy.to<double>(),
                                      chassisSpeeds.omega.to<double>()};

  Eigen::Vector<double, 4> wheelsVector =
      m_inverseKinematics * chassisSpeedsVector;

  MecanumDriveWheelSpeeds wheelSpeeds;
  wheelSpeeds.frontLeft = units::meters_per_second_t{wheelsVector(0)};
  wheelSpeeds.frontRight = units::meters_per_second_t{wheelsVector(1)};
  wheelSpeeds.rearLeft = units::meters_per_second_t{wheelsVector(2)};
  wheelSpeeds.rearRight = units::meters_per_second_t{wheelsVector(3)};
  return wheelSpeeds;
}

ChassisSpeeds MecanumDriveKinematics::ToChassisSpeeds(
    const MecanumDriveWheelSpeeds& wheelSpeeds) const {
  Eigen::Vector<double, 4> wheelSpeedsVector{
      wheelSpeeds.frontLeft.to<double>(), wheelSpeeds.frontRight.to<double>(),
      wheelSpeeds.rearLeft.to<double>(), wheelSpeeds.rearRight.to<double>()};

  Eigen::Vector3d chassisSpeedsVector =
      m_forwardKinematics.solve(wheelSpeedsVector);

  return {units::meters_per_second_t{chassisSpeedsVector(0)},  // NOLINT
          units::meters_per_second_t{chassisSpeedsVector(1)},
          units::radians_per_second_t{chassisSpeedsVector(2)}};
}

void MecanumDriveKinematics::SetInverseKinematics(Translation2d fl,
                                                  Translation2d fr,
                                                  Translation2d rl,
                                                  Translation2d rr) const {
  m_inverseKinematics = Eigen::Matrix<double, 4, 3>{
      {1, -1, (-(fl.X() + fl.Y())).template to<double>()},
      {1, 1, (fr.X() - fr.Y()).template to<double>()},
      {1, 1, (rl.X() - rl.Y()).template to<double>()},
      {1, -1, (-(rr.X() + rr.Y())).template to<double>()}};
}
