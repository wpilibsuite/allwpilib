// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/PrintCommand.h"

#include <fmt/format.h>
#include <wpi/raw_ostream.h>

using namespace frc;

PrintCommand::PrintCommand(std::string_view message)
    : InstantCommand(fmt::format("Print \"{}\"", message)) {
  m_message = message;
}

void PrintCommand::Initialize() {
  wpi::outs() << m_message << '\n';
}
