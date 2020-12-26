// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/OpenClaw.h"

#include "Robot.h"

OpenClaw::OpenClaw() {
  Requires(&Robot::collector);
}

// Called just before this Command runs the first time
void OpenClaw::Initialize() {
  Robot::collector.Open();
}

// Make this return true when this Command no longer needs to run execute()
bool OpenClaw::IsFinished() {
  return Robot::collector.IsOpen();
}
