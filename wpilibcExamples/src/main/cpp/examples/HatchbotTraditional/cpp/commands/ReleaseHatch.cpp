// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/ReleaseHatch.hpp"

ReleaseHatch::ReleaseHatch(HatchSubsystem* subsystem) : hatch(subsystem) {
  AddRequirements(subsystem);
}

void ReleaseHatch::Initialize() {
  hatch->ReleaseHatch();
}

bool ReleaseHatch::IsFinished() {
  return true;
}
