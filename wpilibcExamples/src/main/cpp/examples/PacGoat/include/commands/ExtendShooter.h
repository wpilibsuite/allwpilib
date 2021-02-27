// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/commands/TimedCommand.h>

/**
 * Extend the shooter and then retract it after a second.
 */
class ExtendShooter : public frc::TimedCommand {
 public:
  ExtendShooter();
  void Initialize() override;
  void End() override;
};
