// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <units/time.h>

#include "frc/commands/Command.h"

namespace frc {

/**
 * A TimedCommand will wait for a timeout before finishing.
 *
 * TimedCommand is used to execute a command for a given amount of time.
 *
 * This class is provided by the OldCommands VendorDep
 */
class TimedCommand : public Command {
 public:
  /**
   * Creates a new TimedCommand with the given name and timeout.
   *
   * @param name    the name of the command
   * @param timeout the time before this command "times out"
   */
  TimedCommand(std::string_view name, units::second_t timeout);

  /**
   * Creates a new WaitCommand with the given timeout.
   *
   * @param timeout the time before this command "times out"
   */
  explicit TimedCommand(units::second_t timeout);

  /**
   * Creates a new TimedCommand with the given name and timeout.
   *
   * @param name      the name of the command
   * @param timeout   the time before this command "times out"
   * @param subsystem the subsystem that the command requires
   */
  TimedCommand(std::string_view name, units::second_t timeout,
               Subsystem& subsystem);

  /**
   * Creates a new WaitCommand with the given timeout.
   *
   * @param timeout   the time before this command "times out"
   * @param subsystem the subsystem that the command requires
   */
  TimedCommand(units::second_t timeout, Subsystem& subsystem);

  ~TimedCommand() override = default;

  TimedCommand(TimedCommand&&) = default;
  TimedCommand& operator=(TimedCommand&&) = default;

 protected:
  /**
   * Ends command when timed out.
   */
  bool IsFinished() override;
};

}  // namespace frc
