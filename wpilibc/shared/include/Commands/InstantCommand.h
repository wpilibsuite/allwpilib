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

class InstantCommand : public Command {
 public:
  explicit InstantCommand(const std::string& name);
  InstantCommand() = default;
  virtual ~InstantCommand() = default;

 protected:
  virtual bool IsFinished();
};

}  // namespace frc
