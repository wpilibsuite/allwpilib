// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/ReleaseHatch.h"

ReleaseHatch::ReleaseHatch(HatchSubsystem* subsystem) : m_hatch(subsystem) {
  AddRequirements({subsystem});
}

void ReleaseHatch::Initialize() {
  m_hatch->ReleaseHatch();
}

bool ReleaseHatch::IsFinished() {
  return true;
}
