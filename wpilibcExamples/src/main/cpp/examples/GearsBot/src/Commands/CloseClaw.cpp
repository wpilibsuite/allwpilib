/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CloseClaw.h"

#include "../Robot.h"

CloseClaw::CloseClaw()
    : frc::Command("CloseClaw") {
	Requires(&Robot::claw);
}

// Called just before this Command runs the first time
void CloseClaw::Initialize() {
	Robot::claw.Close();
}

// Make this return true when this Command no longer needs to run execute()
bool CloseClaw::IsFinished() {
	return Robot::claw.IsGripping();
}

// Called once after isFinished returns true
void CloseClaw::End() {
// NOTE: Doesn't stop in simulation due to lower friction causing the can to
// fall out
// + there is no need to worry about stalling the motor or crushing the can.
#ifndef SIMULATION
	Robot::claw.Stop();
#endif
}
