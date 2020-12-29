// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/WaitForChildren.h"

#include "frc/commands/CommandGroup.h"

using namespace frc;

WaitForChildren::WaitForChildren(double timeout)
    : Command("WaitForChildren", timeout) {}

WaitForChildren::WaitForChildren(const wpi::Twine& name, double timeout)
    : Command(name, timeout) {}

bool WaitForChildren::IsFinished() {
  return GetGroup() == nullptr || GetGroup()->GetSize() == 0;
}
