#pragma once

#include <frc2/command/InstantCommand.h>
#include <frc2/command/CommandHelper.h>

class ReplaceMeInstantCommand : public frc2::CommandHelper<frc2::InstantCommand,
    ReplaceMeInstantCommand> {
 public:
  ReplaceMeInstantCommand();

  void Initialize() override;
};