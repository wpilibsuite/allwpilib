/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
