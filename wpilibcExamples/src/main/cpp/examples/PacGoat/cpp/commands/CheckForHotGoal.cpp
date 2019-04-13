/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/CheckForHotGoal.h"

#include "Robot.h"
#pragma warning(disable: 4244 4267 4146)

CheckForHotGoal::CheckForHotGoal(double time) { SetTimeout(time); }

// Make this return true when this Command no longer needs to run execute()
bool CheckForHotGoal::IsFinished() {
  return IsTimedOut() || Robot::shooter.GoalIsHot();
}
