// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/commands/Command.h>

/**
 * This command allows PS3 joystick to drive the robot. It is always running
 * except when interrupted by another command.
 */
class DriveWithJoystick : public frc::Command {
 public:
  DriveWithJoystick();
  void Execute() override;
  bool IsFinished() override;
  void End() override;
};
