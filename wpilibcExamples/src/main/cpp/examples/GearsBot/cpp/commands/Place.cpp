// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
