/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  virtual ~WaitCommand() = default;

  WaitCommand(WaitCommand&&) = default;
  WaitCommand& operator=(WaitCommand&&) = default;
};

}  // namespace frc
