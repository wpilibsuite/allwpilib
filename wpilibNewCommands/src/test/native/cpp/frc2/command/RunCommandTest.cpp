// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc2/command/Commands.h>

#include "CommandTestBase.h"
#include "frc2/command/RunCommand.h"

using namespace frc2;
class RunCommandTest : public CommandTestBase {};

TEST_F(RunCommandTest, RunCommandSchedule) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  auto command = cmd::Run([&counter] { counter++; });

  scheduler.Schedule(command);
  scheduler.Run();
  scheduler.Run();
  scheduler.Run();

  EXPECT_EQ(3, counter);
}
