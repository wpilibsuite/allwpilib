// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/SimHooks.h>

#include "CommandTestBase.h"
#include "frc/command2/ConditionalCommand.h"
#include "frc/command2/FunctionalCommand.h"
#include "frc/command2/InstantCommand.h"
#include "frc/command2/ParallelRaceGroup.h"
#include "frc/command2/RunCommand.h"
#include "frc/command2/SequentialCommandGroup.h"

class CommandDecoratorTest : public CommandTestBase {};

TEST_F(CommandDecoratorTest, WithTimeout) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::sim::PauseTiming();

  auto command = frc::RunCommand([] {}, {}).WithTimeout(100_ms);

  scheduler.Schedule(command);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(command));

  frc::sim::StepTiming(150_ms);

  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));

  frc::sim::ResumeTiming();
}

TEST_F(CommandDecoratorTest, Until) {
  frc::CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command =
      frc::RunCommand([] {}, {}).Until([&finished] { return finished; });

  scheduler.Schedule(command);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(command));

  finished = true;

  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));
}

TEST_F(CommandDecoratorTest, OnlyWhile) {
  frc::CommandScheduler scheduler = GetScheduler();

  bool run = true;

  auto command = frc::RunCommand([] {}, {}).OnlyWhile([&run] { return run; });

  scheduler.Schedule(command);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(command));

  run = false;

  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));
}

TEST_F(CommandDecoratorTest, IgnoringDisable) {
  frc::CommandScheduler scheduler = GetScheduler();

  auto command = frc::RunCommand([] {}, {}).IgnoringDisable(true);

  SetDSEnabled(false);

  scheduler.Schedule(command);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(command));
}

TEST_F(CommandDecoratorTest, BeforeStarting) {
  frc::CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = frc::InstantCommand([] {}, {}).BeforeStarting(
      [&finished] { finished = true; });

  scheduler.Schedule(command);

  EXPECT_TRUE(finished);

  scheduler.Run();
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(command));
}

TEST_F(CommandDecoratorTest, AndThen) {
  frc::CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command =
      frc::InstantCommand([] {}, {}).AndThen([&finished] { finished = true; });

  scheduler.Schedule(command);

  EXPECT_FALSE(finished);

  scheduler.Run();
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(command));
  EXPECT_TRUE(finished);
}

TEST_F(CommandDecoratorTest, Unless) {
  frc::CommandScheduler scheduler = GetScheduler();

  bool hasRun = false;
  bool unlessBool = true;

  auto command = frc::InstantCommand([&hasRun] { hasRun = true; }, {})
                     .Unless([&unlessBool] { return unlessBool; });

  scheduler.Schedule(command);
  scheduler.Run();
  EXPECT_FALSE(hasRun);

  unlessBool = false;
  scheduler.Schedule(command);
  scheduler.Run();
  EXPECT_TRUE(hasRun);
}

TEST_F(CommandDecoratorTest, OnlyIf) {
  frc::CommandScheduler scheduler = GetScheduler();

  bool hasRun = false;
  bool onlyIfBool = false;

  auto command = frc::InstantCommand([&hasRun] { hasRun = true; }, {})
                     .OnlyIf([&onlyIfBool] { return onlyIfBool; });

  scheduler.Schedule(command);
  scheduler.Run();
  EXPECT_FALSE(hasRun);

  onlyIfBool = true;
  scheduler.Schedule(command);
  scheduler.Run();
  EXPECT_TRUE(hasRun);
}

TEST_F(CommandDecoratorTest, FinallyDo) {
  frc::CommandScheduler scheduler = GetScheduler();
  int first = 0;
  int second = 0;
  frc::CommandPtr command = frc::FunctionalCommand([] {}, [] {},
                                                   [&first](bool interrupted) {
                                                     if (!interrupted) {
                                                       first++;
                                                     }
                                                   },
                                                   [] { return true; })
                                .FinallyDo([&first, &second](bool interrupted) {
                                  if (!interrupted) {
                                    // to differentiate between "didn't run" and
                                    // "ran before command's `end()`
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
  frc::CommandScheduler scheduler = GetScheduler();
  int first = 0;
  int second = 0;
  frc::CommandPtr command = frc::FunctionalCommand([] {}, [] {},
                                                   [&first](bool interrupted) {
                                                     if (interrupted) {
                                                       first++;
                                                     }
                                                   },
                                                   [] { return false; })
                                .HandleInterrupt([&first, &second] {
                                  // to differentiate between "didn't run" and
                                  // "ran before command's `end()`
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

TEST_F(CommandDecoratorTest, WithName) {
  frc::InstantCommand command;
  std::string name{"Named"};
  frc::CommandPtr named = std::move(command).WithName(name);
  EXPECT_EQ(name, named.get()->GetName());
}
