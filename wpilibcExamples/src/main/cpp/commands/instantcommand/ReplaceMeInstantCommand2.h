// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command2/CommandHelper.h>
#include <frc/command2/InstantCommand.h>

class ReplaceMeInstantCommand2
    : public frc::CommandHelper<frc::InstantCommand, ReplaceMeInstantCommand2> {
 public:
  ReplaceMeInstantCommand2();

  void Initialize() override;
};
