#pragma once

#include <frc2/command/CommandBase.h>
#include <frc2/command/CommandHelper.h>

class ReplaceMeCommand : public frc2::CommandHelper<frc2::CommandBase, ReplaceMeCommand> {
 public:
  ReplaceMeCommand();

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;
};