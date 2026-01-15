// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/DifferentialDrive.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>

#include "wpi/hal/UsageReporting.h"
#include "wpi/hardware/motor/MotorController.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

using namespace wpi;

WPI_IGNORE_DEPRECATED

DifferentialDrive::DifferentialDrive(MotorController& leftMotor,
                                     MotorController& rightMotor)
    : DifferentialDrive{
          [&](double output) { leftMotor.SetDutyCycle(output); },
          [&](double output) { rightMotor.SetDutyCycle(output); }} {
  wpi::util::SendableRegistry::AddChild(this, &leftMotor);
  wpi::util::SendableRegistry::AddChild(this, &rightMotor);
}

WPI_UNIGNORE_DEPRECATED

DifferentialDrive::DifferentialDrive(std::function<void(double)> leftMotor,
                                     std::function<void(double)> rightMotor)
    : m_leftMotor{std::move(leftMotor)}, m_rightMotor{std::move(rightMotor)} {
  static int instances = 0;
  ++instances;
  wpi::util::SendableRegistry::Add(this, "DifferentialDrive", instances);
}

void DifferentialDrive::ArcadeDrive(double xVelocity, double zRotation,
                                    bool squareInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_ReportUsage("RobotDrive", "DifferentialArcade");
    reported = true;
  }

  xVelocity = wpi::math::ApplyDeadband(xVelocity, m_deadband);
  zRotation = wpi::math::ApplyDeadband(zRotation, m_deadband);

  auto [left, right] = ArcadeDriveIK(xVelocity, zRotation, squareInputs);

  m_leftOutput = left * m_maxOutput;
  m_rightOutput = right * m_maxOutput;

  m_leftMotor(m_leftOutput);
  m_rightMotor(m_rightOutput);

  Feed();
}

void DifferentialDrive::CurvatureDrive(double xVelocity, double zRotation,
                                       bool allowTurnInPlace) {
  static bool reported = false;
  if (!reported) {
    HAL_ReportUsage("RobotDrive", "DifferentialCurvature");
    reported = true;
  }

  xVelocity = wpi::math::ApplyDeadband(xVelocity, m_deadband);
  zRotation = wpi::math::ApplyDeadband(zRotation, m_deadband);

  auto [left, right] = CurvatureDriveIK(xVelocity, zRotation, allowTurnInPlace);

  m_leftOutput = left * m_maxOutput;
  m_rightOutput = right * m_maxOutput;

  m_leftMotor(m_leftOutput);
  m_rightMotor(m_rightOutput);

  Feed();
}

void DifferentialDrive::TankDrive(double leftVelocity, double rightVelocity,
                                  bool squareInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_ReportUsage("RobotDrive", "DifferentialTank");
    reported = true;
  }

  leftVelocity = wpi::math::ApplyDeadband(leftVelocity, m_deadband);
  rightVelocity = wpi::math::ApplyDeadband(rightVelocity, m_deadband);

  auto [left, right] = TankDriveIK(leftVelocity, rightVelocity, squareInputs);

  m_leftOutput = left * m_maxOutput;
  m_rightOutput = right * m_maxOutput;

  m_leftMotor(m_leftOutput);
  m_rightMotor(m_rightOutput);

  Feed();
}

DifferentialDrive::WheelVelocities DifferentialDrive::ArcadeDriveIK(
    double xVelocity, double zRotation, bool squareInputs) {
  xVelocity = std::clamp(xVelocity, -1.0, 1.0);
  zRotation = std::clamp(zRotation, -1.0, 1.0);

  // Square the inputs (while preserving the sign) to increase fine control
  // while permitting full power.
  if (squareInputs) {
    xVelocity = wpi::math::CopyDirectionPow(xVelocity, 2);
    zRotation = wpi::math::CopyDirectionPow(zRotation, 2);
  }

  double leftVelocity = xVelocity - zRotation;
  double rightVelocity = xVelocity + zRotation;

  // Find the maximum possible value of (throttle + turn) along the vector that
  // the joystick is pointing, then desaturate the wheel velocities
  double greaterInput = (std::max)(std::abs(xVelocity), std::abs(zRotation));
  double lesserInput = (std::min)(std::abs(xVelocity), std::abs(zRotation));
  if (greaterInput == 0.0) {
    return {0.0, 0.0};
  }
  double saturatedInput = (greaterInput + lesserInput) / greaterInput;
  leftVelocity /= saturatedInput;
  rightVelocity /= saturatedInput;

  return {leftVelocity, rightVelocity};
}

DifferentialDrive::WheelVelocities DifferentialDrive::CurvatureDriveIK(
    double xVelocity, double zRotation, bool allowTurnInPlace) {
  xVelocity = std::clamp(xVelocity, -1.0, 1.0);
  zRotation = std::clamp(zRotation, -1.0, 1.0);

  double leftVelocity = 0.0;
  double rightVelocity = 0.0;

  if (allowTurnInPlace) {
    leftVelocity = xVelocity - zRotation;
    rightVelocity = xVelocity + zRotation;
  } else {
    leftVelocity = xVelocity - std::abs(xVelocity) * zRotation;
    rightVelocity = xVelocity + std::abs(xVelocity) * zRotation;
  }

  // Desaturate wheel velocities
  double maxMagnitude =
      std::max(std::abs(leftVelocity), std::abs(rightVelocity));
  if (maxMagnitude > 1.0) {
    leftVelocity /= maxMagnitude;
    rightVelocity /= maxMagnitude;
  }

  return {leftVelocity, rightVelocity};
}

DifferentialDrive::WheelVelocities DifferentialDrive::TankDriveIK(
    double leftVelocity, double rightVelocity, bool squareInputs) {
  leftVelocity = std::clamp(leftVelocity, -1.0, 1.0);
  rightVelocity = std::clamp(rightVelocity, -1.0, 1.0);

  // Square the inputs (while preserving the sign) to increase fine control
  // while permitting full power.
  if (squareInputs) {
    leftVelocity = wpi::math::CopyDirectionPow(leftVelocity, 2);
    rightVelocity = wpi::math::CopyDirectionPow(rightVelocity, 2);
  }

  return {leftVelocity, rightVelocity};
}

void DifferentialDrive::StopMotor() {
  m_leftOutput = 0.0;
  m_rightOutput = 0.0;

  m_leftMotor(0.0);
  m_rightMotor(0.0);

  Feed();
}

std::string DifferentialDrive::GetDescription() const {
  return "DifferentialDrive";
}

void DifferentialDrive::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("DifferentialDrive");
  builder.SetActuator(true);
  builder.AddDoubleProperty(
      "Left Motor Velocity", [&] { return m_leftOutput; }, m_leftMotor);
  builder.AddDoubleProperty(
      "Right Motor Velocity", [&] { return m_rightOutput; }, m_rightMotor);
}
