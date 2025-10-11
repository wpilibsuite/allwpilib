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

  Eigen::Vector4d wheelsVector = m_inverseKinematics * chassisSpeedsVector;

  MecanumDriveWheelSpeeds wheelSpeeds;
  wheelSpeeds.frontLeft = units::meters_per_second_t{wheelsVector(0)};
  wheelSpeeds.frontRight = units::meters_per_second_t{wheelsVector(1)};
  wheelSpeeds.rearLeft = units::meters_per_second_t{wheelsVector(2)};
  wheelSpeeds.rearRight = units::meters_per_second_t{wheelsVector(3)};
  return wheelSpeeds;
}

ChassisSpeeds MecanumDriveKinematics::ToChassisSpeeds(
    const MecanumDriveWheelSpeeds& wheelSpeeds) const {
  Eigen::Vector4d wheelSpeedsVector{
      wheelSpeeds.frontLeft.value(), wheelSpeeds.frontRight.value(),
      wheelSpeeds.rearLeft.value(), wheelSpeeds.rearRight.value()};

  Eigen::Vector3d chassisSpeedsVector =
      m_forwardKinematics.solve(wheelSpeedsVector);

  return {units::meters_per_second_t{chassisSpeedsVector(0)},
          units::meters_per_second_t{chassisSpeedsVector(1)},
          units::radians_per_second_t{chassisSpeedsVector(2)}};
}

Twist2d MecanumDriveKinematics::ToTwist2d(
    const MecanumDriveWheelPositions& start,
    const MecanumDriveWheelPositions& end) const {
  Eigen::Vector4d wheelDeltasVector{
      end.frontLeft.value() - start.frontLeft.value(),
      end.frontRight.value() - start.frontRight.value(),
      end.rearLeft.value() - start.rearLeft.value(),
      end.rearRight.value() - start.rearRight.value()};

  Eigen::Vector3d twistVector = m_forwardKinematics.solve(wheelDeltasVector);

  return {units::meter_t{twistVector(0)}, units::meter_t{twistVector(1)},
          units::radian_t{twistVector(2)}};
}

Twist2d MecanumDriveKinematics::ToTwist2d(
    const MecanumDriveWheelPositions& wheelDeltas) const {
  Eigen::Vector4d wheelDeltasVector{
      wheelDeltas.frontLeft.value(), wheelDeltas.frontRight.value(),
      wheelDeltas.rearLeft.value(), wheelDeltas.rearRight.value()};

  Eigen::Vector3d twistVector = m_forwardKinematics.solve(wheelDeltasVector);

  return {units::meter_t{twistVector(0)}, units::meter_t{twistVector(1)},
          units::radian_t{twistVector(2)}};
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

ChassisAccelerations MecanumDriveKinematics::ToChassisAccelerations(
    const MecanumDriveWheelAccelerations& wheelAccelerations) const {
  Eigen::Vector4d wheelAccelerationsVector{
      wheelAccelerations.frontLeft.value(),
      wheelAccelerations.frontRight.value(),
      wheelAccelerations.rearLeft.value(),
      wheelAccelerations.rearRight.value()};

  Eigen::Vector3d chassisAccelerationsVector =
      m_forwardKinematics.solve(wheelAccelerationsVector);

  return {units::meters_per_second_squared_t{chassisAccelerationsVector(0)},
          units::meters_per_second_squared_t{chassisAccelerationsVector(1)},
          units::radians_per_second_squared_t{chassisAccelerationsVector(2)}};
}

MecanumDriveWheelAccelerations MecanumDriveKinematics::ToWheelAccelerations(
    const ChassisAccelerations& chassisAccelerations,
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

  Eigen::Vector3d chassisAccelerationsVector{
      chassisAccelerations.ax.value(), chassisAccelerations.ay.value(),
      chassisAccelerations.alpha.value()};

  Eigen::Vector4d wheelsVector =
      m_inverseKinematics * chassisAccelerationsVector;

  MecanumDriveWheelAccelerations wheelAccelerations;
  wheelAccelerations.frontLeft =
      units::meters_per_second_squared_t{wheelsVector(0)};
  wheelAccelerations.frontRight =
      units::meters_per_second_squared_t{wheelsVector(1)};
  wheelAccelerations.rearLeft =
      units::meters_per_second_squared_t{wheelsVector(2)};
  wheelAccelerations.rearRight =
      units::meters_per_second_squared_t{wheelsVector(3)};
  return wheelAccelerations;
}
