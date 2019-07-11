/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/drive/MecanumDrive.h"

#include <algorithm>
#include <cmath>

#include <hal/HAL.h>

#include "frc/SpeedController.h"
#include "frc/drive/Vector2d.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

constexpr double kPi = 3.14159265358979323846;

MecanumDrive::MecanumDrive(SpeedController& frontLeftMotor,
                           SpeedController& rearLeftMotor,
                           SpeedController& frontRightMotor,
                           SpeedController& rearRightMotor)
    : m_frontLeftMotor(frontLeftMotor),
      m_rearLeftMotor(rearLeftMotor),
      m_frontRightMotor(frontRightMotor),
      m_rearRightMotor(rearRightMotor) {
  AddChild(&m_frontLeftMotor);
  AddChild(&m_rearLeftMotor);
  AddChild(&m_frontRightMotor);
  AddChild(&m_rearRightMotor);
  static int instances = 0;
  ++instances;
  SetName("MecanumDrive", instances);
}

void MecanumDrive::DriveCartesian(double ySpeed, double xSpeed,
                                  double zRotation, double gyroAngle) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 4,
               HALUsageReporting::kRobotDrive2_MecanumCartesian);
    reported = true;
  }

  ySpeed = Limit(ySpeed);
  ySpeed = ApplyDeadband(ySpeed, m_deadband);

  xSpeed = Limit(xSpeed);
  xSpeed = ApplyDeadband(xSpeed, m_deadband);

  // Compensate for gyro angle.
  Vector2d input{ySpeed, xSpeed};
  input.Rotate(-gyroAngle);

  double wheelSpeeds[4];
  wheelSpeeds[kFrontLeft] = input.x + input.y + zRotation;
  wheelSpeeds[kFrontRight] = -input.x + input.y - zRotation;
  wheelSpeeds[kRearLeft] = -input.x + input.y + zRotation;
  wheelSpeeds[kRearRight] = input.x + input.y - zRotation;

  Normalize(wheelSpeeds);

  m_frontLeftMotor.Set(wheelSpeeds[kFrontLeft] * m_maxOutput);
  m_frontRightMotor.Set(wheelSpeeds[kFrontRight] * m_maxOutput *
                        m_rightSideInvertMultiplier);
  m_rearLeftMotor.Set(wheelSpeeds[kRearLeft] * m_maxOutput);
  m_rearRightMotor.Set(wheelSpeeds[kRearRight] * m_maxOutput *
                       m_rightSideInvertMultiplier);

  Feed();
}

void MecanumDrive::DrivePolar(double magnitude, double angle,
                              double zRotation) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 4,
               HALUsageReporting::kRobotDrive2_MecanumPolar);
    reported = true;
  }

  DriveCartesian(magnitude * std::sin(angle * (kPi / 180.0)),
                 magnitude * std::cos(angle * (kPi / 180.0)), zRotation, 0.0);
}

bool MecanumDrive::IsRightSideInverted() const {
  return m_rightSideInvertMultiplier == -1.0;
}

void MecanumDrive::SetRightSideInverted(bool rightSideInverted) {
  m_rightSideInvertMultiplier = rightSideInverted ? -1.0 : 1.0;
}

void MecanumDrive::StopMotor() {
  m_frontLeftMotor.StopMotor();
  m_frontRightMotor.StopMotor();
  m_rearLeftMotor.StopMotor();
  m_rearRightMotor.StopMotor();
  Feed();
}

void MecanumDrive::GetDescription(wpi::raw_ostream& desc) const {
  desc << "MecanumDrive";
}

void MecanumDrive::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("MecanumDrive");
  builder.SetActuator(true);
  builder.SetSafeState([=] { StopMotor(); });
  builder.AddDoubleProperty("Front Left Motor Speed",
                            [=]() { return m_frontLeftMotor.Get(); },
                            [=](double value) { m_frontLeftMotor.Set(value); });
  builder.AddDoubleProperty(
      "Front Right Motor Speed",
      [=]() { return m_frontRightMotor.Get() * m_rightSideInvertMultiplier; },
      [=](double value) {
        m_frontRightMotor.Set(value * m_rightSideInvertMultiplier);
      });
  builder.AddDoubleProperty("Rear Left Motor Speed",
                            [=]() { return m_rearLeftMotor.Get(); },
                            [=](double value) { m_rearLeftMotor.Set(value); });
  builder.AddDoubleProperty(
      "Rear Right Motor Speed",
      [=]() { return m_rearRightMotor.Get() * m_rightSideInvertMultiplier; },
      [=](double value) {
        m_rearRightMotor.Set(value * m_rightSideInvertMultiplier);
      });
}
