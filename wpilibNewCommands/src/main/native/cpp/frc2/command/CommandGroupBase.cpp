// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/CommandGroupBase.h"

using namespace frc2;

bool CommandGroupBase::RequireUngrouped(const Command& command) {
  if (command.IsGrouped()) {
    throw FRC_MakeError(
        frc::err::CommandIllegalUse,
        "Commands cannot be added to more than one CommandGroup");
  }
  return true;
}

bool CommandGroupBase::RequireUngrouped(const Command* command) {
  return RequireUngrouped(*command);
}

bool CommandGroupBase::RequireUngrouped(
    std::span<const std::unique_ptr<Command>> commands) {
  bool allUngrouped = true;
  for (auto&& command : commands) {
    allUngrouped &= !command.get()->IsGrouped();
  }
  if (!allUngrouped) {
    throw FRC_MakeError(
        frc::err::CommandIllegalUse,
        "Commands cannot be added to more than one CommandGroup");
  }
  return allUngrouped;
}

bool CommandGroupBase::RequireUngrouped(
    std::initializer_list<const Command*> commands) {
  bool allUngrouped = true;
  for (auto&& command : commands) {
    allUngrouped &= !command->IsGrouped();
  }
  if (!allUngrouped) {
    throw FRC_MakeError(
        frc::err::CommandIllegalUse,
        "Commands cannot be added to more than one CommandGroup");
  }
  return allUngrouped;
}
