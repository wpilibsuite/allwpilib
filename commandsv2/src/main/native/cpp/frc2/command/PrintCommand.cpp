// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/PrintCommand.hpp"

#include <string>

#include "wpi/util/print.hpp"

using namespace wpi::cmd;

PrintCommand::PrintCommand(std::string_view message)
    : CommandHelper{
          [str = std::string(message)] { wpi::util::print("{}\n", str); },
          {}} {}

bool PrintCommand::RunsWhenDisabled() const {
  return true;
}
