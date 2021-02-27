// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <wpi/Twine.h>

#include "frc/commands/InstantCommand.h"

namespace frc {

class PrintCommand : public InstantCommand {
 public:
  explicit PrintCommand(const wpi::Twine& message);
  ~PrintCommand() override = default;

  PrintCommand(PrintCommand&&) = default;
  PrintCommand& operator=(PrintCommand&&) = default;

 protected:
  void Initialize() override;

 private:
  std::string m_message;
};

}  // namespace frc
