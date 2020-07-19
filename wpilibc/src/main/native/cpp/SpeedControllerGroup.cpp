/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/SpeedControllerGroup.h"

#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

// Can't use a delegated constructor here because of an MSVC bug.
// https://developercommunity.visualstudio.com/content/problem/583/compiler-bug-with-delegating-a-constructor.html

SpeedControllerGroup::SpeedControllerGroup(
    std::vector<std::reference_wrapper<SpeedController>>&& speedControllers)
    : m_speedControllers(std::move(speedControllers)) {
  Initialize();
}

void SpeedControllerGroup::Initialize() {
  for (auto& speedController : m_speedControllers)
    SendableRegistry::GetInstance().AddChild(this, &speedController.get());
  static int instances = 0;
  ++instances;
  SendableRegistry::GetInstance().Add(this, "SpeedControllerGroup", instances);
}

void SpeedControllerGroup::Set(double speed) {
  for (auto speedController : m_speedControllers) {
    speedController.get().Set(m_isInverted ? -speed : speed);
  }
}

double SpeedControllerGroup::Get() const {
  if (!m_speedControllers.empty()) {
    return m_speedControllers.front().get().Get() * (m_isInverted ? -1 : 1);
  }
  return 0.0;
}

void SpeedControllerGroup::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool SpeedControllerGroup::GetInverted() const { return m_isInverted; }

void SpeedControllerGroup::Disable() {
  for (auto speedController : m_speedControllers) {
    speedController.get().Disable();
  }
}

void SpeedControllerGroup::StopMotor() {
  for (auto speedController : m_speedControllers) {
    speedController.get().StopMotor();
  }
}

void SpeedControllerGroup::PIDWrite(double output) { Set(output); }

void SpeedControllerGroup::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Speed Controller");
  builder.SetActuator(true);
  builder.SetSafeState([=]() { StopMotor(); });
  builder.AddDoubleProperty(
      "Value", [=]() { return Get(); }, [=](double value) { Set(value); });
}
