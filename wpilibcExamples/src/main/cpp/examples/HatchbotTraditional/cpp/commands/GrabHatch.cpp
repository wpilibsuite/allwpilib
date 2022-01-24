// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/GrabHatch.h"

GrabHatch::GrabHatch(HatchSubsystem* subsystem) : m_hatch(subsystem) {
  AddRequirements(subsystem);
}

void GrabHatch::Initialize() {
  m_hatch->GrabHatch();
}

bool GrabHatch::IsFinished() {
  return true;
}
