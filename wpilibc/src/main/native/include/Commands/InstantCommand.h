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
 * This command will execute once, then finish immediately afterward.
 *
 * <p>Subclassing {@link InstantCommand} is shorthand for returning true from
 * {@link Command isFinished}.
 */
class InstantCommand : public Command {
 public:
  explicit InstantCommand(const std::string& name);
  InstantCommand() = default;
  virtual ~InstantCommand() = default;

 protected:
  bool IsFinished() override;
};

}  // namespace frc
