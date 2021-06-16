// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/CheckForHotGoal.h"

#include "Robot.h"

CheckForHotGoal::CheckForHotGoal(units::second_t time) {
  SetTimeout(time);
}

// Make this return true when this Command no longer needs to run execute()
bool CheckForHotGoal::IsFinished() {
  return IsTimedOut() || Robot::shooter.GoalIsHot();
}
