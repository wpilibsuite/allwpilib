/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/ReleaseHatch.h"

ReleaseHatch::ReleaseHatch(HatchSubsystem* subsystem) : m_hatch(subsystem) {
  AddRequirements({subsystem});
}

void ReleaseHatch::Initialize() { m_hatch->ReleaseHatch(); }

bool ReleaseHatch::IsFinished() { return true; }
