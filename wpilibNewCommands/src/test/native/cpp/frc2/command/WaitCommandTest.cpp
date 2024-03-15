// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/SimHooks.h>

#include "CommandTestBase.h"
#include "frc2/command/WaitCommand.h"
#include "frc2/command/WaitUntilCommand.h"

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

TEST_F(WaitCommandTest, WaitCommandScheduleFunction) {
  constexpr auto kWaitTime = 2_s;
  frc::sim::PauseTiming();

  CommandScheduler scheduler = GetScheduler();

  int supplierCounter = 0;

  WaitCommand command{[&supplierCounter, kWaitTime] {
    supplierCounter++;
    return kWaitTime;
  }};

  EXPECT_EQ(0, supplierCounter)
      << "Supplier should not be called at construction.";
  scheduler.Schedule(&command);
  EXPECT_EQ(1, supplierCounter) << "Supplier should be called once and only "
                                   "once when command is scheduled.";

  frc::sim::StepTiming(1_s);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));

  frc::sim::StepTiming(2_s);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  EXPECT_EQ(1, supplierCounter)
      << "Supplier should not be called outside of command initialization.";

  frc::sim::ResumeTiming();
}
