// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/SimHooks.h>
#include <units/time.h>

#include "CommandTestBase.h"
#include "frc2/command/WaitCommand.h"

using namespace frc2;
class WaitCommandTest : public CommandTestBase {};

TEST_F(WaitCommandTest, WaitCommandSchedule) {
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

TEST_F(WaitCommandTest, WaitCommandScheduleLambda) {
  frc::sim::PauseTiming();

  CommandScheduler scheduler = GetScheduler();

  WaitCommand command([] { return 100_ms; });

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  frc::sim::StepTiming(110_ms);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  frc::sim::ResumeTiming();
}

TEST_F(WaitCommandTest, WaitCommandScheduleNonStaticLambda) {
  int calls = 1;
  frc::sim::PauseTiming();

  CommandScheduler scheduler = GetScheduler();

  WaitCommand command([calls] {
      if (calls == 1) {
        return 100_ms;
      }

      if (calls == 2) {
        return 50_ms;
      }

      return 150_ms;
  });

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  frc::sim::StepTiming(110_ms);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  calls ++;

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  frc::sim::StepTiming(40_ms);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  frc::sim::StepTiming(20_ms);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  calls++;

  frc::sim::ResumeTiming();
}
