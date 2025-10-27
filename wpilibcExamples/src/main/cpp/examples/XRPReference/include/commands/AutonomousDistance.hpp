// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/SequentialCommandGroup.h>

#include "commands/DriveDistance.h"
#include "commands/TurnDegrees.h"
#include "subsystems/Drivetrain.h"

class AutonomousDistance
    : public frc2::CommandHelper<frc2::SequentialCommandGroup,
                                 AutonomousDistance> {
 public:
  /**
   * Creates a new Autonomous Drive based on distance. This will drive out for a
   * specified distance, turn around and drive back.
   *
   * @param drive The drivetrain subsystem on which this command will run
   */
  explicit AutonomousDistance(Drivetrain* drive) {
    AddCommands(
        DriveDistance(-0.5, 10_in, drive), TurnDegrees(-0.5, 180_deg, drive),
        DriveDistance(-0.5, 10_in, drive), TurnDegrees(0.5, 180_deg, drive));
  }
};
