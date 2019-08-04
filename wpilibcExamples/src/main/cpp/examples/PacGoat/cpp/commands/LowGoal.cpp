/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
