// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "wpi/command/Commands.hpp"
#include "wpi/command/InstantCommand.hpp"

using namespace wpi::cmd;
class InstantCommandTest : public CommandTestBase {};

TEST_F(InstantCommandTest, InstantCommandSchedule) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  auto command = cmd::RunOnce([&counter] { counter++; });

  scheduler.Schedule(command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));
  EXPECT_EQ(counter, 1);
}
