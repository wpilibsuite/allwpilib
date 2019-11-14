#pragma once

#include <frc2/command/ProfiledPIDCommand.h>
#include <frc2/command/CommandHelper.h>

class ReplaceMeProfiledPIDCommand
    : public frc2::CommandHelper<frc2::ProfiledPIDCommand, ReplaceMeProfiledPIDCommand> {
 public:
  ReplaceMeProfiledPIDCommand();

  bool IsFinished() override;
};