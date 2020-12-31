// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/SimHooks.h>

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/PerpetualCommand.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/SequentialCommandGroup.h"

using namespace frc2;
class CommandDecoratorTest : public CommandTestBase {};

TEST_F(CommandDecoratorTest, WithTimeoutTest) {
  CommandScheduler scheduler = GetScheduler();

  frc::sim::PauseTiming();

  auto command = RunCommand([] {}, {}).WithTimeout(100_ms);

  scheduler.Schedule(&command);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));

  frc::sim::StepTiming(150_ms);

  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  frc::sim::ResumeTiming();
}

TEST_F(CommandDecoratorTest, WithInterruptTest) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command =
      RunCommand([] {}, {}).WithInterrupt([&finished] { return finished; });

  scheduler.Schedule(&command);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));

  finished = true;

  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(CommandDecoratorTest, BeforeStartingTest) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = InstantCommand([] {}, {}).BeforeStarting(
      [&finished] { finished = true; });

  scheduler.Schedule(&command);

  EXPECT_TRUE(finished);

  scheduler.Run();
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(CommandDecoratorTest, AndThenTest) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command =
      InstantCommand([] {}, {}).AndThen([&finished] { finished = true; });

  scheduler.Schedule(&command);

  EXPECT_FALSE(finished);

  scheduler.Run();
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&command));
  EXPECT_TRUE(finished);
}

TEST_F(CommandDecoratorTest, PerpetuallyTest) {
  CommandScheduler scheduler = GetScheduler();

  auto command = InstantCommand([] {}, {}).Perpetually();

  scheduler.Schedule(&command);

  scheduler.Run();
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(&command));
}
