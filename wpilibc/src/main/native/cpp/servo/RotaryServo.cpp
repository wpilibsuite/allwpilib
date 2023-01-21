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

void RotaryServo::SetAngle(units::radian_t angle) {
  if (angle < m_minServoAngle) {
    angle = m_minServoAngle;
  } else if (angle > m_maxServoAngle) {
    angle = m_maxServoAngle;
  }

  SetPosition((angle - m_minServoAngle) / GetServoAngleRange());
}

units::radian_t RotaryServo::GetAngle() const {
  return GetPosition() * GetServoAngleRange() + m_minServoAngle;
}

units::radian_t RotaryServo::GetMaxAngle() const {
  return m_maxServoAngle;
}

units::radian_t RotaryServo::GetMinAngle() const {
  return m_minServoAngle;
}

RotaryServo::RotaryServo(std::string_view name, int channel,
                         units::radian_t minServoAngle,
                         units::radian_t maxServoAngle)
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

units::radian_t RotaryServo::GetServoAngleRange() const {
  return m_maxServoAngle - m_minServoAngle;
}
