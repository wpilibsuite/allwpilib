/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/Place.h"

#include "commands/OpenClaw.h"
#include "commands/SetElevatorSetpoint.h"
#include "commands/SetWristSetpoint.h"

Place::Place(Claw* claw, Wrist* wrist, Elevator* elevator) {
  SetName("Place");
  // clang-format off
  AddCommands(SetElevatorSetpoint(0.25, elevator),
              SetWristSetpoint(0, wrist),
              OpenClaw(claw));
  // clang-format on
}
