/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/Twine.h>

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

  WaitUntilCommand(const wpi::Twine& name, double time);

  virtual ~WaitUntilCommand() = default;

  WaitUntilCommand(WaitUntilCommand&&) = default;
  WaitUntilCommand& operator=(WaitUntilCommand&&) = default;

 protected:
  /**
   * Check if we've reached the actual finish time.
   */
  virtual bool IsFinished();

 private:
  double m_time;
};

}  // namespace frc
