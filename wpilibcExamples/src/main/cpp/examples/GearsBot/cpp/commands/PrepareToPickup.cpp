// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/PrepareToPickup.h"

#include <frc2/command/ParallelCommandGroup.h>

#include "commands/OpenClaw.h"
#include "commands/SetElevatorSetpoint.h"
#include "commands/SetWristSetpoint.h"

PrepareToPickup::PrepareToPickup(Claw* claw, Wrist* wrist, Elevator* elevator) {
  SetName("PrepareToPickup");
  AddCommands(OpenClaw(claw),
              frc2::ParallelCommandGroup(SetElevatorSetpoint(0, elevator),
                                         SetWristSetpoint(0, wrist)));
}
