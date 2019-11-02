/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CommandTestBase.h"
#include "frc2/command/FunctionalCommand.h"

using namespace frc2;
class FunctionalCommandTest : public CommandTestBase {};

TEST_F(FunctionalCommandTest, FunctionalCommandScheduleTest) {
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
