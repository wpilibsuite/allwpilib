/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Place.h"

#include "OpenClaw.h"
#include "SetElevatorSetpoint.h"
#include "SetWristSetpoint.h"

Place::Place()
    : frc::CommandGroup("Place") {
	AddSequential(new SetElevatorSetpoint(0.25));
	AddSequential(new SetWristSetpoint(0));
	AddSequential(new OpenClaw());
}
