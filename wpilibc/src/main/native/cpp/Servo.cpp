// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Servo.h"

#include <hal/UsageReporting.h>
#include <wpi/telemetry/TelemetryTable.h>

using namespace frc;

Servo::Servo(int channel) : m_pwm(channel) {
  // Assign defaults for period multiplier for the servo PWM control signal
  m_pwm.SetOutputPeriod(PWM::kOutputPeriod_20Ms);

  HAL_ReportUsage("IO", channel, "Servo");

  m_simDevice = hal::SimDevice{"Servo", channel};
  if (m_simDevice) {
    m_simPosition = m_simDevice.CreateDouble("Position", true, 0.0);
    m_pwm.SetSimDevice(m_simDevice);
  }
}

void Servo::Set(double value) {
  value = std::clamp(value, 0.0, 1.0);
  if (m_simPosition) {
    m_simPosition.Set(value);
  }

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
  return (rawValue - m_minPwm).to<double>() /
         GetFullRangeScaleFactor().to<double>();
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

void Servo::UpdateTelemetry(wpi::TelemetryTable& table) const {
  table.Log("Value", Get());
}

std::string_view Servo::GetTelemetryType() const {
  return "Servo";
}

double Servo::GetServoAngleRange() {
  return kMaxServoAngle - kMinServoAngle;
}

units::microsecond_t Servo::GetFullRangeScaleFactor() const {
  return m_maxPwm - m_minPwm;
}

int Servo::GetChannel() const {
  return m_pwm.GetChannel();
}
