// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/InstantCommand.hpp"

using namespace wpi::cmd;

class CommandTest : public CommandTestBase {};

TEST_CASE_METHOD(CommandTest, "CommandTest DefaultSubsystemIsUngrouped",
                 "[commandsv2][command]") {
  InstantCommand command;

  CHECK("Ungrouped" == command.GetSubsystem());
}
