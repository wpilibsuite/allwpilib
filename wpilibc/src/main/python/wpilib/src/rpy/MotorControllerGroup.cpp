// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "MotorControllerGroup.h"

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace wpi;

void PyMotorControllerGroup::SetThrottle(double throttle) {
  for (auto motorController : m_motorControllers) {
    motorController->SetThrottle(m_isInverted ? -throttle : throttle);
  }
}

void PyMotorControllerGroup::SetVoltage(wpi::units::volt_t voltage) {
  for (auto motorController : m_motorControllers) {
    motorController->SetVoltage(m_isInverted ? -voltage : voltage);
  }
}

double PyMotorControllerGroup::GetThrottle() const {
  if (!m_motorControllers.empty()) {
    return m_motorControllers.front()->GetThrottle() * (m_isInverted ? -1 : 1);
  }
  return 0.0;
}

void PyMotorControllerGroup::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool PyMotorControllerGroup::GetInverted() const {
  return m_isInverted;
}

void PyMotorControllerGroup::Disable() {
  for (auto motorController : m_motorControllers) {
    motorController->Disable();
  }
}

void PyMotorControllerGroup::LogTo(wpi::TelemetryTable& table) const {
  table.Log("Value", GetThrottle());
}

std::string_view PyMotorControllerGroup::GetTelemetryType() const {
  return "Motor Controller";
}
