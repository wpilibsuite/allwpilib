/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "shuffleboard/MockActuatorSendable.h"

using namespace frc;

MockActuatorSendable::MockActuatorSendable(wpi::StringRef name)
    : SendableBase(false) {
  SetName(name);
}

void MockActuatorSendable::InitSendable(SendableBuilder& builder) {
  builder.SetActuator(true);
}
