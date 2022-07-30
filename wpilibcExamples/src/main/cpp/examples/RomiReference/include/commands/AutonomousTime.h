// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command/CommandHelper.h>
#include <frc/command/SequentialCommandGroup.h>

#include "commands/DriveTime.h"
#include "commands/TurnTime.h"
#include "subsystems/Drivetrain.h"

class AutonomousTime
    : public frc::CommandHelper<frc::SequentialCommandGroup, AutonomousTime> {
 public:
  explicit AutonomousTime(Drivetrain* drive) {
    AddCommands(DriveTime(-0.6, 2_s, drive), TurnTime(-0.5, 1.3_s, drive),
                DriveTime(-0.6, 2_s, drive), TurnTime(0.5, 1.3_s, drive));
  }
};
