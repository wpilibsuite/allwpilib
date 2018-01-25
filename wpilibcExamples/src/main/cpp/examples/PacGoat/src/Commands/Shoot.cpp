/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Shoot.h"

#include "../Robot.h"
#include "ExtendShooter.h"
#include "OpenClaw.h"
#include "SetCollectionSpeed.h"
#include "WaitForPressure.h"

Shoot::Shoot() {
	AddSequential(new WaitForPressure());
	AddSequential(new SetCollectionSpeed(Collector::kStop));
	AddSequential(new OpenClaw());
	AddSequential(new ExtendShooter());
}
