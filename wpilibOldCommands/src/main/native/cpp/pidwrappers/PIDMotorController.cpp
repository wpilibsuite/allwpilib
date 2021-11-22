// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/pidwrappers/PIDMotorController.h"

#include <fmt/format.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

using namespace frc;

PIDMotorController::PIDMotorController(MotorController& motorController) {
  m_motorController = motorController;
}

void PIDMotorController::Set(double speed) {
  m_motorController.Set(m_isInverted ? -speed : speed);
}

double PIDMotorController::Get() const {
  return m_motorController.Get() * (m_isInverted ? -1.0 : 1.0);
}

void PIDMotorController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool PIDMotorController::GetInverted() const {
  return m_isInverted;
}

void PIDMotorController::Disable() {
  m_motorController.Disable();
}

void PIDMotorController::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Motor Controller");
  builder.SetActuator(true);
  builder.SetSafeState([=] { Disable(); });
  builder.AddDoubleProperty(
      "Value", [=] { return Get(); }, [=](double value) { Set(value); });
}
