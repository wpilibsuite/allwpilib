/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Drive/MecanumDrive.h"

#include <algorithm>
#include <cmath>

#include <HAL/HAL.h>

#include "Drive/Vector2d.h"
#include "SmartDashboard/SendableBuilder.h"
#include "SpeedController.h"

using namespace frc;

constexpr double kPi = 3.14159265358979323846;

/**
 * Construct a MecanumDrive.
 *
 * If a motor needs to be inverted, do so before passing it in.
 */
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

/**
 * Drive method for Mecanum platform.
 *
 * Angles are measured clockwise from the positive X axis. The robot's speed is
 * independent from its angle or rotation rate.
 *
 * @param ySpeed    The robot's speed along the Y axis [-1.0..1.0]. Right is
 *                  positive.
 * @param xSpeed    The robot's speed along the X axis [-1.0..1.0]. Forward is
 *                  positive.
 * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0].
 *                  Clockwise is positive.
 * @param gyroAngle The current angle reading from the gyro in degrees around
 *                  the Z axis. Use this to implement field-oriented controls.
 */
void MecanumDrive::DriveCartesian(double ySpeed, double xSpeed,
                                  double zRotation, double gyroAngle) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 4,
               HALUsageReporting::kRobotDrive_MecanumCartesian);
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
  wheelSpeeds[kFrontRight] = input.x - input.y + zRotation;
  wheelSpeeds[kRearLeft] = -input.x + input.y + zRotation;
  wheelSpeeds[kRearRight] = -input.x - input.y + zRotation;

  Normalize(wheelSpeeds);

  m_frontLeftMotor.Set(wheelSpeeds[kFrontLeft] * m_maxOutput);
  m_frontRightMotor.Set(wheelSpeeds[kFrontRight] * m_maxOutput);
  m_rearLeftMotor.Set(wheelSpeeds[kRearLeft] * m_maxOutput);
  m_rearRightMotor.Set(wheelSpeeds[kRearRight] * m_maxOutput);

  m_safetyHelper.Feed();
}

/**
 * Drive method for Mecanum platform.
 *
 * Angles are measured clockwise from the positive X axis. The robot's speed is
 * independent from its angle or rotation rate.
 *
 * @param magnitude The robot's speed at a given angle [-1.0..1.0]. Forward is
 *                  positive.
 * @param angle     The angle around the Z axis at which the robot drives in
 *                  degrees [-180..180].
 * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0].
 *                  Clockwise is positive.
 */
void MecanumDrive::DrivePolar(double magnitude, double angle,
                              double zRotation) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 4,
               HALUsageReporting::kRobotDrive_MecanumPolar);
    reported = true;
  }

  DriveCartesian(magnitude * std::sin(angle * (kPi / 180.0)),
                 magnitude * std::cos(angle * (kPi / 180.0)), zRotation, 0.0);
}

void MecanumDrive::StopMotor() {
  m_frontLeftMotor.StopMotor();
  m_frontRightMotor.StopMotor();
  m_rearLeftMotor.StopMotor();
  m_rearRightMotor.StopMotor();
  m_safetyHelper.Feed();
}

void MecanumDrive::GetDescription(llvm::raw_ostream& desc) const {
  desc << "MecanumDrive";
}

void MecanumDrive::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("MecanumDrive");
  builder.AddDoubleProperty("Front Left Motor Speed",
                            [=]() { return m_frontLeftMotor.Get(); },
                            [=](double value) { m_frontLeftMotor.Set(value); });
  builder.AddDoubleProperty(
      "Front Right Motor Speed", [=]() { return m_frontRightMotor.Get(); },
      [=](double value) { m_frontRightMotor.Set(value); });
  builder.AddDoubleProperty("Rear Left Motor Speed",
                            [=]() { return m_rearLeftMotor.Get(); },
                            [=](double value) { m_rearLeftMotor.Set(value); });
  builder.AddDoubleProperty("Rear Right Motor Speed",
                            [=]() { return m_rearRightMotor.Get(); },
                            [=](double value) { m_rearRightMotor.Set(value); });
}
