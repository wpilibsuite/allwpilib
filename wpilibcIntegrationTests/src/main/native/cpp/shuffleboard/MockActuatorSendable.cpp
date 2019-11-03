/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "shuffleboard/MockActuatorSendable.h"

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

MockActuatorSendable::MockActuatorSendable(wpi::StringRef name) {
  SendableRegistry::GetInstance().Add(this, name);
}

void MockActuatorSendable::InitSendable(SendableBuilder& builder) {
  builder.SetActuator(true);
}
