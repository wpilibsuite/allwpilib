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

class CancelButtonScheduler : public ButtonScheduler {
 public:
  CancelButtonScheduler(bool last, Trigger* button, Command* orders);
  virtual ~CancelButtonScheduler() = default;

  CancelButtonScheduler(CancelButtonScheduler&&) = default;
  CancelButtonScheduler& operator=(CancelButtonScheduler&&) = default;

  virtual void Execute();
};

}  // namespace frc
