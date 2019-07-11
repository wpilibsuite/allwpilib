/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Servo.h"

#include <hal/HAL.h>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

constexpr double Servo::kMaxServoAngle;
constexpr double Servo::kMinServoAngle;

constexpr double Servo::kDefaultMaxServoPWM;
constexpr double Servo::kDefaultMinServoPWM;

Servo::Servo(int channel) : PWM(channel) {
  // Set minimum and maximum PWM values supported by the servo
  SetBounds(kDefaultMaxServoPWM, 0.0, 0.0, 0.0, kDefaultMinServoPWM);

  // Assign defaults for period multiplier for the servo PWM control signal
  SetPeriodMultiplier(kPeriodMultiplier_4X);

  HAL_Report(HALUsageReporting::kResourceType_Servo, channel);
  SetName("Servo", channel);
}

void Servo::Set(double value) { SetPosition(value); }

void Servo::SetOffline() { SetRaw(0); }

double Servo::Get() const { return GetPosition(); }

void Servo::SetAngle(double degrees) {
  if (degrees < kMinServoAngle) {
    degrees = kMinServoAngle;
  } else if (degrees > kMaxServoAngle) {
    degrees = kMaxServoAngle;
  }

  SetPosition((degrees - kMinServoAngle) / GetServoAngleRange());
}

double Servo::GetAngle() const {
  return GetPosition() * GetServoAngleRange() + kMinServoAngle;
}

double Servo::GetMaxAngle() const { return kMaxServoAngle; }

double Servo::GetMinAngle() const { return kMinServoAngle; }

void Servo::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Servo");
  builder.AddDoubleProperty("Value", [=]() { return Get(); },
                            [=](double value) { Set(value); });
}

double Servo::GetServoAngleRange() const {
  return kMaxServoAngle - kMinServoAngle;
}
