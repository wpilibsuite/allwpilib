/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/TrapezoidProfileCommand.h>
#include <units/length.h>

class ReplaceMeTrapezoidProfileCommand
    : public frc2::CommandHelper<frc2::TrapezoidProfileCommand<units::meters>,
                                 ReplaceMeTrapezoidProfileCommand> {
 public:
  ReplaceMeTrapezoidProfileCommand();
};
