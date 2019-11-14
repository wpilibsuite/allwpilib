#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/ParallelDeadlineGroup.h>

class ReplaceMeParallelDeadlineGroup
    : public frc2::CommandHelper<frc2::ParallelDeadlineGroup, ReplaceMeParallelDeadlineGroup> {
 public:
  ReplaceMeParallelDeadlineGroup();
};