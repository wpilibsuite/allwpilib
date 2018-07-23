/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/commands/Command.h>

/**
 * Wait until the collector senses that it has the ball. This command does
 * nothing and is intended to be used in command groups to wait for this
 * condition.
 */
class WaitForBall : public frc::Command {
 public:
  WaitForBall();
  bool IsFinished() override;
};
