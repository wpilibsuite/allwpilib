// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ReplaceMeTimedCommand.h"

ReplaceMeTimedCommand::ReplaceMeTimedCommand(units::second_t timeout)
    : TimedCommand(timeout) {
  // Use Requires() here to declare subsystem dependencies
  // eg. Requires(Robot::chassis.get());
}

// Called just before this Command runs the first time
void ReplaceMeTimedCommand::Initialize() {}

// Called repeatedly when this Command is scheduled to run
void ReplaceMeTimedCommand::Execute() {}

// Called once after command times out
void ReplaceMeTimedCommand::End() {}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void ReplaceMeTimedCommand::Interrupted() {}
