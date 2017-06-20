/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Drive/MecanumDrive.h"

#include <algorithm>
#include <cmath>

#include <HAL/HAL.h>

#include "Drive/Vector2d.h"
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
      m_rearRightMotor(rearRightMotor) {}

/**
 * Drive method for Mecanum platform.
 *
 * @param x         The speed that the robot should drive in the X direction.
 *                  [-1.0..1.0]
 * @param y         The speed that the robot should drive in the Y direction.
 *                  [-1.0..1.0]
 * @param rotation  The rate of rotation for the robot that is completely
 *                  independent of the translation. [-1.0..1.0]
 * @param gyroAngle The current angle reading from the gyro. Use this to
 *                  implement field-oriented controls.
 */
void MecanumDrive::DriveCartesian(double x, double y, double rotation,
                                  double gyroAngle) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 4,
               HALUsageReporting::kRobotDrive_MecanumCartesian);
    reported = true;
  }

  x = Limit(x);
  x = ApplyDeadband(x, m_deadband);

  y = Limit(y);
  y = ApplyDeadband(y, m_deadband);

  // Compensate for gyro angle.
  Vector2d input{x, y};
  input.Rotate(gyroAngle);

  double wheelSpeeds[4];
  wheelSpeeds[kFrontLeft] = input.x + input.y + rotation;
  wheelSpeeds[kFrontRight] = input.x - input.y + rotation;
  wheelSpeeds[kRearLeft] = -input.x + input.y + rotation;
  wheelSpeeds[kRearRight] = -input.x - input.y + rotation;

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
 * @param magnitude The speed that the robot should drive in a given direction.
 *                  [-1.0..1.0]
 * @param angle     The direction the robot should drive in degrees. 0.0 is
 *                  straight ahead. The direction and maginitude are independent
 *                  of the rotation rate.
 * @param rotation  The rate of rotation for the robot that is completely
 *                  independent of the magnitude or direction. [-1.0..1.0]
 */
void MecanumDrive::DrivePolar(double magnitude, double angle, double rotation) {
  if (!reported) {
    HAL_Report(HALUsageReporting::kResourceType_RobotDrive, 4,
               HALUsageReporting::kRobotDrive_MecanumPolar);
    reported = true;
  }

  // Normalized for full power along the Cartesian axes.
  magnitude = Limit(magnitude) * std::sqrt(2.0);

  DriveCartesian(magnitude * std::cos(angle * (kPi / 180.0)),
                 magnitude * std::sin(angle * (kPi / 180.0)), rotation, 0.0);
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
