// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/drive/HDrive.h"

#include <algorithm>
#include <cmath>

#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/MathUtil.h"
#include "frc/motorcontrol/MotorController.h"

using namespace frc;

HDrive::HDrive(MotorController& leftMotor, MotorController& rightMotor,
               MotorController& lateralMotor)
    : m_leftMotor(&leftMotor),
      m_rightMotor(&rightMotor),
      m_lateralMotor(&lateralMotor) {
  wpi::SendableRegistry::AddChild(this, m_leftMotor);
  wpi::SendableRegistry::AddChild(this, m_rightMotor);
  wpi::SendableRegistry::AddChild(this, m_lateralMotor);
  static int instances = 0;
  ++instances;
  wpi::SendableRegistry::AddLW(this, "HDrive", instances);
}

void HDrive::ArcadeDrive(double xSpeed, double zRotation, double ySpeed,
                         bool squareInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_HDriveArcade, 3);
    reported = true;
  }

  xSpeed = ApplyDeadband(xSpeed, m_deadband);
  zRotation = ApplyDeadband(zRotation, m_deadband);
  ySpeed = ApplyDeadband(ySpeed, m_deadband);

  auto [left, right, lateral] =
      ArcadeDriveIK(xSpeed, zRotation, ySpeed, squareInputs);

  m_leftMotor->Set(left);
  m_rightMotor->Set(right);
  m_lateralMotor->Set(lateral);

  Feed();
}

void HDrive::TankDrive(double leftSpeed, double rightSpeed, double lateralSpeed,
                       bool squareInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_HDriveTank, 3);
    reported = true;
  }

  leftSpeed = ApplyDeadband(leftSpeed, m_deadband);
  rightSpeed = ApplyDeadband(rightSpeed, m_deadband);
  lateralSpeed = ApplyDeadband(lateralSpeed, m_deadband);

  auto [left, right, lateral] =
      TankDriveIK(leftSpeed, rightSpeed, squareInputs);

  m_leftMotor->Set(left * m_maxOutput);
  m_rightMotor->Set(right * m_maxOutput);
  m_lateralMotor->Set(lateral * m_maxOutput);

  Feed();
}

HDrive::WheelSpeeds HDrive::ArcadeDriveIK(double xSpeed, double zRotation,
                                          double ySpeed, bool squareInputs) {
  xSpeed = std::clamp(xSpeed, -1.0, 1.0);
  zRotation = std::clamp(zRotation, -1.0, 1.0);
  ySpeed = std::clamp(ySpeed, -1.0, 1.0);

  // Square the inputs (while preserving the sign) to increase fine control
  // while permitting full power.
  if (squareInputs) {
    xSpeed = std::copysign(xSpeed * xSpeed, xSpeed);
    zRotation = std::copysign(zRotation * zRotation, zRotation);
    ySpeed = std::copysign(ySpeed * ySpeed, ySpeed);
  }

  double leftSpeed = xSpeed - zRotation;
  double rightSpeed = xSpeed + zRotation;
  double lateralSpeed = ySpeed;

  // Find the maximum possible value of (throttle + turn) along the vector that
  // the joystick is pointing, then desaturate the wheel speeds
  double greaterInput = (std::max)(std::abs(xSpeed), std::abs(zRotation));
  double lesserInput = (std::min)(std::abs(xSpeed), std::abs(zRotation));
  if (greaterInput == 0.0) {
    return {0.0, 0.0};
  }
  double saturatedInput = (greaterInput + lesserInput) / greaterInput;
  leftSpeed /= saturatedInput;
  rightSpeed /= saturatedInput;

  return {leftSpeed, rightSpeed, lateralSpeed};
}

HDrive::WheelSpeeds HDrive::TankDriveIK(double leftSpeed, double rightSpeed,
                                        double lateralSpeed,
                                        bool squareInputs) {
  leftSpeed = std::clamp(leftSpeed, -1.0, 1.0);
  rightSpeed = std::clamp(rightSpeed, -1.0, 1.0);
  lateralSpeed = std::clamp(lateralSpeed, -1.0, 1.0);

  // Square the inputs (while preserving the sign) to increase fine control
  // while permitting full power.
  if (squareInputs) {
    leftSpeed = std::copysign(leftSpeed * leftSpeed, leftSpeed);
    rightSpeed = std::copysign(rightSpeed * rightSpeed, rightSpeed);
    lateralSpeed = std::copysign(lateralSpeed * lateralSpeed, lateralSpeed);
  }

  return {leftSpeed, rightSpeed, lateralSpeed};
}

void HDrive::StopMotor() {
  m_leftMotor->StopMotor();
  m_rightMotor->StopMotor();
  m_lateralMotor->StopMotor();
  Feed();
}

std::string HDrive::GetDescription() const {
  return "HDrive";
}

void HDrive::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("HDrive");
  builder.SetActuator(true);
  builder.SetSafeState([=, this] { StopMotor(); });
  builder.AddDoubleProperty(
      "Left Motor Speed", [=, this] { return m_leftMotor->Get(); },
      [=, this](double value) { m_leftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Right Motor Speed", [=, this] { return m_rightMotor->Get(); },
      [=, this](double value) { m_rightMotor->Set(value); });
  builder.AddDoubleProperty(
      "Lateral Motor Speed", [=, this] { return m_lateralMotor->Get(); },
      [=, this](double value) { m_lateralMotor->Set(value); });
}
