// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/PrintCommand.h"

#include <wpi/raw_ostream.h>

using namespace frc2;

PrintCommand::PrintCommand(const wpi::Twine& message)
    : CommandHelper{[str = message.str()] { wpi::outs() << str << "\n"; }, {}} {
}

bool PrintCommand::RunsWhenDisabled() const {
  return true;
}
