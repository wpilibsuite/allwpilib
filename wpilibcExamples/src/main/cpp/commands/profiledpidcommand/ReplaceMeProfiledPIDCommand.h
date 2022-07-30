// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command/CommandHelper.h>
#include <frc/command/ProfiledPIDCommand.h>
#include <units/length.h>

class ReplaceMeProfiledPIDCommand
    : public frc::CommandHelper<frc::ProfiledPIDCommand<units::meters>,
                                ReplaceMeProfiledPIDCommand> {
 public:
  ReplaceMeProfiledPIDCommand();

  bool IsFinished() override;
};
