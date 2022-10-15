// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/drive/KilloughDrive.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/MathUtil.h"
#include "frc/motorcontrol/MotorController.h"

using namespace frc;

KilloughDrive::KilloughDrive(MotorController& leftMotor,
                             MotorController& rightMotor,
                             MotorController& backMotor)
    : KilloughDrive(leftMotor, rightMotor, backMotor, kDefaultLeftMotorAngle,
                    kDefaultRightMotorAngle, kDefaultBackMotorAngle) {}

KilloughDrive::KilloughDrive(MotorController& leftMotor,
                             MotorController& rightMotor,
                             MotorController& backMotor, double leftMotorAngle,
                             double rightMotorAngle, double backMotorAngle)
    : m_leftMotor(&leftMotor),
      m_rightMotor(&rightMotor),
      m_backMotor(&backMotor) {
  m_leftVec = {std::cos(leftMotorAngle * (std::numbers::pi / 180.0)),
               std::sin(leftMotorAngle * (std::numbers::pi / 180.0))};
  m_rightVec = {std::cos(rightMotorAngle * (std::numbers::pi / 180.0)),
                std::sin(rightMotorAngle * (std::numbers::pi / 180.0))};
  m_backVec = {std::cos(backMotorAngle * (std::numbers::pi / 180.0)),
               std::sin(backMotorAngle * (std::numbers::pi / 180.0))};
  wpi::SendableRegistry::AddChild(this, m_leftMotor);
  wpi::SendableRegistry::AddChild(this, m_rightMotor);
  wpi::SendableRegistry::AddChild(this, m_backMotor);
  static int instances = 0;
  ++instances;
  wpi::SendableRegistry::AddLW(this, "KilloughDrive", instances);
}

void KilloughDrive::DriveCartesian(double ySpeed, double xSpeed,
                                   double zRotation, double gyroAngle) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_KilloughCartesian, 3);
    reported = true;
  }

  ySpeed = ApplyDeadband(ySpeed, m_deadband);
  xSpeed = ApplyDeadband(xSpeed, m_deadband);

  auto [left, right, back] =
      DriveCartesianIK(ySpeed, xSpeed, zRotation, gyroAngle);

  m_leftMotor->Set(left * m_maxOutput);
  m_rightMotor->Set(right * m_maxOutput);
  m_backMotor->Set(back * m_maxOutput);

  Feed();
}

void KilloughDrive::DrivePolar(double magnitude, double angle,
                               double zRotation) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_KilloughPolar, 3);
    reported = true;
  }

  DriveCartesian(magnitude * std::sin(angle * (std::numbers::pi / 180.0)),
                 magnitude * std::cos(angle * (std::numbers::pi / 180.0)),
                 zRotation, 0.0);
}

KilloughDrive::WheelSpeeds KilloughDrive::DriveCartesianIK(double ySpeed,
                                                           double xSpeed,
                                                           double zRotation,
                                                           double gyroAngle) {
  ySpeed = std::clamp(ySpeed, -1.0, 1.0);
  xSpeed = std::clamp(xSpeed, -1.0, 1.0);

  // Compensate for gyro angle.
  Vector2d input{ySpeed, xSpeed};
  input.Rotate(-gyroAngle);

  double wheelSpeeds[3];
  wheelSpeeds[kLeft] = input.ScalarProject(m_leftVec) + zRotation;
  wheelSpeeds[kRight] = input.ScalarProject(m_rightVec) + zRotation;
  wheelSpeeds[kBack] = input.ScalarProject(m_backVec) + zRotation;

  Desaturate(wheelSpeeds);

  return {wheelSpeeds[kLeft], wheelSpeeds[kRight], wheelSpeeds[kBack]};
}

void KilloughDrive::StopMotor() {
  m_leftMotor->StopMotor();
  m_rightMotor->StopMotor();
  m_backMotor->StopMotor();
  Feed();
}

std::string KilloughDrive::GetDescription() const {
  return "KilloughDrive";
}

void KilloughDrive::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("KilloughDrive");
  builder.SetActuator(true);
  builder.SetSafeState([=, this] { StopMotor(); });
  builder.AddDoubleProperty(
      "Left Motor Speed", [=, this] { return m_leftMotor->Get(); },
      [=, this](double value) { m_leftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Right Motor Speed", [=, this] { return m_rightMotor->Get(); },
      [=, this](double value) { m_rightMotor->Set(value); });
  builder.AddDoubleProperty(
      "Back Motor Speed", [=, this] { return m_backMotor->Get(); },
      [=, this](double value) { m_backMotor->Set(value); });
}
