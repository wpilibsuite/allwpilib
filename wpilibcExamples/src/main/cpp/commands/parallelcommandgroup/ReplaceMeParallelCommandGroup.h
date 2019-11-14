#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/ParallelCommandGroup.h>

class ReplaceMeParallelCommandGroup
    : public frc2::CommandHelper<frc2::ParallelCommandGroup, ReplaceMeParallelCommandGroup> {
 public:
  ReplaceMeParallelCommandGroup();
};