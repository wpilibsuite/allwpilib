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

  Eigen::Vector3d chassisSpeedsVector{chassisSpeeds.vx.value(),
                                      chassisSpeeds.vy.value(),
                                      chassisSpeeds.omega.value()};

  Vectord<4> wheelsVector = m_inverseKinematics * chassisSpeedsVector;

  MecanumDriveWheelSpeeds wheelSpeeds;
  wheelSpeeds.frontLeft = units::meters_per_second_t{wheelsVector(0)};
  wheelSpeeds.frontRight = units::meters_per_second_t{wheelsVector(1)};
  wheelSpeeds.rearLeft = units::meters_per_second_t{wheelsVector(2)};
  wheelSpeeds.rearRight = units::meters_per_second_t{wheelsVector(3)};
  return wheelSpeeds;
}

ChassisSpeeds MecanumDriveKinematics::ToChassisSpeeds(
    const MecanumDriveWheelSpeeds& wheelSpeeds) const {
  Vectord<4> wheelSpeedsVector{
      wheelSpeeds.frontLeft.value(), wheelSpeeds.frontRight.value(),
      wheelSpeeds.rearLeft.value(), wheelSpeeds.rearRight.value()};

  Eigen::Vector3d chassisSpeedsVector =
      m_forwardKinematics.solve(wheelSpeedsVector);

  return {units::meters_per_second_t{chassisSpeedsVector(0)},  // NOLINT
          units::meters_per_second_t{chassisSpeedsVector(1)},
          units::radians_per_second_t{chassisSpeedsVector(2)}};
}

Twist2d MecanumDriveKinematics::ToTwist2d(
    const MecanumDriveWheelPositions& wheelDeltas) const {
  Vectord<4> wheelDeltasVector{
      wheelDeltas.frontLeft.value(), wheelDeltas.frontRight.value(),
      wheelDeltas.rearLeft.value(), wheelDeltas.rearRight.value()};

  Eigen::Vector3d twistVector = m_forwardKinematics.solve(wheelDeltasVector);

  return {units::meter_t{twistVector(0)},  // NOLINT
          units::meter_t{twistVector(1)}, units::radian_t{twistVector(2)}};
}

void MecanumDriveKinematics::SetInverseKinematics(Translation2d fl,
                                                  Translation2d fr,
                                                  Translation2d rl,
                                                  Translation2d rr) const {
  m_inverseKinematics = Matrixd<4, 3>{{1, -1, (-(fl.X() + fl.Y())).value()},
                                      {1, 1, (fr.X() - fr.Y()).value()},
                                      {1, 1, (rl.X() - rl.Y()).value()},
                                      {1, -1, (-(rr.X() + rr.Y())).value()}};
}
