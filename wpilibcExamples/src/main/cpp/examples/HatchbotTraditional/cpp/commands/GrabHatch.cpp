/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/GrabHatch.h"

GrabHatch::GrabHatch(HatchSubsystem* subsystem) : m_hatch(subsystem) {
  AddRequirements({subsystem});
}

void GrabHatch::Initialize() { m_hatch->GrabHatch(); }

bool GrabHatch::IsFinished() { return true; }
