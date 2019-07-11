/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/drive/KilloughDrive.h"

#include <algorithm>
#include <cmath>

#include <hal/HAL.h>

#include "frc/SpeedController.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

constexpr double kPi = 3.14159265358979323846;

KilloughDrive::KilloughDrive(SpeedController& leftMotor,
                             SpeedController& rightMotor,
                             SpeedController& backMotor)
    : KilloughDrive(leftMotor, rightMotor, backMotor, kDefaultLeftMotorAngle,
                    kDefaultRightMotorAngle, kDefaultBackMotorAngle) {}

KilloughDrive::KilloughDrive(SpeedController& leftMotor,
                             SpeedController& rightMotor,
                             SpeedController& backMotor, double leftMotorAngle,
                             double rightMotorAngle, double backMotorAngle)
    : m_leftMotor(leftMotor), m_rightMotor(rightMotor), m_backMotor(backMotor) {
  m_leftVec = {std::cos(leftMotorAngle * (kPi / 180.0)),
               std::sin(leftMotorAngle * (kPi / 180.0))};
  m_rightVec = {std::cos(rightMotorAngle * (kPi / 180.0)),
                std::sin(rightMotorAngle * (kPi / 180.0))};
  m_backVec = {std::cos(backMotorAngle * (kPi / 180.0)),
               std::sin(backMotorAngle * (kPi / 180.0))};
  AddChild(&m_leftMotor);
  AddChild(&m_rightMotor);
  AddChild(&m_backMotor);
  static int instances = 0;
  ++instances;
  SetName("KilloughDrive", instances);
}

void KilloughDrive::DriveCartesian(double ySpeed, double xSpeed,
                                   double zRotation, double gyroAngle) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 3,
               HALUsageReporting::kRobotDrive2_KilloughCartesian);
    reported = true;
  }

  ySpeed = Limit(ySpeed);
  ySpeed = ApplyDeadband(ySpeed, m_deadband);

  xSpeed = Limit(xSpeed);
  xSpeed = ApplyDeadband(xSpeed, m_deadband);

  // Compensate for gyro angle.
  Vector2d input{ySpeed, xSpeed};
  input.Rotate(-gyroAngle);

  double wheelSpeeds[3];
  wheelSpeeds[kLeft] = input.ScalarProject(m_leftVec) + zRotation;
  wheelSpeeds[kRight] = input.ScalarProject(m_rightVec) + zRotation;
  wheelSpeeds[kBack] = input.ScalarProject(m_backVec) + zRotation;

  Normalize(wheelSpeeds);

  m_leftMotor.Set(wheelSpeeds[kLeft] * m_maxOutput);
  m_rightMotor.Set(wheelSpeeds[kRight] * m_maxOutput);
  m_backMotor.Set(wheelSpeeds[kBack] * m_maxOutput);

  Feed();
}

void KilloughDrive::DrivePolar(double magnitude, double angle,
                               double zRotation) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 3,
               HALUsageReporting::kRobotDrive2_KilloughPolar);
    reported = true;
  }

  DriveCartesian(magnitude * std::sin(angle * (kPi / 180.0)),
                 magnitude * std::cos(angle * (kPi / 180.0)), zRotation, 0.0);
}

void KilloughDrive::StopMotor() {
  m_leftMotor.StopMotor();
  m_rightMotor.StopMotor();
  m_backMotor.StopMotor();
  Feed();
}

void KilloughDrive::GetDescription(wpi::raw_ostream& desc) const {
  desc << "KilloughDrive";
}

void KilloughDrive::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("KilloughDrive");
  builder.SetActuator(true);
  builder.SetSafeState([=] { StopMotor(); });
  builder.AddDoubleProperty("Left Motor Speed",
                            [=]() { return m_leftMotor.Get(); },
                            [=](double value) { m_leftMotor.Set(value); });
  builder.AddDoubleProperty("Right Motor Speed",
                            [=]() { return m_rightMotor.Get(); },
                            [=](double value) { m_rightMotor.Set(value); });
  builder.AddDoubleProperty("Back Motor Speed",
                            [=]() { return m_backMotor.Get(); },
                            [=](double value) { m_backMotor.Set(value); });
}
