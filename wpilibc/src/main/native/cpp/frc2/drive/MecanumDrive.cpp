// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/drive/MecanumDrive.h"

#include <algorithm>
#include <cmath>

#include <hal/HAL.h>
#include <wpi/math>

#include "frc/SpeedController.h"
#include "frc/drive/Vector2d.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc2;

MecanumDrive::MecanumDrive(frc::SpeedController& frontLeftMotor,
                           frc::SpeedController& rearLeftMotor,
                           frc::SpeedController& frontRightMotor,
                           frc::SpeedController& rearRightMotor)
    : m_frontLeftMotor(&frontLeftMotor),
      m_rearLeftMotor(&rearLeftMotor),
      m_frontRightMotor(&frontRightMotor),
      m_rearRightMotor(&rearRightMotor) {
  auto& registry = frc::SendableRegistry::GetInstance();
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
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 4,
               HALUsageReporting::kRobotDrive2_MecanumCartesian);
    reported = true;
  }

  ySpeed = std::clamp(ySpeed, -1.0, 1.0);
  xSpeed = std::clamp(xSpeed, -1.0, 1.0);

  // Compensate for gyro angle.
  frc::Vector2d input{ySpeed, xSpeed};
  input.Rotate(-gyroAngle);

  double wheelSpeeds[4];
  wheelSpeeds[kFrontLeft] = input.x + input.y + zRotation;
  wheelSpeeds[kFrontRight] = input.x - input.y - zRotation;
  wheelSpeeds[kRearLeft] = input.x - input.y + zRotation;
  wheelSpeeds[kRearRight] = input.x + input.y - zRotation;

  Normalize(wheelSpeeds);

  m_frontLeftMotor->Set(wheelSpeeds[kFrontLeft] * m_maxOutput);
  m_frontRightMotor->Set(wheelSpeeds[kFrontRight] * m_maxOutput);
  m_rearLeftMotor->Set(wheelSpeeds[kRearLeft] * m_maxOutput);
  m_rearRightMotor->Set(wheelSpeeds[kRearRight] * m_maxOutput);
}

void MecanumDrive::DrivePolar(double magnitude, double angle,
                              double zRotation) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 4,
               HALUsageReporting::kRobotDrive2_MecanumPolar);
    reported = true;
  }

  DriveCartesian(magnitude * std::cos(angle * (wpi::math::pi / 180.0)),
                 magnitude * std::sin(angle * (wpi::math::pi / 180.0)),
                 zRotation, 0.0);
}

void MecanumDrive::InitSendable(frc::SendableBuilder& builder) {
  builder.SetSmartDashboardType("MecanumDrive");
  builder.SetActuator(true);
  builder.AddDoubleProperty(
      "Front Left Motor Speed", [=]() { return m_frontLeftMotor->Get(); },
      [=](double value) { m_frontLeftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Front Right Motor Speed", [=]() { return m_frontRightMotor->Get(); },
      [=](double value) { m_frontRightMotor->Set(value); });
  builder.AddDoubleProperty(
      "Rear Left Motor Speed", [=]() { return m_rearLeftMotor->Get(); },
      [=](double value) { m_rearLeftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Rear Right Motor Speed", [=]() { return m_rearRightMotor->Get(); },
      [=](double value) { m_rearRightMotor->Set(value); });
}
