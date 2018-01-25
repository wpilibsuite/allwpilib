/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Pickup.h"

#include "CloseClaw.h"
#include "SetElevatorSetpoint.h"
#include "SetWristSetpoint.h"

Pickup::Pickup()
    : frc::CommandGroup("Pickup") {
	AddSequential(new CloseClaw());
	AddParallel(new SetWristSetpoint(-45));
	AddSequential(new SetElevatorSetpoint(0.25));
}
