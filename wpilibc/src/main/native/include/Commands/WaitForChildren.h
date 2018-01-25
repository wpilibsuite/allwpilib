/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <llvm/Twine.h>

#include "Commands/Command.h"

namespace frc {

class WaitForChildren : public Command {
 public:
  explicit WaitForChildren(double timeout);
  WaitForChildren(const llvm::Twine& name, double timeout);
  virtual ~WaitForChildren() = default;

 protected:
  virtual bool IsFinished();
};

}  // namespace frc
