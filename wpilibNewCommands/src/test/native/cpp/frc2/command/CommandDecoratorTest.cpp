// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/SimHooks.h>

#include "CommandTestBase.h"
#include "frc2/command/ConditionalCommand.h"
#include "frc2/command/FunctionalCommand.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/PerpetualCommand.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/SequentialCommandGroup.h"

using namespace frc2;
class CommandDecoratorTest : public CommandTestBase {};

TEST_F(CommandDecoratorTest, WithTimeout) {
  CommandScheduler scheduler = GetScheduler();

  frc::sim::PauseTiming();

  auto command = RunCommand([] {}, {}).WithTimeout(100_ms);

  scheduler.Schedule(command);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(command));

  frc::sim::StepTiming(150_ms);

  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));

  frc::sim::ResumeTiming();
}

TEST_F(CommandDecoratorTest, Until) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = RunCommand([] {}, {}).Until([&finished] { return finished; });

  scheduler.Schedule(command);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(command));

  finished = true;

  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));
}

TEST_F(CommandDecoratorTest, IgnoringDisable) {
  CommandScheduler scheduler = GetScheduler();

  auto command = RunCommand([] {}, {}).IgnoringDisable(true);

  SetDSEnabled(false);

  scheduler.Schedule(command);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(command));
}

TEST_F(CommandDecoratorTest, BeforeStarting) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = InstantCommand([] {}, {}).BeforeStarting(
      [&finished] { finished = true; });

  scheduler.Schedule(command);

  EXPECT_TRUE(finished);

  scheduler.Run();
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(command));
}

TEST_F(CommandDecoratorTest, AndThen) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command =
      InstantCommand([] {}, {}).AndThen([&finished] { finished = true; });

  scheduler.Schedule(command);

  EXPECT_FALSE(finished);

  scheduler.Run();
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(command));
  EXPECT_TRUE(finished);
}

TEST_F(CommandDecoratorTest, Perpetually) {
  CommandScheduler scheduler = GetScheduler();

  WPI_IGNORE_DEPRECATED
  auto command = InstantCommand([] {}, {}).Perpetually();
  WPI_UNIGNORE_DEPRECATED

  scheduler.Schedule(&command);

  scheduler.Run();
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(&command));
}

TEST_F(CommandDecoratorTest, Unless) {
  CommandScheduler scheduler = GetScheduler();

  bool hasRun = false;
  bool unlessBool = true;

  auto command =
      InstantCommand([&hasRun] { hasRun = true; }, {}).Unless([&unlessBool] {
        return unlessBool;
      });

  scheduler.Schedule(command);
  scheduler.Run();
  EXPECT_FALSE(hasRun);

  unlessBool = false;
  scheduler.Schedule(command);
  scheduler.Run();
  EXPECT_TRUE(hasRun);
}

TEST_F(CommandDecoratorTest, FinallyDo) {
  CommandScheduler scheduler = GetScheduler();
  int first = 0;
  int second = 0;
  CommandPtr command = FunctionalCommand([] {}, [] {},
                                         [&first](bool interrupted) {
                                           if (!interrupted) {
                                             first++;
                                           }
                                         },
                                         [] { return true; })
                           .FinallyDo([&first, &second](bool interrupted) {
                             if (!interrupted) {
                               // to differentiate between "didn't run" and "ran
                               // before command's `end()`
                               second += 1 + first;
                             }
                           });

  scheduler.Schedule(command);
  EXPECT_EQ(0, first);
  EXPECT_EQ(0, second);
  scheduler.Run();
  EXPECT_EQ(1, first);
  // if `second == 0`, neither of the lambdas ran.
  // if `second == 1`, the second lambda ran before the first one
  EXPECT_EQ(2, second);
}

// handleInterruptTest() implicitly tests the interrupt=true branch of
// finallyDo()
TEST_F(CommandDecoratorTest, HandleInterrupt) {
  CommandScheduler scheduler = GetScheduler();
  int first = 0;
  int second = 0;
  CommandPtr command = FunctionalCommand([] {}, [] {},
                                         [&first](bool interrupted) {
                                           if (interrupted) {
                                             first++;
                                           }
                                         },
                                         [] { return false; })
                           .HandleInterrupt([&first, &second] {
                             // to differentiate between "didn't run" and "ran
                             // before command's `end()`
                             second += 1 + first;
                           });

  scheduler.Schedule(command);
  scheduler.Run();
  EXPECT_EQ(0, first);
  EXPECT_EQ(0, second);

  scheduler.Cancel(command);
  // if `second == 0`, neither of the lambdas ran.
  // if `second == 1`, the second lambda ran before the first one
  EXPECT_EQ(2, second);
}
