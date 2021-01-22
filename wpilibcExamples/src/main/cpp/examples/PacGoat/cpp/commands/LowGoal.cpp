// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/LowGoal.h"

#include "Robot.h"
#include "commands/ExtendShooter.h"
#include "commands/SetCollectionSpeed.h"
#include "commands/SetPivotSetpoint.h"

LowGoal::LowGoal() {
  AddSequential(new SetPivotSetpoint(Pivot::kLowGoal));
  AddSequential(new SetCollectionSpeed(Collector::kReverse));
  AddSequential(new ExtendShooter());
}
