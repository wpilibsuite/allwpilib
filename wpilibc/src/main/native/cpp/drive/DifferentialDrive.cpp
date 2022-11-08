// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/drive/DifferentialDrive.h"

#include <algorithm>
#include <cmath>

#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/MathUtil.h"
#include "frc/motorcontrol/MotorController.h"

using namespace frc;

DifferentialDrive::DifferentialDrive(MotorController& leftMotor,
                                     MotorController& rightMotor)
    : m_leftMotor(&leftMotor), m_rightMotor(&rightMotor) {
  wpi::SendableRegistry::AddChild(this, m_leftMotor);
  wpi::SendableRegistry::AddChild(this, m_rightMotor);
  static int instances = 0;
  ++instances;
  wpi::SendableRegistry::AddLW(this, "DifferentialDrive", instances);
}

void DifferentialDrive::ArcadeDrive(double xSpeed, double zRotation,
                                    bool squareInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_DifferentialArcade, 2);
    reported = true;
  }

  xSpeed = ApplyDeadband(xSpeed, m_deadband);
  zRotation = ApplyDeadband(zRotation, m_deadband);

  auto [left, right] = ArcadeDriveIK(xSpeed, zRotation, squareInputs);

  m_leftMotor->Set(left);
  m_rightMotor->Set(right);

  Feed();
}

void DifferentialDrive::CurvatureDrive(double xSpeed, double zRotation,
                                       bool allowTurnInPlace) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_DifferentialCurvature, 2);
    reported = true;
  }

  xSpeed = ApplyDeadband(xSpeed, m_deadband);
  zRotation = ApplyDeadband(zRotation, m_deadband);

  auto [left, right] = CurvatureDriveIK(xSpeed, zRotation, allowTurnInPlace);

  m_leftMotor->Set(left * m_maxOutput);
  m_rightMotor->Set(right * m_maxOutput);

  Feed();
}

void DifferentialDrive::TankDrive(double leftSpeed, double rightSpeed,
                                  bool squareInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_DifferentialTank, 2);
    reported = true;
  }

  leftSpeed = ApplyDeadband(leftSpeed, m_deadband);
  rightSpeed = ApplyDeadband(rightSpeed, m_deadband);

  auto [left, right] = TankDriveIK(leftSpeed, rightSpeed, squareInputs);

  m_leftMotor->Set(left * m_maxOutput);
  m_rightMotor->Set(right * m_maxOutput);

  Feed();
}

DifferentialDrive::WheelSpeeds DifferentialDrive::ArcadeDriveIK(
    double xSpeed, double zRotation, bool squareInputs) {
  xSpeed = std::clamp(xSpeed, -1.0, 1.0);
  zRotation = std::clamp(zRotation, -1.0, 1.0);

  // Square the inputs (while preserving the sign) to increase fine control
  // while permitting full power.
  if (squareInputs) {
    xSpeed = std::copysign(xSpeed * xSpeed, xSpeed);
    zRotation = std::copysign(zRotation * zRotation, zRotation);
  }

  double leftSpeed = xSpeed - zRotation;
  double rightSpeed = xSpeed + zRotation;

  // Find the maximum possible value of (throttle + turn) along the vector that
  // the joystick is pointing, then desaturate the wheel speeds
  double greaterInput = std::max(std::abs(xSpeed), std::abs(zRotation));
  double lesserInput = std::min(std::abs(xSpeed), std::abs(zRotation));
  if (greaterInput == 0.0) {
    return {0.0, 0.0};
  }
  double saturatedInput = (greaterInput + lesserInput) / greaterInput;
  leftSpeed /= saturatedInput;
  rightSpeed /= saturatedInput;

  return {leftSpeed, rightSpeed};
}

DifferentialDrive::WheelSpeeds DifferentialDrive::CurvatureDriveIK(
    double xSpeed, double zRotation, bool allowTurnInPlace) {
  xSpeed = std::clamp(xSpeed, -1.0, 1.0);
  zRotation = std::clamp(zRotation, -1.0, 1.0);

  double leftSpeed = 0.0;
  double rightSpeed = 0.0;

  if (allowTurnInPlace) {
    leftSpeed = xSpeed - zRotation;
    rightSpeed = xSpeed + zRotation;
  } else {
    leftSpeed = xSpeed - std::abs(xSpeed) * zRotation;
    rightSpeed = xSpeed + std::abs(xSpeed) * zRotation;
  }

  // Desaturate wheel speeds
  double maxMagnitude = std::max(std::abs(leftSpeed), std::abs(rightSpeed));
  if (maxMagnitude > 1.0) {
    leftSpeed /= maxMagnitude;
    rightSpeed /= maxMagnitude;
  }

  return {leftSpeed, rightSpeed};
}

DifferentialDrive::WheelSpeeds DifferentialDrive::TankDriveIK(
    double leftSpeed, double rightSpeed, bool squareInputs) {
  leftSpeed = std::clamp(leftSpeed, -1.0, 1.0);
  rightSpeed = std::clamp(rightSpeed, -1.0, 1.0);

  // Square the inputs (while preserving the sign) to increase fine control
  // while permitting full power.
  if (squareInputs) {
    leftSpeed = std::copysign(leftSpeed * leftSpeed, leftSpeed);
    rightSpeed = std::copysign(rightSpeed * rightSpeed, rightSpeed);
  }

  return {leftSpeed, rightSpeed};
}

void DifferentialDrive::StopMotor() {
  m_leftMotor->StopMotor();
  m_rightMotor->StopMotor();
  Feed();
}

std::string DifferentialDrive::GetDescription() const {
  return "DifferentialDrive";
}

void DifferentialDrive::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("DifferentialDrive");
  builder.SetActuator(true);
  builder.SetSafeState([=, this] { StopMotor(); });
  builder.AddDoubleProperty(
      "Left Motor Speed", [=, this] { return m_leftMotor->Get(); },
      [=, this](double value) { m_leftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Right Motor Speed", [=, this] { return m_rightMotor->Get(); },
      [=, this](double value) { m_rightMotor->Set(value); });
}
