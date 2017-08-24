/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Commands/InstantCommand.h"

namespace frc {

class StartCommand : public InstantCommand {
 public:
  explicit StartCommand(Command* commandToStart);
  virtual ~StartCommand() = default;

 protected:
  virtual void Initialize();

 private:
  Command* m_commandToFork;
};

}  // namespace frc
