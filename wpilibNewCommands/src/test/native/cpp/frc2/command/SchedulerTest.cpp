// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/StartEndCommand.h"

using namespace frc2;
class SchedulerTest : public CommandTestBase {};

TEST_F(SchedulerTest, SchedulerLambdaTestNoInterrupt) {
  CommandScheduler scheduler = GetScheduler();

  InstantCommand command;

  int counter = 0;

  scheduler.OnCommandInitialize([&counter](const Command&) { counter++; });
  scheduler.OnCommandExecute([&counter](const Command&) { counter++; });
  scheduler.OnCommandFinish([&counter](const Command&) { counter++; });

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_EQ(counter, 3);
}

TEST_F(SchedulerTest, SchedulerLambdaInterrupt) {
  CommandScheduler scheduler = GetScheduler();

  RunCommand command([] {}, {});

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const Command&) { counter++; });

  scheduler.Schedule(&command);
  scheduler.Run();
  scheduler.Cancel(&command);

  EXPECT_EQ(counter, 1);
}

TEST_F(SchedulerTest, SchedulerLambdaInterruptNoCause) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  scheduler.OnCommandInterrupt(
      [&counter](const Command&, const std::optional<Command*>& interruptor) {
        EXPECT_FALSE(interruptor);
        counter++;
      });

  RunCommand command([] {});

  scheduler.Schedule(&command);
  scheduler.Cancel(&command);

  EXPECT_EQ(1, counter);
}

TEST_F(SchedulerTest, SchedulerLambdaInterruptCause) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  TestSubsystem subsystem{};
  RunCommand command([] {}, {&subsystem});
  InstantCommand interruptor([] {}, {&subsystem});

  scheduler.OnCommandInterrupt(
      [&](const Command&, const std::optional<Command*>& cause) {
        ASSERT_TRUE(cause);
        EXPECT_EQ(&interruptor, *cause);
        counter++;
      });

  scheduler.Schedule(&command);
  scheduler.Schedule(&interruptor);

  EXPECT_EQ(1, counter);
}

TEST_F(SchedulerTest, SchedulerLambdaInterruptCauseInRunLoop) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  TestSubsystem subsystem{};
  RunCommand command([] {}, {&subsystem});
  InstantCommand interruptor([] {}, {&subsystem});
  // This command will schedule interruptor in execute() inside the run loop
  InstantCommand interruptorScheduler(
      [&] { scheduler.Schedule(&interruptor); });

  scheduler.OnCommandInterrupt(
      [&](const Command&, const std::optional<Command*>& cause) {
        ASSERT_TRUE(cause);
        EXPECT_EQ(&interruptor, *cause);
        counter++;
      });

  scheduler.Schedule(&command);
  scheduler.Schedule(&interruptorScheduler);

  scheduler.Run();

  EXPECT_EQ(1, counter);
}

TEST_F(SchedulerTest, RegisterSubsystem) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;
  TestSubsystem system{[&counter] { counter++; }};

  EXPECT_NO_FATAL_FAILURE(scheduler.RegisterSubsystem(&system));

  scheduler.Run();
  EXPECT_EQ(counter, 1);
}

TEST_F(SchedulerTest, UnregisterSubsystem) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;
  TestSubsystem system{[&counter] { counter++; }};

  scheduler.RegisterSubsystem(&system);

  EXPECT_NO_FATAL_FAILURE(scheduler.UnregisterSubsystem(&system));

  scheduler.Run();
  ASSERT_EQ(counter, 0);
}

TEST_F(SchedulerTest, SchedulerCancelAll) {
  CommandScheduler scheduler = GetScheduler();

  RunCommand command([] {}, {});
  RunCommand command2([] {}, {});

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const Command&) { counter++; });
  scheduler.OnCommandInterrupt(
      [](const Command&, const std::optional<Command*>& interruptor) {
        EXPECT_FALSE(interruptor);
      });

  scheduler.Schedule(&command);
  scheduler.Schedule(&command2);
  scheduler.Run();
  scheduler.CancelAll();

  EXPECT_EQ(counter, 2);
}

TEST_F(SchedulerTest, ScheduleScheduledNoOp) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  StartEndCommand command([&counter] { counter++; }, [] {});

  scheduler.Schedule(&command);
  scheduler.Schedule(&command);

  EXPECT_EQ(counter, 1);
}
