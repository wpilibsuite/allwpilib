// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/servo/RotaryServo.h"

#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

using namespace frc;

void RotaryServo::Set(double value) {
  SetPosition(value);
}

void RotaryServo::SetOffline() {
  SetRaw(0);
}

double RotaryServo::Get() const {
  return GetPosition();
}

void RotaryServo::SetAngle(double degrees) {
  if (degrees < m_minServoAngle) {
    degrees = m_minServoAngle;
  } else if (degrees > m_maxServoAngle) {
    degrees = m_maxServoAngle;
  }

  SetPosition((degrees - m_minServoAngle) / GetServoAngleRange());
}

double RotaryServo::GetAngle() const {
  return GetPosition() * GetServoAngleRange() + m_minServoAngle;
}

double RotaryServo::GetMaxAngle() const {
  return m_maxServoAngle;
}

double RotaryServo::GetMinAngle() const {
  return m_minServoAngle;
}

RotaryServo::RotaryServo(std::string_view name, int channel,
                         double minServoAngle, double maxServoAngle)
    : PWM(channel),
      m_minServoAngle(minServoAngle),
      m_maxServoAngle(maxServoAngle) {
  // Assign defaults for period multiplier for the servo PWM control signal
  SetPeriodMultiplier(kPeriodMultiplier_4X);

  HAL_Report(HALUsageReporting::kResourceType_Servo, channel + 1);
  wpi::SendableRegistry::SetName(this, name, channel);
}

void RotaryServo::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Servo");
  builder.AddDoubleProperty(
      "Value", [=, this] { return Get(); },
      [=, this](double value) { Set(value); });
}

double RotaryServo::GetServoAngleRange() const {
  return m_maxServoAngle - m_minServoAngle;
}
