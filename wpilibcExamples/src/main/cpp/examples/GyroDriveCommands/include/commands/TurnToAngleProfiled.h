// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/ProfiledPIDCommand.h>

#include "subsystems/DriveSubsystem.h"

/**
 * A command that will turn the robot to the specified angle using a motion
 * profile.
 */
class TurnToAngleProfiled
    : public frc2::CommandHelper<frc2::ProfiledPIDCommand<units::radians>,
                                 TurnToAngleProfiled> {
 public:
  /**
   * Turns to robot to the specified angle using a motion profile.
   *
   * @param targetAngleDegrees The angle to turn to
   * @param drive              The drive subsystem to use
   */
  TurnToAngleProfiled(units::degree_t targetAngleDegrees,
                      DriveSubsystem* drive);

  bool IsFinished() override;
};
