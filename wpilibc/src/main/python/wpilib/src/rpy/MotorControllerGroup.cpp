// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "MotorControllerGroup.h"

#include "wpi/util/sendable/SendableBuilder.hpp"

using namespace wpi;

void PyMotorControllerGroup::Initialize() {
  for (auto motorController : m_motorControllers) {
    wpi::util::SendableRegistry::AddChild(this, motorController.get());
  }
  static int instances = 0;
  ++instances;
  wpi::util::SendableRegistry::Add(this, "MotorControllerGroup", instances);
}

void PyMotorControllerGroup::Set(double speed) {
  for (auto motorController : m_motorControllers) {
    motorController->Set(m_isInverted ? -speed : speed);
  }
}

void PyMotorControllerGroup::SetVoltage(wpi::units::volt_t output) {
  for (auto motorController : m_motorControllers) {
    motorController->SetVoltage(m_isInverted ? -output : output);
  }
}

double PyMotorControllerGroup::Get() const {
  if (!m_motorControllers.empty()) {
    return m_motorControllers.front()->Get() * (m_isInverted ? -1 : 1);
  }
  return 0.0;
}

void PyMotorControllerGroup::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool PyMotorControllerGroup::GetInverted() const { return m_isInverted; }

void PyMotorControllerGroup::Disable() {
  for (auto motorController : m_motorControllers) {
    motorController->Disable();
  }
}

void PyMotorControllerGroup::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Motor Controller");
  builder.SetActuator(true);
  builder.AddDoubleProperty("Value", [=, this]() { return Get(); },
                            [=, this](double value) { Set(value); });
}
