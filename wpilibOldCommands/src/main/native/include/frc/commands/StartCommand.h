// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/commands/InstantCommand.h"

namespace frc {

/**
 * A PrintCommand is a command which prints out a string when it is initialized,
 * and then immediately finishes. It is useful if you want a CommandGroup to
 * print out a string when it reaches a certain point.
 *
 * This class is provided by the OldCommands VendorDep
 */
class StartCommand : public InstantCommand {
 public:
  explicit StartCommand(Command* commandToStart);
  ~StartCommand() override = default;

  StartCommand(StartCommand&&) = default;
  StartCommand& operator=(StartCommand&&) = default;

 protected:
  void Initialize() override;

 private:
  Command* m_commandToFork;
};

}  // namespace frc
