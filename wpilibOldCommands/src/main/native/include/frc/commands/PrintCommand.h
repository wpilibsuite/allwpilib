// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "frc/commands/InstantCommand.h"

namespace frc {

/**
 * A PrintCommand is a command which prints out a string when it is initialized,
 * and then immediately finishes. It is useful if you want a CommandGroup to
 * print out a string when it reaches a certain point.
 *
 * This class is provided by the OldCommands VendorDep
 */
class PrintCommand : public InstantCommand {
 public:
  explicit PrintCommand(std::string_view message);
  ~PrintCommand() override = default;

  PrintCommand(PrintCommand&&) = default;
  PrintCommand& operator=(PrintCommand&&) = default;

 protected:
  void Initialize() override;

 private:
  std::string m_message;
};

}  // namespace frc
