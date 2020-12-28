// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/PrintCommand.h"

#include <wpi/raw_ostream.h>

using namespace frc;

PrintCommand::PrintCommand(const wpi::Twine& message)
    : InstantCommand("Print \"" + message + wpi::Twine('"')) {
  m_message = message.str();
}

void PrintCommand::Initialize() {
  wpi::outs() << m_message << '\n';
}
