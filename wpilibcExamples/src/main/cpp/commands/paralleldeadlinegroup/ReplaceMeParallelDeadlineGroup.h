// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/ParallelDeadlineGroup.h>

class ReplaceMeParallelDeadlineGroup
    : public frc2::CommandHelper<frc2::ParallelDeadlineGroup,
                                 ReplaceMeParallelDeadlineGroup> {
 public:
  ReplaceMeParallelDeadlineGroup();
};
