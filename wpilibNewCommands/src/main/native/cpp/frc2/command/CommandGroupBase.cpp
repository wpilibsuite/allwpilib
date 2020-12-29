// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/CommandGroupBase.h"

#include <frc/WPIErrors.h>

using namespace frc2;

bool CommandGroupBase::RequireUngrouped(Command& command) {
  if (command.IsGrouped()) {
    wpi_setGlobalWPIErrorWithContext(
        CommandIllegalUse,
        "Commands cannot be added to more than one CommandGroup");
    return false;
  } else {
    return true;
  }
}

bool CommandGroupBase::RequireUngrouped(
    wpi::ArrayRef<std::unique_ptr<Command>> commands) {
  bool allUngrouped = true;
  for (auto&& command : commands) {
    allUngrouped &= !command.get()->IsGrouped();
  }
  if (!allUngrouped) {
    wpi_setGlobalWPIErrorWithContext(
        CommandIllegalUse,
        "Commands cannot be added to more than one CommandGroup");
  }
  return allUngrouped;
}

bool CommandGroupBase::RequireUngrouped(
    std::initializer_list<Command*> commands) {
  bool allUngrouped = true;
  for (auto&& command : commands) {
    allUngrouped &= !command->IsGrouped();
  }
  if (!allUngrouped) {
    wpi_setGlobalWPIErrorWithContext(
        CommandIllegalUse,
        "Commands cannot be added to more than one CommandGroup");
  }
  return allUngrouped;
}
