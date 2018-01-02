/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Drive/KilloughDrive.h"

#include <algorithm>
#include <cmath>

#include <HAL/HAL.h>

#include "SmartDashboard/SendableBuilder.h"
#include "SpeedController.h"

using namespace frc;

constexpr double kPi = 3.14159265358979323846;

/**
 * Construct a Killough drive with the given motors and default motor angles.
 *
 * The default motor angles make the wheels on each corner parallel to their
 * respective opposite sides.
 *
 * If a motor needs to be inverted, do so before passing it in.
 *
 * @param leftMotor  The motor on the left corner.
 * @param rightMotor The motor on the right corner.
 * @param backMotor  The motor on the back corner.
 */
KilloughDrive::KilloughDrive(SpeedController& leftMotor,
                             SpeedController& rightMotor,
                             SpeedController& backMotor)
    : KilloughDrive(leftMotor, rightMotor, backMotor, kDefaultLeftMotorAngle,
                    kDefaultRightMotorAngle, kDefaultBackMotorAngle) {}

/**
 * Construct a Killough drive with the given motors.
 *
 * Angles are measured in degrees clockwise from the positive X axis.
 *
 * @param leftMotor       The motor on the left corner.
 * @param rightMotor      The motor on the right corner.
 * @param backMotor       The motor on the back corner.
 * @param leftMotorAngle  The angle of the left wheel's forward direction of
 *                        travel.
 * @param rightMotorAngle The angle of the right wheel's forward direction of
 *                        travel.
 * @param backMotorAngle  The angle of the back wheel's forward direction of
 *                        travel.
 */
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

/**
 * Drive method for Killough platform.
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
void KilloughDrive::DriveCartesian(double ySpeed, double xSpeed,
                                   double zRotation, double gyroAngle) {
  if (!reported) {
    // HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 3,
    //            HALUsageReporting::kRobotDrive_KilloughCartesian);
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

  m_safetyHelper.Feed();
}

/**
 * Drive method for Killough platform.
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
void KilloughDrive::DrivePolar(double magnitude, double angle,
                               double rotation) {
  if (!reported) {
    // HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 3,
    //            HALUsageReporting::kRobotDrive_KilloughPolar);
    reported = true;
  }

  DriveCartesian(magnitude * std::sin(angle * (kPi / 180.0)),
                 magnitude * std::cos(angle * (kPi / 180.0)), rotation, 0.0);
}

void KilloughDrive::StopMotor() {
  m_leftMotor.StopMotor();
  m_rightMotor.StopMotor();
  m_backMotor.StopMotor();
  m_safetyHelper.Feed();
}

void KilloughDrive::GetDescription(llvm::raw_ostream& desc) const {
  desc << "KilloughDrive";
}

void KilloughDrive::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("KilloughDrive");
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
