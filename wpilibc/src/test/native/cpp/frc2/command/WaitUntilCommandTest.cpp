/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CommandTestBase.h"
#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;
class WaitUntilCommandTest : public CommandTestBase {};

TEST_F(WaitUntilCommandTest, WaitUntilCommandScheduleTest) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  WaitUntilCommand command([&finished] { return finished; });

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
