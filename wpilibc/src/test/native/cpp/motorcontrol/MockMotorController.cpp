// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "motorcontrol/MockMotorController.hpp"

using namespace wpi;

void MockMotorController::SetThrottle(double throttle) {
  m_throttle = m_isInverted ? -throttle : throttle;
}

double MockMotorController::GetThrottle() const {
  return m_throttle;
}

void MockMotorController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool MockMotorController::GetInverted() const {
  return m_isInverted;
}

void MockMotorController::Disable() {
  m_throttle = 0;
}
