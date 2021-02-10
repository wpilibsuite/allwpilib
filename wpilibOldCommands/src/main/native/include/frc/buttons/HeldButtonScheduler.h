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
  ~HeldButtonScheduler() override = default;

  HeldButtonScheduler(HeldButtonScheduler&&) = default;
  HeldButtonScheduler& operator=(HeldButtonScheduler&&) = default;

  void Execute() override;
};

}  // namespace frc
