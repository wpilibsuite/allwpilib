/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CommandTestBase.h"
#include "frc2/command/RunCommand.h"

using namespace frc2;
class RunCommandTest : public CommandTestBase {};

TEST_F(RunCommandTest, RunCommandScheduleTest) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  RunCommand command([&counter] { counter++; }, {});

  scheduler.Schedule(&command);
  scheduler.Run();
  scheduler.Run();
  scheduler.Run();

  EXPECT_EQ(3, counter);
}
