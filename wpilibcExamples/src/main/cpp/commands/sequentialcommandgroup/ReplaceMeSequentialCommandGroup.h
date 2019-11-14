#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/SequentialCommandGroup.h>

class ReplaceMeSequentialCommandGroup
    : public frc2::CommandHelper<frc2::SequentialCommandGroup, ReplaceMeSequentialCommandGroup> {
 public:
  ReplaceMeSequentialCommandGroup();
};