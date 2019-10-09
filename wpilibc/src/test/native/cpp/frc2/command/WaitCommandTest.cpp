/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CommandTestBase.h"
#include "frc2/command/WaitCommand.h"
#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;
class WaitCommandTest : public CommandTestBase {};

TEST_F(WaitCommandTest, WaitCommandScheduleTest) {
  CommandScheduler scheduler = GetScheduler();

  WaitCommand command(.1_s);

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  std::this_thread::sleep_for(std::chrono::milliseconds(110));
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
