// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command/CommandHelper.h>
#include <frc/command/ParallelDeadlineGroup.h>

class ReplaceMeParallelDeadlineGroup
    : public frc::CommandHelper<frc::ParallelDeadlineGroup,
                                ReplaceMeParallelDeadlineGroup> {
 public:
  ReplaceMeParallelDeadlineGroup();
};
