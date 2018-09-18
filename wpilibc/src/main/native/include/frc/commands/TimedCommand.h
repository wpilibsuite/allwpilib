/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/Twine.h>

#include "frc/commands/Command.h"

namespace frc {

/**
 * A TimedCommand will wait for a timeout before finishing.
 *
 * TimedCommand is used to execute a command for a given amount of time.
 */
class TimedCommand : public Command {
 public:
  /**
   * Creates a new TimedCommand with the given name and timeout.
   *
   * @param name    the name of the command
   * @param timeout the time (in seconds) before this command "times out"
   */
  TimedCommand(const wpi::Twine& name, double timeout);

  /**
   * Creates a new WaitCommand with the given timeout.
   *
   * @param timeout the time (in seconds) before this command "times out"
   */
  explicit TimedCommand(double timeout);

  /**
   * Creates a new TimedCommand with the given name and timeout.
   *
   * @param name      the name of the command
   * @param timeout   the time (in seconds) before this command "times out"
   * @param subsystem the subsystem that the command requires
   */
  TimedCommand(const wpi::Twine& name, double timeout, Subsystem& subsystem);

  /**
   * Creates a new WaitCommand with the given timeout.
   *
   * @param timeout   the time (in seconds) before this command "times out"
   * @param subsystem the subsystem that the command requires
   */
  TimedCommand(double timeout, Subsystem& subsystem);

  virtual ~TimedCommand() = default;

  TimedCommand(TimedCommand&&) = default;
  TimedCommand& operator=(TimedCommand&&) = default;

 protected:
  /**
   * Ends command when timed out.
   */
  bool IsFinished() override;
};

}  // namespace frc
