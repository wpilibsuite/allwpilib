/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/Twine.h>
#include <wpi/raw_ostream.h>

#include "CommandHelper.h"
#include "InstantCommand.h"

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
