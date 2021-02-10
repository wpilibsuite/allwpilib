// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/buttons/ButtonScheduler.h"

namespace frc {

class Trigger;
class Command;

class ReleasedButtonScheduler : public ButtonScheduler {
 public:
  ReleasedButtonScheduler(bool last, Trigger* button, Command* orders);
  ~ReleasedButtonScheduler() override = default;

  ReleasedButtonScheduler(ReleasedButtonScheduler&&) = default;
  ReleasedButtonScheduler& operator=(ReleasedButtonScheduler&&) = default;

  void Execute() override;
};

}  // namespace frc
