/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CloseClaw.h"

#include "../Robot.h"

CloseClaw::CloseClaw() {
	Requires(&Robot::collector);
}

// Called just before this Command runs the first time
void CloseClaw::Initialize() {
	Robot::collector.Close();
}
