// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "frc/commands/Command.h"

namespace frc {

class WaitUntilCommand : public Command {
 public:
  /**
   * A WaitCommand will wait until a certain match time before finishing.
   *
   * This will wait until the game clock reaches some value, then continue to
   * the next command.
   *
   * @see CommandGroup
   */
  explicit WaitUntilCommand(double time);

  WaitUntilCommand(std::string_view name, double time);

  ~WaitUntilCommand() override = default;

  WaitUntilCommand(WaitUntilCommand&&) = default;
  WaitUntilCommand& operator=(WaitUntilCommand&&) = default;

 protected:
  /**
   * Check if we've reached the actual finish time.
   */
  bool IsFinished() override;

 private:
  double m_time;
};

}  // namespace frc
