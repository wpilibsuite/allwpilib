/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/Shoot.h"

#include "Commands/ExtendShooter.h"
#include "Commands/OpenClaw.h"
#include "Commands/SetCollectionSpeed.h"
#include "Commands/WaitForPressure.h"
#include "Robot.h"

Shoot::Shoot() {
  AddSequential(new WaitForPressure());
  AddSequential(new SetCollectionSpeed(Collector::kStop));
  AddSequential(new OpenClaw());
  AddSequential(new ExtendShooter());
}
