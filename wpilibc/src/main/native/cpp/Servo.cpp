// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Servo.h"

#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

using namespace frc;

constexpr units::degree_t Servo::kMaxServoAngle;
constexpr units::degree_t Servo::kMinServoAngle;

constexpr units::millisecond_t Servo::kDefaultMaxServoPWM;
constexpr units::millisecond_t Servo::kDefaultMinServoPWM;

Servo::Servo(int channel) : PWM(channel) {
  // Set minimum and maximum PWM values supported by the servo
  SetBounds(kDefaultMaxServoPWM, 0.0_ms, 0.0_ms, 0.0_ms, kDefaultMinServoPWM);

  // Assign defaults for period multiplier for the servo PWM control signal
  SetPeriodMultiplier(kPeriodMultiplier_4X);

  HAL_Report(HALUsageReporting::kResourceType_Servo, channel + 1);
  wpi::SendableRegistry::SetName(this, "Servo", channel);
}

void Servo::Set(double value) {
  SetPosition(value);
}

void Servo::SetOffline() {
  SetDisabled();
}

double Servo::Get() const {
  return GetPosition();
}

void Servo::SetAngle(units::degree_t degrees) {
  if (degrees < kMinServoAngle) {
    degrees = kMinServoAngle;
  } else if (degrees > kMaxServoAngle) {
    degrees = kMaxServoAngle;
  }

  SetPosition((degrees - kMinServoAngle) / GetServoAngleRange());
}

units::degree_t Servo::GetAngle() const {
  return GetPosition() * GetServoAngleRange() + kMinServoAngle;
}

units::degree_t Servo::GetMaxAngle() const {
  return kMaxServoAngle;
}

units::degree_t Servo::GetMinAngle() const {
  return kMinServoAngle;
}

void Servo::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Servo");
  builder.AddDoubleProperty(
      "Value", [=, this] { return Get(); },
      [=, this](double value) { Set(value); });
}

units::degree_t Servo::GetServoAngleRange() const {
  return kMaxServoAngle - kMinServoAngle;
}
