// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc/command2/InstantCommand.h"
#include "frc/command2/RunCommand.h"
#include "frc/command2/StartEndCommand.h"

class SchedulerTest : public CommandTestBase {};

TEST_F(SchedulerTest, SchedulerLambdaTestNoInterrupt) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::InstantCommand command;

  int counter = 0;

  scheduler.OnCommandInitialize([&counter](const frc::Command&) { counter++; });
  scheduler.OnCommandExecute([&counter](const frc::Command&) { counter++; });
  scheduler.OnCommandFinish([&counter](const frc::Command&) { counter++; });

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_EQ(counter, 3);
}

TEST_F(SchedulerTest, SchedulerLambdaInterrupt) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::RunCommand command([] {}, {});

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const frc::Command&) { counter++; });

  scheduler.Schedule(&command);
  scheduler.Run();
  scheduler.Cancel(&command);

  EXPECT_EQ(counter, 1);
}

TEST_F(SchedulerTest, SchedulerLambdaInterruptNoCause) {
  frc::CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  scheduler.OnCommandInterrupt(
      [&counter](const frc::Command&,
                 const std::optional<frc::Command*>& interruptor) {
        EXPECT_FALSE(interruptor);
        counter++;
      });

  frc::RunCommand command([] {});

  scheduler.Schedule(&command);
  scheduler.Cancel(&command);

  EXPECT_EQ(1, counter);
}

TEST_F(SchedulerTest, SchedulerLambdaInterruptCause) {
  frc::CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  TestSubsystem subsystem{};
  frc::RunCommand command([] {}, {&subsystem});
  frc::InstantCommand interruptor([] {}, {&subsystem});

  scheduler.OnCommandInterrupt(
      [&](const frc::Command&, const std::optional<frc::Command*>& cause) {
        ASSERT_TRUE(cause);
        EXPECT_EQ(&interruptor, *cause);
        counter++;
      });

  scheduler.Schedule(&command);
  scheduler.Schedule(&interruptor);

  EXPECT_EQ(1, counter);
}

TEST_F(SchedulerTest, SchedulerLambdaInterruptCauseInRunLoop) {
  frc::CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  TestSubsystem subsystem{};
  frc::RunCommand command([] {}, {&subsystem});
  frc::InstantCommand interruptor([] {}, {&subsystem});
  // This command will schedule interruptor in execute() inside the run loop
  frc::InstantCommand interruptorScheduler(
      [&] { scheduler.Schedule(&interruptor); });

  scheduler.OnCommandInterrupt(
      [&](const frc::Command&, const std::optional<frc::Command*>& cause) {
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
  frc::CommandScheduler scheduler = GetScheduler();

  int counter = 0;
  TestSubsystem system{[&counter] { counter++; }};

  EXPECT_NO_FATAL_FAILURE(scheduler.RegisterSubsystem(&system));

  scheduler.Run();
  EXPECT_EQ(counter, 1);
}

TEST_F(SchedulerTest, UnregisterSubsystem) {
  frc::CommandScheduler scheduler = GetScheduler();

  int counter = 0;
  TestSubsystem system{[&counter] { counter++; }};

  scheduler.RegisterSubsystem(&system);

  EXPECT_NO_FATAL_FAILURE(scheduler.UnregisterSubsystem(&system));

  scheduler.Run();
  ASSERT_EQ(counter, 0);
}

TEST_F(SchedulerTest, SchedulerCancelAll) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::RunCommand command([] {}, {});
  frc::RunCommand command2([] {}, {});

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const frc::Command&) { counter++; });
  scheduler.OnCommandInterrupt(
      [](const frc::Command&, const std::optional<frc::Command*>& interruptor) {
        EXPECT_FALSE(interruptor);
      });

  scheduler.Schedule(&command);
  scheduler.Schedule(&command2);
  scheduler.Run();
  scheduler.CancelAll();

  EXPECT_EQ(counter, 2);
}

TEST_F(SchedulerTest, ScheduleScheduledNoOp) {
  frc::CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  frc::StartEndCommand command([&counter] { counter++; }, [] {});

  scheduler.Schedule(&command);
  scheduler.Schedule(&command);

  EXPECT_EQ(counter, 1);
}
