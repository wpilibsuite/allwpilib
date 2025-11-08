// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/commands2/CommandHelper.hpp>
#include <wpi/commands2/SequentialCommandGroup.hpp>

#include "commands/DriveTime.hpp"
#include "commands/TurnTime.hpp"
#include "subsystems/Drivetrain.hpp"

class AutonomousTime
    : public wpi::cmd::CommandHelper<wpi::cmd::SequentialCommandGroup,
                                     AutonomousTime> {
 public:
  /**
   * Creates a new Autonomous Drive based on time. This will drive out for a
   * period of time, turn around for time (equivalent to time to turn around)
   * and drive forward again. This should mimic driving out, turning around and
   * driving back.
   *
   * @param drive The drive subsystem on which this command will run
   */
  explicit AutonomousTime(Drivetrain* drive) {
    AddCommands(DriveTime(-0.6, 2_s, drive), TurnTime(-0.5, 1.3_s, drive),
                DriveTime(-0.6, 2_s, drive), TurnTime(0.5, 1.3_s, drive));
  }
};
