// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/servo/LinearServo.h"

#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

using namespace frc;

void LinearServo::Set(double value) {
  m_pwm.SetPosition(value);
}

void LinearServo::SetOffline() {
  m_pwm.SetRaw(0);
}

double LinearServo::Get() const {
  return m_pwm.GetPosition();
}

void LinearServo::SetExtendDistance(units::meter_t distance) {
  if (distance < 0.0_m) {
    distance = 0.0_m;
  } else if (distance > m_stroke) {
    distance = m_stroke;
  }

  m_pwm.SetPosition(distance / m_stroke);
}

units::meter_t LinearServo::GetExtendDistance() const {
  return m_pwm.GetPosition() * m_stroke;
}

LinearServo::LinearServo(std::string_view name, int channel,
                         units::meter_t stroke)
    : m_pwm(channel, false), m_stroke(stroke) {
  // Assign defaults for period multiplier for the servo PWM control signal
  m_pwm.SetPeriodMultiplier(PWM::kPeriodMultiplier_4X);

  HAL_Report(HALUsageReporting::kResourceType_Servo, channel + 1);
  wpi::SendableRegistry::AddLW(this, name, channel);
}

void LinearServo::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Servo");
  builder.SetActuator(true);
  builder.AddDoubleProperty(
      "Value", [=, this] { return Get(); },
      [=, this](double value) { Set(value); });
}
