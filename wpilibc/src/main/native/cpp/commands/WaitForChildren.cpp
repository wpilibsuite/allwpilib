/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
