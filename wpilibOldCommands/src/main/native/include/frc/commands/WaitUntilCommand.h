// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <units/time.h>

#include "frc/commands/Command.h"

namespace frc {

/**
 * A WaitCommand will wait until a certain match time before finishing.
 *
 * This will wait until the game clock reaches some value, then continue to
 * the next command.
 *
 * This class is provided by the OldCommands VendorDep
 *
 * @see CommandGroup
 */
class WaitUntilCommand : public Command {
 public:
  explicit WaitUntilCommand(units::second_t time);

  WaitUntilCommand(std::string_view name, units::second_t time);

  ~WaitUntilCommand() override = default;

  WaitUntilCommand(WaitUntilCommand&&) = default;
  WaitUntilCommand& operator=(WaitUntilCommand&&) = default;

 protected:
  /**
   * Check if we've reached the actual finish time.
   */
  bool IsFinished() override;

 private:
  units::second_t m_time;
};

}  // namespace frc
