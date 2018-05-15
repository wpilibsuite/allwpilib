/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/Collect.h"

#include "Commands/CloseClaw.h"
#include "Commands/SetCollectionSpeed.h"
#include "Commands/SetPivotSetpoint.h"
#include "Commands/WaitForBall.h"
#include "Robot.h"

Collect::Collect() {
  AddSequential(new SetCollectionSpeed(Collector::kForward));
  AddParallel(new CloseClaw());
  AddSequential(new SetPivotSetpoint(Pivot::kCollect));
  AddSequential(new WaitForBall());
}
