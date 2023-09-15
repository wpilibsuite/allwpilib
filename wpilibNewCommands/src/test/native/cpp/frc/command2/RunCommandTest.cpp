// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc/command2/RunCommand.h"

class RunCommandTest : public CommandTestBase {};

TEST_F(RunCommandTest, RunCommandSchedule) {
  frc::CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  frc::RunCommand command([&counter] { counter++; }, {});

  scheduler.Schedule(&command);
  scheduler.Run();
  scheduler.Run();
  scheduler.Run();

  EXPECT_EQ(3, counter);
}
