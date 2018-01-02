/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SpeedControllerGroup.h"

#include "SmartDashboard/SendableBuilder.h"

using namespace frc;

void SpeedControllerGroup::Set(double speed) {
  for (auto speedController : m_speedControllers) {
    speedController.get().Set(m_isInverted ? -speed : speed);
  }
}

double SpeedControllerGroup::Get() const {
  if (!m_speedControllers.empty()) {
    return m_speedControllers.front().get().Get();
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

void SpeedControllerGroup::PIDWrite(double output) {
  for (auto speedController : m_speedControllers) {
    speedController.get().PIDWrite(output);
  }
}

void SpeedControllerGroup::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Speed Controller");
  builder.SetSafeState([=]() { StopMotor(); });
  builder.AddDoubleProperty("Value", [=]() { return Get(); },
                            [=](double value) { Set(value); });
}
