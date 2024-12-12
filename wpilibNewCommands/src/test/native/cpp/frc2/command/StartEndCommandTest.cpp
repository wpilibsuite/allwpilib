// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc2/command/Commands.h>

#include "CommandTestBase.h"
#include "frc2/command/StartEndCommand.h"

using namespace frc2;
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
