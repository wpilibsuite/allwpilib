// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/Twine.h>

#include "frc/commands/TimedCommand.h"

namespace frc {

class WaitCommand : public TimedCommand {
 public:
  /**
   * Creates a new WaitCommand with the given name and timeout.
   *
   * @param name    the name of the command
   * @param timeout the time (in seconds) before this command "times out"
   */
  explicit WaitCommand(double timeout);

  /**
   * Creates a new WaitCommand with the given timeout.
   *
   * @param timeout the time (in seconds) before this command "times out"
   */
  WaitCommand(const wpi::Twine& name, double timeout);

  ~WaitCommand() override = default;

  WaitCommand(WaitCommand&&) = default;
  WaitCommand& operator=(WaitCommand&&) = default;
};

}  // namespace frc
