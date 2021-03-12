// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "MockSpeedController.h"

using namespace frc;

void MockSpeedController::Set(double speed) {
  m_speed = m_isInverted ? -speed : speed;
}

double MockSpeedController::Get() const {
  return m_speed;
}

void MockSpeedController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool MockSpeedController::GetInverted() const {
  return m_isInverted;
}

void MockSpeedController::Disable() {
  m_speed = 0;
}

void MockSpeedController::StopMotor() {
  Disable();
}
