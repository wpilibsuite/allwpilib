// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/MecanumDrive.hpp"

#include <algorithm>
#include <string>
#include <utility>

#include "wpi/hal/UsageReporting.h"
#include "wpi/hardware/motor/MotorController.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi;

WPI_IGNORE_DEPRECATED

MecanumDrive::MecanumDrive(MotorController& frontLeftMotor,
                           MotorController& rearLeftMotor,
                           MotorController& frontRightMotor,
                           MotorController& rearRightMotor)
    : MecanumDrive{[&](double output) { frontLeftMotor.Set(output); },
                   [&](double output) { rearLeftMotor.Set(output); },
                   [&](double output) { frontRightMotor.Set(output); },
                   [&](double output) { rearRightMotor.Set(output); }} {}

WPI_UNIGNORE_DEPRECATED

MecanumDrive::MecanumDrive(std::function<void(double)> frontLeftMotor,
                           std::function<void(double)> rearLeftMotor,
                           std::function<void(double)> frontRightMotor,
                           std::function<void(double)> rearRightMotor)
    : m_frontLeftMotor{std::move(frontLeftMotor)},
      m_rearLeftMotor{std::move(rearLeftMotor)},
      m_frontRightMotor{std::move(frontRightMotor)},
      m_rearRightMotor{std::move(rearRightMotor)} {}

void MecanumDrive::DriveCartesian(double xSpeed, double ySpeed,
                                  double zRotation,
                                  wpi::math::Rotation2d gyroAngle) {
  if (!reported) {
    HAL_ReportUsage("RobotDrive", "MecanumCartesian");
    reported = true;
  }

  xSpeed = wpi::math::ApplyDeadband(xSpeed, m_deadband);
  ySpeed = wpi::math::ApplyDeadband(ySpeed, m_deadband);

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

void MecanumDrive::DrivePolar(double magnitude, wpi::math::Rotation2d angle,
                              double zRotation) {
  if (!reported) {
    HAL_ReportUsage("RobotDrive", "MecanumPolar");
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

MecanumDrive::WheelSpeeds MecanumDrive::DriveCartesianIK(
    double xSpeed, double ySpeed, double zRotation,
    wpi::math::Rotation2d gyroAngle) {
  xSpeed = std::clamp(xSpeed, -1.0, 1.0);
  ySpeed = std::clamp(ySpeed, -1.0, 1.0);

  // Compensate for gyro angle.
  auto input = wpi::math::Translation2d{
      wpi::units::meter_t{xSpeed},
      wpi::units::meter_t{
          ySpeed}}.RotateBy(-gyroAngle);

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

void MecanumDrive::UpdateTelemetry(wpi::TelemetryTable& table) const {
  table.Log("Front Left Motor Speed", m_frontLeftOutput);
  table.Log("Front Right Motor Speed", m_frontRightOutput);
  table.Log("Rear Left Motor Speed", m_rearLeftOutput);
  table.Log("Rear Right Motor Speed", m_rearRightOutput);
}

std::string_view MecanumDrive::GetTelemetryType() const {
  return "MecanumDrive";
}
