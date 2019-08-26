/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
