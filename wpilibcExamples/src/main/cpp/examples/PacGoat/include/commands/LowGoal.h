// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/commands/CommandGroup.h>

/**
 * Spit the ball out into the low goal assuming that the robot is in front of
 * it.
 */
class LowGoal : public frc::CommandGroup {
 public:
  LowGoal();
};
