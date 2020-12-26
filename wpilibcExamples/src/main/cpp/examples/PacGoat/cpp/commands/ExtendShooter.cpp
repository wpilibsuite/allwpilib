// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/ExtendShooter.h"

#include "Robot.h"

ExtendShooter::ExtendShooter() : frc::TimedCommand(1.0) {
  Requires(&Robot::shooter);
}

// Called just before this Command runs the first time
void ExtendShooter::Initialize() { Robot::shooter.ExtendBoth(); }

// Called once after isFinished returns true
void ExtendShooter::End() { Robot::shooter.RetractBoth(); }
