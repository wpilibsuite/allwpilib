// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/SimHooks.h>

#include "CommandTestBase.h"
#include "frc/command2/WaitCommand.h"
#include "frc/command2/WaitUntilCommand.h"

class WaitCommandTest : public CommandTestBase {};

TEST_F(WaitCommandTest, WaitCommandSchedule) {
  frc::sim::PauseTiming();

  frc::CommandScheduler scheduler = GetScheduler();

  frc::WaitCommand command(100_ms);

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  frc::sim::StepTiming(110_ms);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  frc::sim::ResumeTiming();
}
