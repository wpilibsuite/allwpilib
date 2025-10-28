// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/commands2/Commands.hpp>

#include "CommandTestBase.hpp"
#include "wpi/commands2/StartEndCommand.hpp"

using namespace wpi::cmd;
class StartEndCommandTest : public CommandTestBase {};

TEST_F(StartEndCommandTest, StartEndCommandSchedule) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  auto command =
      cmd::StartEnd([&counter] { counter++; }, [&counter] { counter++; });

  scheduler.Schedule(command);
  scheduler.Run();
  scheduler.Run();
  scheduler.Cancel(command);

  EXPECT_EQ(2, counter);
}
