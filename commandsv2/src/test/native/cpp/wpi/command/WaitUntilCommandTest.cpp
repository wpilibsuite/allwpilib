// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/WaitUntilCommand.hpp"

using namespace frc2;
class WaitUntilCommandTest : public CommandTestBase {};

TEST_F(WaitUntilCommandTest, WaitUntilCommandSchedule) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = cmd::WaitUntil([&finished] { return finished; });

  scheduler.Schedule(command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));
}
