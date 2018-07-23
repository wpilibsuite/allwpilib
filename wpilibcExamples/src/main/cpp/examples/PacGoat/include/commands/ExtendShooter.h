/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
