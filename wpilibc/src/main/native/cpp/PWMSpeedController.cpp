// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PWMSpeedController.h"

#include <wpi/raw_ostream.h>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

void PWMSpeedController::Set(double speed) {
  m_pwm.SetSpeed(m_isInverted ? -speed : speed);
}

double PWMSpeedController::Get() const {
  return m_pwm.GetSpeed() * (m_isInverted ? -1.0 : 1.0);
}

void PWMSpeedController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool PWMSpeedController::GetInverted() const {
  return m_isInverted;
}

void PWMSpeedController::Disable() {
  m_pwm.SetDisabled();
}

void PWMSpeedController::StopMotor() {
  Disable();
}

void PWMSpeedController::GetDescription(wpi::raw_ostream& desc) const {
  desc << "PWM " << GetChannel();
}

int PWMSpeedController::GetChannel() const {
  return m_pwm.GetChannel();
}

void PWMSpeedController::PIDWrite(double output) {
  Set(output);
}

PWMSpeedController::PWMSpeedController(const wpi::Twine& name, int channel)
    : m_pwm(channel, false) {
  SendableRegistry::GetInstance().AddLW(this, name, channel);
}

void PWMSpeedController::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Speed Controller");
  builder.SetActuator(true);
  builder.SetSafeState([=] { Disable(); });
  builder.AddDoubleProperty(
      "Value", [=] { return Get(); }, [=](double value) { Set(value); });
}
