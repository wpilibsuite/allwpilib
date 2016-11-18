/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include "Commands/Command.h"

namespace frc {

class TimedCommand : public Command {
 public:
  TimedCommand(const std::string& name, double timeout);
  explicit TimedCommand(double timeout);
  virtual ~TimedCommand() = default;

 protected:
  virtual bool IsFinished();
};

}  // namespace frc
