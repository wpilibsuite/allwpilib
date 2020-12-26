// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/ExampleCommand.h"

#include "Robot.h"

ExampleCommand::ExampleCommand() {
  // Use Requires() here to declare subsystem dependencies
  Requires(&Robot::m_subsystem);
}

// Called just before this Command runs the first time
void ExampleCommand::Initialize() {}

// Called repeatedly when this Command is scheduled to run
void ExampleCommand::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool ExampleCommand::IsFinished() { return false; }

// Called once after isFinished returns true
void ExampleCommand::End() {}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void ExampleCommand::Interrupted() {}
