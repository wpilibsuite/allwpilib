// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/Pickup.h"

#include <frc2/command/ParallelCommandGroup.h>

#include "commands/CloseClaw.h"
#include "commands/SetElevatorSetpoint.h"
#include "commands/SetWristSetpoint.h"

Pickup::Pickup(Claw* claw, Wrist* wrist, Elevator* elevator) {
  SetName("Pickup");
  AddCommands(CloseClaw(claw),
              frc2::ParallelCommandGroup(SetWristSetpoint(-45, wrist),
                                         SetElevatorSetpoint(0.25, elevator)));
}
