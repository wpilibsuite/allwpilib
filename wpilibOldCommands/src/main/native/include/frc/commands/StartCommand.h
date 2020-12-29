// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/commands/InstantCommand.h"

namespace frc {

class StartCommand : public InstantCommand {
 public:
  explicit StartCommand(Command* commandToStart);
  virtual ~StartCommand() = default;

  StartCommand(StartCommand&&) = default;
  StartCommand& operator=(StartCommand&&) = default;

 protected:
  virtual void Initialize();

 private:
  Command* m_commandToFork;
};

}  // namespace frc
