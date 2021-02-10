// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/Collect.h"

#include "Robot.h"
#include "commands/CloseClaw.h"
#include "commands/SetCollectionSpeed.h"
#include "commands/SetPivotSetpoint.h"
#include "commands/WaitForBall.h"

Collect::Collect() {
  AddSequential(new SetCollectionSpeed(Collector::kForward));
  AddParallel(new CloseClaw());
  AddSequential(new SetPivotSetpoint(Pivot::kCollect));
  AddSequential(new WaitForBall());
}
