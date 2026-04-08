// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "motorcontrol/MockPWMMotorController.hpp"

using namespace wpi;

void MockPWMMotorController::SetDutyCycle(double dutyCycle) {
  m_dutyCycle = m_isInverted ? -dutyCycle : dutyCycle;
}

double MockPWMMotorController::GetDutyCycle() const {
  return m_dutyCycle;
}

void MockPWMMotorController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool MockPWMMotorController::GetInverted() const {
  return m_isInverted;
}

void MockPWMMotorController::Disable() {
  m_dutyCycle = 0;
}

void MockPWMMotorController::StopMotor() {
  Disable();
}
