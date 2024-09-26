// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/NidecBrushless.h"

#include <string>

#include <fmt/format.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

using namespace frc;

WPI_IGNORE_DEPRECATED

NidecBrushless::NidecBrushless(int pwmChannel, int dioChannel)
    : m_dio(dioChannel), m_pwm(pwmChannel) {
  wpi::SendableRegistry::AddChild(this, &m_dio);
  wpi::SendableRegistry::AddChild(this, &m_pwm);
  SetExpiration(0_s);
  SetSafetyEnabled(false);

  // the dio controls the output (in PWM mode)
  m_dio.SetPWMRate(15625);
  m_dio.EnablePWM(0.5);

  HAL_Report(HALUsageReporting::kResourceType_NidecBrushless, pwmChannel + 1);
  wpi::SendableRegistry::AddLW(this, "Nidec Brushless", pwmChannel);
}

WPI_UNIGNORE_DEPRECATED

void NidecBrushless::Set(double speed) {
  if (!m_disabled) {
    m_speed = speed;
    m_dio.UpdateDutyCycle(0.5 + 0.5 * (m_isInverted ? -speed : speed));
    m_pwm.SetAlwaysHighMode();
  }
  Feed();
}

double NidecBrushless::Get() const {
  return m_speed;
}

void NidecBrushless::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool NidecBrushless::GetInverted() const {
  return m_isInverted;
}

void NidecBrushless::Disable() {
  m_disabled = true;
  m_dio.UpdateDutyCycle(0.5);
  m_pwm.SetDisabled();
}

void NidecBrushless::Enable() {
  m_disabled = false;
}

void NidecBrushless::StopMotor() {
  m_dio.UpdateDutyCycle(0.5);
  m_pwm.SetDisabled();
}

std::string NidecBrushless::GetDescription() const {
  return fmt::format("Nidec {}", GetChannel());
}

int NidecBrushless::GetChannel() const {
  return m_pwm.GetChannel();
}

void NidecBrushless::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Nidec Brushless");
  builder.SetActuator(true);
  builder.SetSafeState([=, this] { StopMotor(); });
  builder.AddDoubleProperty(
      "Value", [=, this] { return Get(); },
      [=, this](double value) { Set(value); });
}
