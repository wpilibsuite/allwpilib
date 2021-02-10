// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/commands/Command.h>

/**
 * Wait until the pneumatics are fully pressurized. This command does nothing
 * and is intended to be used in command groups to wait for this condition.
 */
class WaitForPressure : public frc::Command {
 public:
  WaitForPressure();
  bool IsFinished() override;
};
