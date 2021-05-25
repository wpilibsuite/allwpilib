// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/drive/KilloughDrive.h"

#include <algorithm>
#include <cmath>

#include <hal/FRCUsageReporting.h>
#include <wpi/numbers>

#include "frc/SpeedController.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

#if defined(_MSC_VER)
#pragma warning(disable : 4996)  // was declared deprecated
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

KilloughDrive::KilloughDrive(SpeedController& leftMotor,
                             SpeedController& rightMotor,
                             SpeedController& backMotor)
    : KilloughDrive(leftMotor, rightMotor, backMotor, kDefaultLeftMotorAngle,
                    kDefaultRightMotorAngle, kDefaultBackMotorAngle) {}

KilloughDrive::KilloughDrive(SpeedController& leftMotor,
                             SpeedController& rightMotor,
                             SpeedController& backMotor, double leftMotorAngle,
                             double rightMotorAngle, double backMotorAngle)
    : m_leftMotor(&leftMotor),
      m_rightMotor(&rightMotor),
      m_backMotor(&backMotor) {
  m_leftVec = {std::cos(leftMotorAngle * (wpi::numbers::pi / 180.0)),
               std::sin(leftMotorAngle * (wpi::numbers::pi / 180.0))};
  m_rightVec = {std::cos(rightMotorAngle * (wpi::numbers::pi / 180.0)),
                std::sin(rightMotorAngle * (wpi::numbers::pi / 180.0))};
  m_backVec = {std::cos(backMotorAngle * (wpi::numbers::pi / 180.0)),
               std::sin(backMotorAngle * (wpi::numbers::pi / 180.0))};
  auto& registry = SendableRegistry::GetInstance();
  registry.AddChild(this, m_leftMotor);
  registry.AddChild(this, m_rightMotor);
  registry.AddChild(this, m_backMotor);
  static int instances = 0;
  ++instances;
  registry.AddLW(this, "KilloughDrive", instances);
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

  DriveCartesian(magnitude * std::sin(angle * (wpi::numbers::pi / 180.0)),
                 magnitude * std::cos(angle * (wpi::numbers::pi / 180.0)),
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

  Normalize(wheelSpeeds);

  return {wheelSpeeds[kLeft], wheelSpeeds[kRight], wheelSpeeds[kBack]};
}

void KilloughDrive::StopMotor() {
  m_leftMotor->StopMotor();
  m_rightMotor->StopMotor();
  m_backMotor->StopMotor();
  Feed();
}

void KilloughDrive::GetDescription(wpi::raw_ostream& desc) const {
  desc << "KilloughDrive";
}

void KilloughDrive::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("KilloughDrive");
  builder.SetActuator(true);
  builder.SetSafeState([=] { StopMotor(); });
  builder.AddDoubleProperty(
      "Left Motor Speed", [=]() { return m_leftMotor->Get(); },
      [=](double value) { m_leftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Right Motor Speed", [=]() { return m_rightMotor->Get(); },
      [=](double value) { m_rightMotor->Set(value); });
  builder.AddDoubleProperty(
      "Back Motor Speed", [=]() { return m_backMotor->Get(); },
      [=](double value) { m_backMotor->Set(value); });
}
