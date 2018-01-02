/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Servo.h"

#include <HAL/HAL.h>

#include "SmartDashboard/SendableBuilder.h"

using namespace frc;

constexpr double Servo::kMaxServoAngle;
constexpr double Servo::kMinServoAngle;

constexpr double Servo::kDefaultMaxServoPWM;
constexpr double Servo::kDefaultMinServoPWM;

/**
 * @param channel The PWM channel to which the servo is attached. 0-9 are
 *                on-board, 10-19 are on the MXP port
 */
Servo::Servo(int channel) : SafePWM(channel) {
  // Set minimum and maximum PWM values supported by the servo
  SetBounds(kDefaultMaxServoPWM, 0.0, 0.0, 0.0, kDefaultMinServoPWM);

  // Assign defaults for period multiplier for the servo PWM control signal
  SetPeriodMultiplier(kPeriodMultiplier_4X);

  HAL_Report(HALUsageReporting::kResourceType_Servo, channel);
  SetName("Servo", channel);
}

/**
 * Set the servo position.
 *
 * Servo values range from 0.0 to 1.0 corresponding to the range of full left to
 * full right.
 *
 * @param value Position from 0.0 to 1.0.
 */
void Servo::Set(double value) { SetPosition(value); }

/**
 * Set the servo to offline.
 *
 * Set the servo raw value to 0 (undriven)
 */
void Servo::SetOffline() { SetRaw(0); }

/**
 * Get the servo position.
 *
 * Servo values range from 0.0 to 1.0 corresponding to the range of full left to
 * full right.
 *
 * @return Position from 0.0 to 1.0.
 */
double Servo::Get() const { return GetPosition(); }

/**
 * Set the servo angle.
 *
 * Assume that the servo angle is linear with respect to the PWM value (big
 * assumption, need to test).
 *
 * Servo angles that are out of the supported range of the servo simply
 * "saturate" in that direction. In other words, if the servo has a range of
 * (X degrees to Y degrees) than angles of less than X result in an angle of
 * X being set and angles of more than Y degrees result in an angle of Y being
 * set.
 *
 * @param degrees The angle in degrees to set the servo.
 */
void Servo::SetAngle(double degrees) {
  if (degrees < kMinServoAngle) {
    degrees = kMinServoAngle;
  } else if (degrees > kMaxServoAngle) {
    degrees = kMaxServoAngle;
  }

  SetPosition((degrees - kMinServoAngle) / GetServoAngleRange());
}

/**
 * Get the servo angle.
 *
 * Assume that the servo angle is linear with respect to the PWM value (big
 * assumption, need to test).
 *
 * @return The angle in degrees to which the servo is set.
 */
double Servo::GetAngle() const {
  return GetPosition() * GetServoAngleRange() + kMinServoAngle;
}

void Servo::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Servo");
  builder.AddDoubleProperty("Value", [=]() { return Get(); },
                            [=](double value) { Set(value); });
}
