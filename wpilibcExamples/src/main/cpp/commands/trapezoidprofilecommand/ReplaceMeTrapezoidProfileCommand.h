#pragma once

#include <frc2/command/TrapezoidProfileCommand.h>
#include <frc2/command/CommandHelper.h>

class ReplaceMeTrapezoidProfileCommand
    : public frc2::CommandHelper<frc2::TrapezoidProfileCommand, ReplaceMeTrapezoidProfileCommand> {
 public:
  ReplaceMeTrapezoidProfileCommand();
};
