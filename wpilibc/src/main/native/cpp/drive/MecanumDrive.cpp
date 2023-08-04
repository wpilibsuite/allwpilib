// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/drive/MecanumDrive.h"

#include <algorithm>

#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/MathUtil.h"
#include "frc/geometry/Translation2d.h"
#include "frc/motorcontrol/MotorController.h"

using namespace frc;

MecanumDrive::MecanumDrive(MotorController& frontLeftMotor,
                           MotorController& rearLeftMotor,
                           MotorController& frontRightMotor,
                           MotorController& rearRightMotor)
    : m_frontLeftMotor(&frontLeftMotor),
      m_rearLeftMotor(&rearLeftMotor),
      m_frontRightMotor(&frontRightMotor),
      m_rearRightMotor(&rearRightMotor) {
  wpi::SendableRegistry::AddChild(this, m_frontLeftMotor);
  wpi::SendableRegistry::AddChild(this, m_rearLeftMotor);
  wpi::SendableRegistry::AddChild(this, m_frontRightMotor);
  wpi::SendableRegistry::AddChild(this, m_rearRightMotor);
  static int instances = 0;
  ++instances;
  wpi::SendableRegistry::AddLW(this, "MecanumDrive", instances);
}

void MecanumDrive::DriveCartesian(double xSpeed, double ySpeed,
                                  double zRotation, Rotation2d gyroAngle) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_MecanumCartesian, 4);
    reported = true;
  }

  xSpeed = ApplyDeadband(xSpeed, m_deadband);
  ySpeed = ApplyDeadband(ySpeed, m_deadband);

  auto [frontLeft, frontRight, rearLeft, rearRight] =
      DriveCartesianIK(xSpeed, ySpeed, zRotation, gyroAngle);

  m_frontLeftMotor->Set(frontLeft * m_maxOutput);
  m_frontRightMotor->Set(frontRight * m_maxOutput);
  m_rearLeftMotor->Set(rearLeft * m_maxOutput);
  m_rearRightMotor->Set(rearRight * m_maxOutput);

  Feed();
}

void MecanumDrive::DrivePolar(double magnitude, Rotation2d angle,
                              double zRotation) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive,
               HALUsageReporting::kRobotDrive2_MecanumPolar, 4);
    reported = true;
  }

  DriveCartesian(magnitude * angle.Cos(), magnitude * angle.Sin(), zRotation,
                 0_rad);
}

void MecanumDrive::StopMotor() {
  m_frontLeftMotor->StopMotor();
  m_frontRightMotor->StopMotor();
  m_rearLeftMotor->StopMotor();
  m_rearRightMotor->StopMotor();
  Feed();
}

MecanumDrive::WheelSpeeds MecanumDrive::DriveCartesianIK(double xSpeed,
                                                         double ySpeed,
                                                         double zRotation,
                                                         Rotation2d gyroAngle) {
  xSpeed = std::clamp(xSpeed, -1.0, 1.0);
  ySpeed = std::clamp(ySpeed, -1.0, 1.0);

  // Compensate for gyro angle.
  auto input =
      Translation2d{units::meter_t{xSpeed}, units::meter_t{ySpeed}}.RotateBy(
          -gyroAngle);

  double wheelSpeeds[4];
  wheelSpeeds[kFrontLeft] = input.X().value() + input.Y().value() + zRotation;
  wheelSpeeds[kFrontRight] = input.X().value() - input.Y().value() - zRotation;
  wheelSpeeds[kRearLeft] = input.X().value() - input.Y().value() + zRotation;
  wheelSpeeds[kRearRight] = input.X().value() + input.Y().value() - zRotation;

  Desaturate(wheelSpeeds);

  return {wheelSpeeds[kFrontLeft], wheelSpeeds[kFrontRight],
          wheelSpeeds[kRearLeft], wheelSpeeds[kRearRight]};
}

std::string MecanumDrive::GetDescription() const {
  return "MecanumDrive";
}

void MecanumDrive::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("MecanumDrive");
  builder.SetActuator(true);
  builder.SetSafeState([=, this] { StopMotor(); });
  builder.AddDoubleProperty(
      "Front Left Motor Speed", [=, this] { return m_frontLeftMotor->Get(); },
      [=, this](double value) { m_frontLeftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Front Right Motor Speed", [=, this] { return m_frontRightMotor->Get(); },
      [=, this](double value) { m_frontRightMotor->Set(value); });
  builder.AddDoubleProperty(
      "Rear Left Motor Speed", [=, this] { return m_rearLeftMotor->Get(); },
      [=, this](double value) { m_rearLeftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Rear Right Motor Speed", [=, this] { return m_rearRightMotor->Get(); },
      [=, this](double value) { m_rearRightMotor->Set(value); });
}
