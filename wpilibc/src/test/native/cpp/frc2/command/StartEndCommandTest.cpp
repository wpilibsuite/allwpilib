/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CommandTestBase.h"
#include "frc2/command/StartEndCommand.h"

using namespace frc2;
class StartEndCommandTest : public CommandTestBase {};

TEST_F(StartEndCommandTest, StartEndCommandScheduleTest) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  StartEndCommand command([&counter] { counter++; }, [&counter] { counter++; },
                          {});

  scheduler.Schedule(&command);
  scheduler.Run();
  scheduler.Run();
  scheduler.Cancel(&command);

  EXPECT_EQ(2, counter);
}
