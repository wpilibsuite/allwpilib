/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"

using namespace frc2;
class InstantCommandTest : public CommandTestBase {};

TEST_F(InstantCommandTest, InstantCommandScheduleTest) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  InstantCommand command([&counter] { counter++; }, {});

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  EXPECT_EQ(counter, 1);
}
