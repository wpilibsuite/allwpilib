// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/DriveAndShootAutonomous.h"

#include "Robot.h"
#include "commands/CheckForHotGoal.h"
#include "commands/CloseClaw.h"
#include "commands/DriveForward.h"
#include "commands/SetPivotSetpoint.h"
#include "commands/Shoot.h"
#include "commands/WaitForPressure.h"

DriveAndShootAutonomous::DriveAndShootAutonomous() {
  AddSequential(new CloseClaw());
  AddSequential(new WaitForPressure(), 2_s);
#ifndef SIMULATION
  // NOTE: Simulation doesn't currently have the concept of hot.
  AddSequential(new CheckForHotGoal(2_s));
#endif
  AddSequential(new SetPivotSetpoint(45));
  AddSequential(new DriveForward(8, 0.3));
  AddSequential(new Shoot());
}
