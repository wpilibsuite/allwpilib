/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "OpenClaw.h"

#include "../Robot.h"

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
