// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/MotorControllerGroup.h"

#include <utility>
#include <vector>

#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

using namespace frc;

// Can't use a delegated constructor here because of an MSVC bug.
// https://developercommunity.visualstudio.com/content/problem/583/compiler-bug-with-delegating-a-constructor.html

WPI_IGNORE_DEPRECATED

MotorControllerGroup::MotorControllerGroup(
    std::vector<std::reference_wrapper<MotorController>>&& motorControllers)
    : m_motorControllers(std::move(motorControllers)) {
  Initialize();
}

void MotorControllerGroup::Initialize() {
  for (auto& motorController : m_motorControllers) {
    wpi::SendableRegistry::AddChild(this, &motorController.get());
  }
  static int instances = 0;
  ++instances;
  wpi::SendableRegistry::Add(this, "MotorControllerGroup", instances);
}

void MotorControllerGroup::Set(double speed) {
  for (auto motorController : m_motorControllers) {
    motorController.get().Set(m_isInverted ? -speed : speed);
  }
}

void MotorControllerGroup::SetVoltage(units::volt_t output) {
  for (auto motorController : m_motorControllers) {
    motorController.get().SetVoltage(m_isInverted ? -output : output);
  }
}

double MotorControllerGroup::Get() const {
  if (!m_motorControllers.empty()) {
    return m_motorControllers.front().get().Get() * (m_isInverted ? -1 : 1);
  }
  return 0.0;
}

void MotorControllerGroup::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool MotorControllerGroup::GetInverted() const {
  return m_isInverted;
}

void MotorControllerGroup::Disable() {
  for (auto motorController : m_motorControllers) {
    motorController.get().Disable();
  }
}

void MotorControllerGroup::StopMotor() {
  for (auto motorController : m_motorControllers) {
    motorController.get().StopMotor();
  }
}

void MotorControllerGroup::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Motor Controller");
  builder.SetActuator(true);
  builder.SetSafeState([=, this] { StopMotor(); });
  builder.AddDoubleProperty(
      "Value", [=, this] { return Get(); },
      [=, this](double value) { Set(value); });
}

WPI_UNIGNORE_DEPRECATED
