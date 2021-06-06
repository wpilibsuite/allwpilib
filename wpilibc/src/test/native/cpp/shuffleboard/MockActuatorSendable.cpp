// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "shuffleboard/MockActuatorSendable.h"

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

MockActuatorSendable::MockActuatorSendable(std::string_view name) {
  SendableRegistry::GetInstance().Add(this, name);
}

void MockActuatorSendable::InitSendable(SendableBuilder& builder) {
  builder.SetActuator(true);
}
