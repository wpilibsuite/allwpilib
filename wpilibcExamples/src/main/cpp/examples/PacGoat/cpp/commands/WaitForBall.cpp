// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/WaitForBall.h"

#include "Robot.h"

WaitForBall::WaitForBall() { Requires(&Robot::collector); }

// Make this return true when this Command no longer needs to run execute()
bool WaitForBall::IsFinished() { return Robot::collector.HasBall(); }
