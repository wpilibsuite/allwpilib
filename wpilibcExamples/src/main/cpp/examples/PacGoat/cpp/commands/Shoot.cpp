// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/Shoot.h"

#include "Robot.h"
#include "commands/ExtendShooter.h"
#include "commands/OpenClaw.h"
#include "commands/SetCollectionSpeed.h"
#include "commands/WaitForPressure.h"

Shoot::Shoot() {
  AddSequential(new WaitForPressure());
  AddSequential(new SetCollectionSpeed(Collector::kStop));
  AddSequential(new OpenClaw());
  AddSequential(new ExtendShooter());
}
