#pragma once

#include <frc2/command/PIDCommand.h>
#include <frc2/command/CommandHelper.h>

class ReplaceMePIDCommand
    : public frc2::CommandHelper<frc2::PIDCommand, ReplaceMePIDCommand> {
 public:
  ReplaceMePIDCommand();

  bool IsFinished() override;
};