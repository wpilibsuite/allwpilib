// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Servo.h"

#include <hal/UsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

using namespace frc;

constexpr double Servo::kMaxServoAngle;
constexpr double Servo::kMinServoAngle;

constexpr units::millisecond_t Servo::kDefaultMaxServoPWM;
constexpr units::millisecond_t Servo::kDefaultMinServoPWM;

Servo::Servo(int channel) : m_pwm(channel) {
  // Set minimum and maximum PWM values supported by the servo
  m_pwm.SetBounds(kDefaultMaxServoPWM, 0.0_ms, 0.0_ms, 0.0_ms,
                  kDefaultMinServoPWM);

  // Assign defaults for period multiplier for the servo PWM control signal
  m_pwm.SetPeriodMultiplier(PWM::kPeriodMultiplier_4X);

  HAL_ReportUsage("IO", channel, "Servo");
  wpi::SendableRegistry::SetName(this, "Servo", channel);
}

void Servo::Set(double value) {
  m_pwm.SetPosition(value);
}

void Servo::SetOffline() {
  m_pwm.SetDisabled();
}

double Servo::Get() const {
  return m_pwm.GetPosition();
}

void Servo::SetAngle(double degrees) {
  if (degrees < kMinServoAngle) {
    degrees = kMinServoAngle;
  } else if (degrees > kMaxServoAngle) {
    degrees = kMaxServoAngle;
  }

  m_pwm.SetPosition((degrees - kMinServoAngle) / GetServoAngleRange());
}

double Servo::GetAngle() const {
  return m_pwm.GetPosition() * GetServoAngleRange() + kMinServoAngle;
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
