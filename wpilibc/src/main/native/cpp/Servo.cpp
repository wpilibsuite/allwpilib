// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Servo.h"

#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

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

  HAL_Report(HALUsageReporting::kResourceType_Servo, channel + 1);
  wpi::SendableRegistry::SetName(this, "Servo", channel);
}

void Servo::Set(double value) {
  SetPosition(value);
}

void Servo::SetOffline() {
  SetRaw(0);
}

double Servo::Get() const {
  return GetPosition();
}

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

double Servo::GetMaxAngle() const {
  return kMaxServoAngle;
}

double Servo::GetMinAngle() const {
  return kMinServoAngle;
}

void Servo::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Servo");
  builder.AddDoubleProperty(
      "Value", [=, this] { return Get(); },
      [=, this](double value) { Set(value); });
}

double Servo::GetServoAngleRange() const {
  return kMaxServoAngle - kMinServoAngle;
}
