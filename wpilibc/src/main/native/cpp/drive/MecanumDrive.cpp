// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/drive/MecanumDrive.h"

#include <algorithm>
#include <cmath>

#include <hal/FRCUsageReporting.h>
#include <wpi/math>

#include "frc/SpeedController.h"
#include "frc/drive/Vector2d.h"
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

MecanumDrive::MecanumDrive(SpeedController& frontLeftMotor,
                           SpeedController& rearLeftMotor,
                           SpeedController& frontRightMotor,
                           SpeedController& rearRightMotor)
    : m_frontLeftMotor(&frontLeftMotor),
      m_rearLeftMotor(&rearLeftMotor),
      m_frontRightMotor(&frontRightMotor),
      m_rearRightMotor(&rearRightMotor) {
  auto& registry = SendableRegistry::GetInstance();
  registry.AddChild(this, m_frontLeftMotor);
  registry.AddChild(this, m_rearLeftMotor);
  registry.AddChild(this, m_frontRightMotor);
  registry.AddChild(this, m_rearRightMotor);
  static int instances = 0;
  ++instances;
  registry.AddLW(this, "MecanumDrive", instances);
}

void MecanumDrive::DriveCartesian(double ySpeed, double xSpeed,
                                  double zRotation, double gyroAngle) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_MecanumCartesian, 4);
    reported = true;
  }

  ySpeed = ApplyDeadband(ySpeed, m_deadband);
  xSpeed = ApplyDeadband(xSpeed, m_deadband);

  auto [frontLeft, frontRight, rearLeft, rearRight] =
      DriveCartesianIK(ySpeed, xSpeed, zRotation, gyroAngle);

  m_frontLeftMotor->Set(frontLeft * m_maxOutput);
  m_frontRightMotor->Set(frontRight * m_maxOutput);
  m_rearLeftMotor->Set(rearLeft * m_maxOutput);
  m_rearRightMotor->Set(rearRight * m_maxOutput);

  Feed();
}

void MecanumDrive::DrivePolar(double magnitude, double angle,
                              double zRotation) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_MecanumPolar, 4);
    reported = true;
  }

  DriveCartesian(magnitude * std::cos(angle * (wpi::math::pi / 180.0)),
                 magnitude * std::sin(angle * (wpi::math::pi / 180.0)),
                 zRotation, 0.0);
}

void MecanumDrive::StopMotor() {
  m_frontLeftMotor->StopMotor();
  m_frontRightMotor->StopMotor();
  m_rearLeftMotor->StopMotor();
  m_rearRightMotor->StopMotor();
  Feed();
}

MecanumDrive::WheelSpeeds MecanumDrive::DriveCartesianIK(double ySpeed,
                                                         double xSpeed,
                                                         double zRotation,
                                                         double gyroAngle) {
  ySpeed = std::clamp(ySpeed, -1.0, 1.0);
  xSpeed = std::clamp(xSpeed, -1.0, 1.0);

  // Compensate for gyro angle.
  Vector2d input{ySpeed, xSpeed};
  input.Rotate(-gyroAngle);

  double wheelSpeeds[4];
  wheelSpeeds[kFrontLeft] = input.x + input.y + zRotation;
  wheelSpeeds[kFrontRight] = input.x - input.y - zRotation;
  wheelSpeeds[kRearLeft] = input.x - input.y + zRotation;
  wheelSpeeds[kRearRight] = input.x + input.y - zRotation;

  Normalize(wheelSpeeds);

  return {wheelSpeeds[kFrontLeft], wheelSpeeds[kFrontRight],
          wheelSpeeds[kRearLeft], wheelSpeeds[kRearRight]};
}

void MecanumDrive::GetDescription(wpi::raw_ostream& desc) const {
  desc << "MecanumDrive";
}

void MecanumDrive::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("MecanumDrive");
  builder.SetActuator(true);
  builder.SetSafeState([=] { StopMotor(); });
  builder.AddDoubleProperty(
      "Front Left Motor Speed", [=]() { return m_frontLeftMotor->Get(); },
      [=](double value) { m_frontLeftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Front Right Motor Speed", [=] { return m_frontRightMotor->Get(); },
      [=](double value) { m_frontRightMotor->Set(value); });
  builder.AddDoubleProperty(
      "Rear Left Motor Speed", [=]() { return m_rearLeftMotor->Get(); },
      [=](double value) { m_rearLeftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Rear Right Motor Speed", [=] { return m_rearRightMotor->Get(); },
      [=](double value) { m_rearRightMotor->Set(value); });
}
