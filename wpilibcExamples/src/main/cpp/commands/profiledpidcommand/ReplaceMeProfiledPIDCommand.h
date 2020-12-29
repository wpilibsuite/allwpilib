// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/ProfiledPIDCommand.h>
#include <units/length.h>

class ReplaceMeProfiledPIDCommand
    : public frc2::CommandHelper<frc2::ProfiledPIDCommand<units::meters>,
                                 ReplaceMeProfiledPIDCommand> {
 public:
  ReplaceMeProfiledPIDCommand();

  bool IsFinished() override;
};
