// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/WaitForPressure.h"

#include "Robot.h"

WaitForPressure::WaitForPressure() { Requires(&Robot::pneumatics); }

// Make this return true when this Command no longer needs to run execute()
bool WaitForPressure::IsFinished() { return Robot::pneumatics.IsPressurized(); }
