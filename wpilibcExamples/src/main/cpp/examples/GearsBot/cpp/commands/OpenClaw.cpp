/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/OpenClaw.h"

#include "Robot.h"

OpenClaw::OpenClaw() : frc::Command("OpenClaw") {
  Requires(&Robot::claw);
  SetTimeout(1);
}

// Called just before this Command runs the first time
void OpenClaw::Initialize() { Robot::claw.Open(); }

// Make this return true when this Command no longer needs to run execute()
bool OpenClaw::IsFinished() { return IsTimedOut(); }

// Called once after isFinished returns true
void OpenClaw::End() { Robot::claw.Stop(); }
