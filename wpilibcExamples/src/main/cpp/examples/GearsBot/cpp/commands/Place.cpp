/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/Place.h"

#include "commands/OpenClaw.h"
#include "commands/SetElevatorReference.h"
#include "commands/SetWristReference.h"

Place::Place() : frc::CommandGroup("Place") {
  AddSequential(new SetElevatorReference(0.25));
  AddSequential(new SetWristReference(0));
  AddSequential(new OpenClaw());
}
