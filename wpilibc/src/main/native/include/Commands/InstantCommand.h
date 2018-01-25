/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <llvm/Twine.h>

#include "Commands/Command.h"

namespace frc {

/**
 * This command will execute once, then finish immediately afterward.
 *
 * Subclassing InstantCommand is shorthand for returning true from IsFinished().
 */
class InstantCommand : public Command {
 public:
  explicit InstantCommand(const llvm::Twine& name);
  InstantCommand() = default;
  virtual ~InstantCommand() = default;

 protected:
  bool IsFinished() override;
};

}  // namespace frc
