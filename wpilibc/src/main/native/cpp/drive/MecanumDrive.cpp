// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/drive/MecanumDrive.h"

#include <algorithm>
#include <string>
#include <utility>

#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/MathUtil.h"
#include "frc/geometry/Translation2d.h"
#include "frc/motorcontrol/MotorController.h"

using namespace frc;

WPI_IGNORE_DEPRECATED

MecanumDrive::MecanumDrive(MotorController& frontLeftMotor,
                           MotorController& rearLeftMotor,
                           MotorController& frontRightMotor,
                           MotorController& rearRightMotor)
    : MecanumDrive{[&](double output) { frontLeftMotor.Set(output); },
                   [&](double output) { rearLeftMotor.Set(output); },
                   [&](double output) { frontRightMotor.Set(output); },
                   [&](double output) { rearRightMotor.Set(output); }} {
  wpi::SendableRegistry::AddChild(this, &frontLeftMotor);
  wpi::SendableRegistry::AddChild(this, &rearLeftMotor);
  wpi::SendableRegistry::AddChild(this, &frontRightMotor);
  wpi::SendableRegistry::AddChild(this, &rearRightMotor);
}

WPI_UNIGNORE_DEPRECATED

MecanumDrive::MecanumDrive(std::function<void(double)> frontLeftMotor,
                           std::function<void(double)> rearLeftMotor,
                           std::function<void(double)> frontRightMotor,
                           std::function<void(double)> rearRightMotor)
    : m_frontLeftMotor{std::move(frontLeftMotor)},
      m_rearLeftMotor{std::move(rearLeftMotor)},
      m_frontRightMotor{std::move(frontRightMotor)},
      m_rearRightMotor{std::move(rearRightMotor)} {
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

  m_frontLeftOutput = frontLeft * m_maxOutput;
  m_rearLeftOutput = rearLeft * m_maxOutput;
  m_frontRightOutput = frontRight * m_maxOutput;
  m_rearRightOutput = rearRight * m_maxOutput;

  m_frontLeftMotor(m_frontLeftOutput);
  m_frontRightMotor(m_frontRightOutput);
  m_rearLeftMotor(m_rearLeftOutput);
  m_rearRightMotor(m_rearRightOutput);

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
  m_frontLeftOutput = 0.0;
  m_frontRightOutput = 0.0;
  m_rearLeftOutput = 0.0;
  m_rearRightOutput = 0.0;

  m_frontLeftMotor(0.0);
  m_frontRightMotor(0.0);
  m_rearLeftMotor(0.0);
  m_rearRightMotor(0.0);

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
      "Front Left Motor Speed", [&] { return m_frontLeftOutput; },
      m_frontLeftMotor);
  builder.AddDoubleProperty(
      "Front Right Motor Speed", [&] { return m_frontRightOutput; },
      m_frontRightMotor);
  builder.AddDoubleProperty(
      "Rear Left Motor Speed", [&] { return m_rearLeftOutput; },
      m_rearLeftMotor);
  builder.AddDoubleProperty(
      "Rear Right Motor Speed", [&] { return m_rearRightOutput; },
      m_rearRightMotor);
}
