/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Drive/KilloughDrive.h"

#include <algorithm>
#include <cmath>

#include <HAL/HAL.h>

#include "SpeedController.h"

using namespace frc;

constexpr double kPi = 3.14159265358979323846;

/**
 * Construct a Killough drive with the given motors and default motor angles.
 *
 * The default motor angles are 120, 60, and 270 degrees for the left, right,
 * and back motors respectively, which make the wheels on each corner parallel
 * to their respective opposite sides.
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
    : KilloughDrive(leftMotor, rightMotor, backMotor, 120.0, 60.0, 270.0) {}

/**
 * Construct a Killough drive with the given motors.
 *
 * Angles are measured in counter-clockwise degrees where zero degrees is
 * straight ahead.
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
}

/**
 * Drive method for Killough platform.
 *
 * @param x         The speed that the robot should drive in the X direction.
 *                  [-1.0..1.0]
 * @param y         The speed that the robot should drive in the Y direction.
 *                  [-1.0..1.0]
 * @param rotation  The rate of rotation for the robot that is completely
 *                  independent of the translation. [-1.0..1.0]
 * @param gyroAngle The current angle reading from the gyro.  Use this to
 *                  implement field-oriented controls.
 */
void KilloughDrive::DriveCartesian(double x, double y, double rotation,
                                   double gyroAngle) {
  if (!reported) {
    // HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 3,
    //            HALUsageReporting::kRobotDrive_KilloughCartesian);
    reported = true;
  }

  x = Limit(x);
  x = ApplyDeadband(x, m_deadband);

  y = Limit(y);
  y = ApplyDeadband(y, m_deadband);

  // Compensate for gyro angle.
  Vector2d input{x, y};
  input.Rotate(gyroAngle);

  double wheelSpeeds[3];
  wheelSpeeds[kLeft] = input.ScalarProject(m_leftVec) + rotation;
  wheelSpeeds[kRight] = input.ScalarProject(m_rightVec) + rotation;
  wheelSpeeds[kBack] = input.ScalarProject(m_backVec) + rotation;

  Normalize(wheelSpeeds);

  m_leftMotor.Set(wheelSpeeds[kLeft] * m_maxOutput);
  m_rightMotor.Set(wheelSpeeds[kRight] * m_maxOutput);
  m_backMotor.Set(wheelSpeeds[kBack] * m_maxOutput);

  m_safetyHelper.Feed();
}

/**
 * Drive method for Killough platform.
 *
 * @param magnitude The speed that the robot should drive in a given direction.
 *                  [-1.0..1.0]
 * @param angle     The direction the robot should drive in degrees. 0.0 is
 *                  straight ahead. The direction and maginitude are independent
 *                  of the rotation rate.
 * @param rotation  The rate of rotation for the robot that is completely
 *                  independent of the magnitude or direction. [-1.0..1.0]
 */
void KilloughDrive::DrivePolar(double magnitude, double angle,
                               double rotation) {
  if (!reported) {
    // HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 3,
    //            HALUsageReporting::kRobotDrive_KilloughPolar);
    reported = true;
  }

  // Normalized for full power along the Cartesian axes.
  magnitude = Limit(magnitude) * std::sqrt(2.0);

  DriveCartesian(magnitude * std::cos(angle * (kPi / 180.0)),
                 magnitude * std::sin(angle * (kPi / 180.0)), rotation, 0.0);
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
