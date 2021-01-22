// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/Twine.h>

#include "frc2/command/CommandHelper.h"
#include "frc2/command/InstantCommand.h"

namespace frc2 {
/**
 * A command that prints a string when initialized.
 */
class PrintCommand : public CommandHelper<InstantCommand, PrintCommand> {
 public:
  /**
   * Creates a new a PrintCommand.
   *
   * @param message the message to print
   */
  explicit PrintCommand(const wpi::Twine& message);

  PrintCommand(PrintCommand&& other) = default;

  PrintCommand(const PrintCommand& other) = default;

  bool RunsWhenDisabled() const override;
};
}  // namespace frc2
