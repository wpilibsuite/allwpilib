/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/PrepareToPickup.h"

#include "commands/OpenClaw.h"
#include "commands/SetElevatorSetpoint.h"
#include "commands/SetWristSetpoint.h"

PrepareToPickup::PrepareToPickup() : frc::CommandGroup("PrepareToPickup") {
  AddParallel(new OpenClaw());
  AddParallel(new SetWristSetpoint(0));
  AddSequential(new SetElevatorSetpoint(0));
}
