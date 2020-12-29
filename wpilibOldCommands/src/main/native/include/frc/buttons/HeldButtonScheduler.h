// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/buttons/ButtonScheduler.h"

namespace frc {

class Trigger;
class Command;

class HeldButtonScheduler : public ButtonScheduler {
 public:
  HeldButtonScheduler(bool last, Trigger* button, Command* orders);
  virtual ~HeldButtonScheduler() = default;

  HeldButtonScheduler(HeldButtonScheduler&&) = default;
  HeldButtonScheduler& operator=(HeldButtonScheduler&&) = default;

  virtual void Execute();
};

}  // namespace frc
