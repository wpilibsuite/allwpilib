// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/commands/CommandGroup.h>

/**
 * Drive over the line and then shoot the ball. If the hot goal is not detected,
 * it will wait briefly.
 */
class DriveAndShootAutonomous : public frc::CommandGroup {
 public:
  DriveAndShootAutonomous();
};
