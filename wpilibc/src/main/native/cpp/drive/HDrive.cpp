/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/drive/HDrive.h"

#include <algorithm>
#include <cmath>

#include <hal/HAL.h>
#include <wpi/math>

#include "frc/SpeedController.h"
#include "frc/drive/Vector2d.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

HDrive::HDrive(SpeedController& leftMotor, SpeedController& rightMotor,
               SpeedController& strafeMotor, double trackWidth,
               double strafeWheelDistance)
    : m_leftMotor(leftMotor),
      m_rightMotor(rightMotor),
      m_strafeMotor(strafeMotor) {
  AddChild(&m_leftMotor);
  AddChild(&m_rightMotor);
  AddChild(&m_strafeMotor);
  static int instances = 0;
  m_strafeRotationFactor =
      trackWidth == 0.0 ? 0.0 : strafeWheelDistance / (trackWidth / 2.0);
  ++instances;
  SetName("HDrive", instances);
}

void HDrive::DriveCartesian(double ySpeed, double xSpeed, double zRotation,
                            double gyroAngle) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 4,
               HALUsageReporting::kRobotDrive2_HDriveCartesian);
    reported = true;
  }

  ySpeed = Limit(ySpeed);
  ySpeed = ApplyDeadband(ySpeed, m_deadband);

  xSpeed = Limit(xSpeed);
  xSpeed = ApplyDeadband(xSpeed, m_deadband);

  zRotation = Limit(zRotation);
  zRotation = ApplyDeadband(zRotation, m_deadband);

  // Compensate for gyro angle.
  Vector2d input{ySpeed, xSpeed};
  input.Rotate(-gyroAngle);

  double wheelSpeeds[3];
  wheelSpeeds[kLeft] = input.x + zRotation + m_strafeRotationFactor * input.y;
  wheelSpeeds[kRight] = input.x - zRotation - m_strafeRotationFactor * input.y;
  wheelSpeeds[kBack] = input.y - m_strafeRotationFactor * zRotation;

  Normalize(wheelSpeeds);

  m_leftMotor.Set(wheelSpeeds[kLeft] * m_maxOutput);
  m_rightMotor.Set(wheelSpeeds[kRight] * m_maxOutput);
  m_strafeMotor.Set(wheelSpeeds[kBack] * m_maxOutput);

  Feed();
}

void HDrive::DrivePolar(double magnitude, double angle, double zRotation) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 4,
               HALUsageReporting::kRobotDrive2_HDrivePolar);
    reported = true;
  }

  DriveCartesian(magnitude * std::sin(angle * (wpi::math::pi / 180.0)),
                 magnitude * std::cos(angle * (wpi::math::pi / 180.0)),
                 zRotation, 0.0);
}

void HDrive::StopMotor() {
  m_leftMotor.StopMotor();
  m_rightMotor.StopMotor();
  m_strafeMotor.StopMotor();
  Feed();
}

void HDrive::GetDescription(wpi::raw_ostream& desc) const { desc << "HDrive"; }

void HDrive::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("HDrive");
  builder.SetActuator(true);
  builder.SetSafeState([=] { StopMotor(); });
  builder.AddDoubleProperty("Left Motor Speed",
                            [=]() { return m_leftMotor.Get(); },
                            [=](double value) { m_leftMotor.Set(value); });
  builder.AddDoubleProperty("Right Motor Speed",
                            [=]() { return m_rightMotor.Get(); },
                            [=](double value) { m_rightMotor.Set(value); });
  builder.AddDoubleProperty("Strafe Motor Speed",
                            [=]() { return m_strafeMotor.Get(); },
                            [=](double value) { m_strafeMotor.Set(value); });
}
