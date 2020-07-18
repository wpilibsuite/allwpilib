/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/simulation/SimHooks.h>

#include "CommandTestBase.h"
#include "frc2/command/WaitCommand.h"
#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;
class WaitCommandTest : public CommandTestBase {};

TEST_F(WaitCommandTest, WaitCommandScheduleTest) {
  frc::sim::PauseTiming();

  CommandScheduler scheduler = GetScheduler();

  WaitCommand command(100_ms);

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  frc::sim::StepTiming(110_ms);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  frc::sim::ResumeTiming();
}
