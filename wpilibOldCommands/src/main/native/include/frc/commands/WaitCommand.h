// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <units/time.h>

#include "frc/commands/TimedCommand.h"

namespace frc {

class WaitCommand : public TimedCommand {
 public:
  /**
   * Creates a new WaitCommand with the given name and timeout.
   *
   * @param name    the name of the command
   * @param timeout the time before this command "times out"
   */
  explicit WaitCommand(units::second_t timeout);

  /**
   * Creates a new WaitCommand with the given timeout.
   *
   * @param timeout the time before this command "times out"
   */
  WaitCommand(std::string_view name, units::second_t timeout);

  ~WaitCommand() override = default;

  WaitCommand(WaitCommand&&) = default;
  WaitCommand& operator=(WaitCommand&&) = default;
};

}  // namespace frc
