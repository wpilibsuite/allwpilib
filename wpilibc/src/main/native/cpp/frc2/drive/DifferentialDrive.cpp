// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/drive/DifferentialDrive.h"

#include <algorithm>
#include <cmath>

#include <hal/HAL.h>

#include "frc/SpeedController.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc2;

DifferentialDrive::DifferentialDrive(frc::SpeedController& leftMotor,
                                     frc::SpeedController& rightMotor)
    : m_leftMotor(&leftMotor), m_rightMotor(&rightMotor) {
  auto& registry = frc::SendableRegistry::GetInstance();
  registry.AddChild(this, m_leftMotor);
  registry.AddChild(this, m_rightMotor);
  static int instances = 0;
  ++instances;
  registry.AddLW(this, "DifferentialDrive", instances);
}

void DifferentialDrive::ArcadeDrive(double xSpeed, double zRotation,
                                    bool squareInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 2,
               HALUsageReporting::kRobotDrive2_DifferentialArcade);
    reported = true;
  }

  xSpeed = std::clamp(xSpeed, -1.0, 1.0);
  zRotation = std::clamp(zRotation, -1.0, 1.0);

  // Square the inputs (while preserving the sign) to increase fine control
  // while permitting full power.
  if (squareInputs) {
    xSpeed = std::copysign(xSpeed * xSpeed, xSpeed);
    zRotation = std::copysign(zRotation * zRotation, zRotation);
  }

  double leftSpeed = xSpeed + zRotation;
  double rightSpeed = xSpeed - zRotation;

  // Normalize wheel speeds
  double maxMagnitude = std::max(std::abs(leftSpeed), std::abs(rightSpeed));
  if (maxMagnitude > 1.0) {
    leftSpeed /= maxMagnitude;
    rightSpeed /= maxMagnitude;
  }

  m_leftMotor->Set(leftSpeed * m_maxOutput);
  m_rightMotor->Set(rightSpeed * m_maxOutput);
}

void DifferentialDrive::CurvatureDrive(double xSpeed, double zRotation,
                                       bool allowTurnInPlace) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 2,
               HALUsageReporting::kRobotDrive2_DifferentialCurvature);
    reported = true;
  }

  xSpeed = std::clamp(xSpeed, -1.0, 1.0);
  zRotation = std::clamp(zRotation, -1.0, 1.0);

  double leftSpeed = 0.0;
  double rightSpeed = 0.0;

  if (allowTurnInPlace) {
    leftSpeed = xSpeed + zRotation;
    rightSpeed = xSpeed - zRotation;
  } else {
    leftSpeed = xSpeed + std::abs(xSpeed) * zRotation;
    rightSpeed = xSpeed - std::abs(xSpeed) * zRotation;
  }

  // Normalize wheel speeds
  double maxMagnitude = std::max(std::abs(leftSpeed), std::abs(rightSpeed));
  if (maxMagnitude > 1.0) {
    leftSpeed /= maxMagnitude;
    rightSpeed /= maxMagnitude;
  }

  m_leftMotor->Set(leftSpeed * m_maxOutput);
  m_rightMotor->Set(rightSpeed * m_maxOutput);
}

void DifferentialDrive::TankDrive(double leftSpeed, double rightSpeed,
                                  bool squareInputs) {
  static bool reported = false;
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 2,
               HALUsageReporting::kRobotDrive2_DifferentialTank);
    reported = true;
  }

  leftSpeed = std::clamp(leftSpeed, -1.0, 1.0);
  rightSpeed = std::clamp(rightSpeed, -1.0, 1.0);

  // Square the inputs (while preserving the sign) to increase fine control
  // while permitting full power.
  if (squareInputs) {
    leftSpeed = std::copysign(leftSpeed * leftSpeed, leftSpeed);
    rightSpeed = std::copysign(rightSpeed * rightSpeed, rightSpeed);
  }

  m_leftMotor->Set(leftSpeed * m_maxOutput);
  m_rightMotor->Set(rightSpeed * m_maxOutput);
}

void DifferentialDrive::InitSendable(frc::SendableBuilder& builder) {
  builder.SetSmartDashboardType("DifferentialDrive");
  builder.SetActuator(true);
  builder.AddDoubleProperty(
      "Left Motor Speed", [=]() { return m_leftMotor->Get(); },
      [=](double value) { m_leftMotor->Set(value); });
  builder.AddDoubleProperty(
      "Right Motor Speed", [=]() { return m_rightMotor->Get(); },
      [=](double value) { m_rightMotor->Set(value); });
}
