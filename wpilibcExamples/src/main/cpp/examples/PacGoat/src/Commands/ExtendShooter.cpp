/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ExtendShooter.h"

#include "../Robot.h"

ExtendShooter::ExtendShooter()
    : frc::TimedCommand(1.0) {
	Requires(&Robot::shooter);
}

// Called just before this Command runs the first time
void ExtendShooter::Initialize() {
	Robot::shooter.ExtendBoth();
}

// Called once after isFinished returns true
void ExtendShooter::End() {
	Robot::shooter.RetractBoth();
}
