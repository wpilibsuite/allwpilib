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

Servo::Servo(int channel) : m_pwm(channel, false) {
  wpi::SendableRegistry::Add(this, "Servo", channel);

  // Assign defaults for period multiplier for the servo PWM control signal
  m_pwm.SetOutputPeriod(PWM::kOutputPeriod_20Ms);

  HAL_ReportUsage("IO", channel, "Servo");
  wpi::SendableRegistry::SetName(this, "Servo", channel);
}

void Servo::Set(double value) {
  value = std::clamp(value, 0.0, 1.0);

  units::microsecond_t rawValue =
      (value * GetFullRangeScaleFactor()) + m_minPwm;

  m_pwm.SetPulseTime(rawValue);
}

double Servo::Get() const {
  units::microsecond_t rawValue = m_pwm.GetPulseTime();

  if (rawValue < m_minPwm) {
    return 0.0;
  } else if (rawValue > m_maxPwm) {
    return 1.0;
  }
  return (rawValue - m_minPwm) / GetFullRangeScaleFactor();
}

void Servo::SetAngle(double degrees) {
  if (degrees < kMinServoAngle) {
    degrees = kMinServoAngle;
  } else if (degrees > kMaxServoAngle) {
    degrees = kMaxServoAngle;
  }

  Set((degrees - kMinServoAngle) / GetServoAngleRange());
}

double Servo::GetAngle() const {
  return Get() * GetServoAngleRange() + kMinServoAngle;
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

units::microsecond_t Servo::GetFullRangeScaleFactor() const {
  return m_maxPwm - m_minPwm;
}
