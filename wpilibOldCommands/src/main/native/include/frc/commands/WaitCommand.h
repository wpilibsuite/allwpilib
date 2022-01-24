// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <units/time.h>

#include "frc/commands/TimedCommand.h"

namespace frc {

/**
 * A WaitCommand will wait for a certain amount of time before finishing. It is
 * useful if you want a CommandGroup to pause for a moment.
 *
 * This class is provided by the OldCommands VendorDep
 */
class WaitCommand : public TimedCommand {
 public:
  /**
   * Creates a new WaitCommand with the given name and timeout.
   *
   * @param timeout the time before this command "times out"
   */
  explicit WaitCommand(units::second_t timeout);

  /**
   * Creates a new WaitCommand with the given timeout.
   *
   * @param name    the name of the command
   * @param timeout the time before this command "times out"
   */
  WaitCommand(std::string_view name, units::second_t timeout);

  ~WaitCommand() override = default;

  WaitCommand(WaitCommand&&) = default;
  WaitCommand& operator=(WaitCommand&&) = default;
};

}  // namespace frc
