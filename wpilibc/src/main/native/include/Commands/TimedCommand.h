/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include "Commands/Command.h"

namespace frc {

/**
 * A {@link TimedCommand} will wait for a timeout before finishing.
 * {@link TimedCommand} is used to execute a command for a given amount of time.
 */
class TimedCommand : public Command {
 public:
  TimedCommand(const std::string& name, double timeout);
  explicit TimedCommand(double timeout);
  virtual ~TimedCommand() = default;

 protected:
  bool IsFinished() override;
};

}  // namespace frc
