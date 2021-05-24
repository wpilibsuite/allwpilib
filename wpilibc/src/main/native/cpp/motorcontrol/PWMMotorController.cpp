// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/PWMMotorController.h"

#include <wpi/raw_ostream.h>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

void PWMMotorController::Set(double speed) {
  m_pwm.SetSpeed(m_isInverted ? -speed : speed);
}

double PWMMotorController::Get() const {
  return m_pwm.GetSpeed() * (m_isInverted ? -1.0 : 1.0);
}

void PWMMotorController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool PWMMotorController::GetInverted() const {
  return m_isInverted;
}

void PWMMotorController::Disable() {
  m_pwm.SetDisabled();
}

void PWMMotorController::StopMotor() {
  Disable();
}

void PWMMotorController::GetDescription(wpi::raw_ostream& desc) const {
  desc << "PWM " << GetChannel();
}

int PWMMotorController::GetChannel() const {
  return m_pwm.GetChannel();
}

PWMMotorController::PWMMotorController(std::string_view name, int channel)
    : m_pwm(channel, false) {
  SendableRegistry::GetInstance().AddLW(this, name, channel);
}

void PWMMotorController::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Motor Controller");
  builder.SetActuator(true);
  builder.SetSafeState([=] { Disable(); });
  builder.AddDoubleProperty(
      "Value", [=] { return Get(); }, [=](double value) { Set(value); });
}
