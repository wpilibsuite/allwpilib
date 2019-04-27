/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/buttons/ButtonScheduler.h"

namespace frc {

class Trigger;
class Command;

class StartWhilePressedButtonScheduler : public ButtonScheduler {
 public:
  StartWhilePressedButtonScheduler(bool last, Trigger* button, Command* orders);
  virtual ~StartWhilePressedButtonScheduler() = default;

  StartWhilePressedButtonScheduler(StartWhilePressedButtonScheduler&&) =
      default;
  StartWhilePressedButtonScheduler& operator=(
      StartWhilePressedButtonScheduler&&) = default;

  virtual void Execute();
};

}  // namespace frc
