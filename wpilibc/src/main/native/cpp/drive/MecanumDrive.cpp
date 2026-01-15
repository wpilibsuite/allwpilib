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
#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

using namespace wpi;

WPI_IGNORE_DEPRECATED

MecanumDrive::MecanumDrive(MotorController& frontLeftMotor,
                           MotorController& rearLeftMotor,
                           MotorController& frontRightMotor,
                           MotorController& rearRightMotor)
    : MecanumDrive{
          [&](double output) { frontLeftMotor.SetDutyCycle(output); },
          [&](double output) { rearLeftMotor.SetDutyCycle(output); },
          [&](double output) { frontRightMotor.SetDutyCycle(output); },
          [&](double output) { rearRightMotor.SetDutyCycle(output); }} {
  wpi::util::SendableRegistry::AddChild(this, &frontLeftMotor);
  wpi::util::SendableRegistry::AddChild(this, &rearLeftMotor);
  wpi::util::SendableRegistry::AddChild(this, &frontRightMotor);
  wpi::util::SendableRegistry::AddChild(this, &rearRightMotor);
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
  wpi::util::SendableRegistry::Add(this, "MecanumDrive", instances);
}

void MecanumDrive::DriveCartesian(double xVelocity, double yVelocity,
                                  double zRotation,
                                  wpi::math::Rotation2d gyroAngle) {
  if (!reported) {
    HAL_ReportUsage("RobotDrive", "MecanumCartesian");
    reported = true;
  }

  xVelocity = wpi::math::ApplyDeadband(xVelocity, m_deadband);
  yVelocity = wpi::math::ApplyDeadband(yVelocity, m_deadband);

  auto [frontLeft, frontRight, rearLeft, rearRight] =
      DriveCartesianIK(xVelocity, yVelocity, zRotation, gyroAngle);

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

MecanumDrive::WheelVelocities MecanumDrive::DriveCartesianIK(
    double xVelocity, double yVelocity, double zRotation,
    wpi::math::Rotation2d gyroAngle) {
  xVelocity = std::clamp(xVelocity, -1.0, 1.0);
  yVelocity = std::clamp(yVelocity, -1.0, 1.0);

  // Compensate for gyro angle.
  auto input = wpi::math::Translation2d{wpi::units::meter_t{xVelocity},
                                        wpi::units::meter_t{yVelocity}}
                   .RotateBy(-gyroAngle);

  double wheelVelocities[4];
  wheelVelocities[kFrontLeft] =
      input.X().value() + input.Y().value() + zRotation;
  wheelVelocities[kFrontRight] =
      input.X().value() - input.Y().value() - zRotation;
  wheelVelocities[kRearLeft] =
      input.X().value() - input.Y().value() + zRotation;
  wheelVelocities[kRearRight] =
      input.X().value() + input.Y().value() - zRotation;

  Desaturate(wheelVelocities);

  return {wheelVelocities[kFrontLeft], wheelVelocities[kFrontRight],
          wheelVelocities[kRearLeft], wheelVelocities[kRearRight]};
}

std::string MecanumDrive::GetDescription() const {
  return "MecanumDrive";
}

void MecanumDrive::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("MecanumDrive");
  builder.SetActuator(true);
  builder.AddDoubleProperty(
      "Front Left Motor Velocity", [&] { return m_frontLeftOutput; },
      m_frontLeftMotor);
  builder.AddDoubleProperty(
      "Front Right Motor Velocity", [&] { return m_frontRightOutput; },
      m_frontRightMotor);
  builder.AddDoubleProperty(
      "Rear Left Motor Velocity", [&] { return m_rearLeftOutput; },
      m_rearLeftMotor);
  builder.AddDoubleProperty(
      "Rear Right Motor Velocity", [&] { return m_rearRightOutput; },
      m_rearRightMotor);
}
