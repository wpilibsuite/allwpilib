#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/ProfiledPIDCommand.h>

#include "subsystems/DriveSubsystem.h"

/**
 * A command that will turn the robot to the specified angle using a motion profile.
 */
class TurnToAngle
  : public frc2::CommandHelper<frc2::ProfiledPIDCommand<units::radians>, TurnToAngle> {
 public:
  /**
   * Turns to robot to the specified angle using a motion profile.
   *
   * @param targetAngleDegrees The angle to turn to
   * @param drive              The drive subsystem to use
   */
  TurnToAngle(double targetAngleDegrees, DriveSubsystem* drive);

  bool IsFinished() override;
};

