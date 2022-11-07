// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "motorcontrol/MockMotorController.h"

using namespace frc;

void MockMotorController::Set(double speed) {
  m_speed = m_isInverted ? -speed : speed;
}

double MockMotorController::Get() const {
  return m_speed;
}

void MockMotorController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool MockMotorController::GetInverted() const {
  return m_isInverted;
}

void MockMotorController::Disable() {
  m_speed = 0;
}

void MockMotorController::StopMotor() {
  Disable();
}
