// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc2/command/FunctionalCommand.h"

using namespace frc2;
class FunctionalCommandTest : public CommandTestBase {};

TEST_F(FunctionalCommandTest, FunctionalCommandSchedule) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;
  bool finished = false;

  FunctionalCommand command(
      [&counter] { counter++; }, [&counter] { counter++; },
      [&counter](bool) { counter++; }, [&finished] { return finished; });

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  EXPECT_EQ(4, counter);
}
