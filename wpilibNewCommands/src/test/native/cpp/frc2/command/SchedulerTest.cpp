// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc2/command/Commands.h>

#include <utility>

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/StartEndCommand.h"

using namespace frc2;
class SchedulerTest : public CommandTestBase {};

TEST_F(SchedulerTest, SchedulerLambdaTestNoInterrupt) {
  CommandScheduler scheduler = GetScheduler();

  auto command = cmd::None();

  int counter = 0;

  scheduler.OnCommandInitialize([&counter](const Command&) { counter++; });
  scheduler.OnCommandExecute([&counter](const Command&) { counter++; });
  scheduler.OnCommandFinish([&counter](const Command&) { counter++; });

  scheduler.Schedule(command);
  scheduler.Run();

  EXPECT_EQ(counter, 3);
}

TEST_F(SchedulerTest, SchedulerLambdaInterrupt) {
  CommandScheduler scheduler = GetScheduler();

  auto command = cmd::Idle();

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const Command&) { counter++; });

  scheduler.Schedule(command);
  scheduler.Run();
  scheduler.Cancel(command);

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

  auto command = cmd::Idle();

  scheduler.Schedule(command);
  scheduler.Cancel(command);

  EXPECT_EQ(1, counter);
}

TEST_F(SchedulerTest, SchedulerLambdaInterruptCause) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  TestSubsystem subsystem{};
  auto command = cmd::Idle({&subsystem});
  InstantCommand interruptor([] {}, {&subsystem});

  scheduler.OnCommandInterrupt(
      [&](const Command&, const std::optional<Command*>& cause) {
        ASSERT_TRUE(cause);
        EXPECT_EQ(&interruptor, *cause);
        counter++;
      });

  scheduler.Schedule(command);
  scheduler.Schedule(&interruptor);

  EXPECT_EQ(1, counter);
}

TEST_F(SchedulerTest, SchedulerLambdaInterruptCauseInRunLoop) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  TestSubsystem subsystem{};
  auto command = cmd::Idle({&subsystem});
  InstantCommand interruptor([] {}, {&subsystem});
  // This command will schedule interruptor in execute() inside the run loop
  auto interruptorScheduler =
      cmd::RunOnce([&] { scheduler.Schedule(&interruptor); });

  scheduler.OnCommandInterrupt(
      [&](const Command&, const std::optional<Command*>& cause) {
        ASSERT_TRUE(cause);
        EXPECT_EQ(&interruptor, *cause);
        counter++;
      });

  scheduler.Schedule(command);
  scheduler.Schedule(interruptorScheduler);

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

  auto command1 = cmd::Idle();
  auto command2 = cmd::Idle();

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const Command&) { counter++; });
  scheduler.OnCommandInterrupt(
      [](const Command&, const std::optional<Command*>& interruptor) {
        EXPECT_FALSE(interruptor);
      });

  scheduler.Schedule(command1);
  scheduler.Schedule(command2);
  scheduler.Run();
  scheduler.CancelAll();

  EXPECT_EQ(counter, 2);
}

TEST_F(SchedulerTest, ScheduleScheduledNoOp) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  auto command = cmd::StartEnd([&counter] { counter++; }, [] {});

  scheduler.Schedule(command);
  scheduler.Schedule(command);

  EXPECT_EQ(counter, 1);
}

class TrackDestroyCommand
    : public frc2::CommandHelper<Command, TrackDestroyCommand> {
 public:
  explicit TrackDestroyCommand(wpi::unique_function<void()> deleteFunc)
      : m_deleteFunc{std::move(deleteFunc)} {}
  TrackDestroyCommand(TrackDestroyCommand&& other)
      : m_deleteFunc{std::exchange(other.m_deleteFunc, [] {})} {}
  TrackDestroyCommand& operator=(TrackDestroyCommand&& other) {
    m_deleteFunc = std::exchange(other.m_deleteFunc, [] {});
    return *this;
  }
  ~TrackDestroyCommand() override { m_deleteFunc(); }

 private:
  wpi::unique_function<void()> m_deleteFunc;
};

TEST_F(SchedulerTest, ScheduleCommandPtr) {
  CommandScheduler scheduler = GetScheduler();
  int destructionCounter = 0;
  int runCounter = 0;

  bool finish = false;

  {
    auto commandPtr =
        TrackDestroyCommand([&destructionCounter] { destructionCounter++; })
            .AlongWith(
                frc2::InstantCommand([&runCounter] { runCounter++; }).ToPtr())
            .Until([&finish] { return finish; });
    EXPECT_EQ(destructionCounter, 0) << "Composition should not delete command";

    scheduler.Schedule(std::move(commandPtr));
    EXPECT_EQ(destructionCounter, 0)
        << "Scheduling should not delete CommandPtr";
  }
  EXPECT_EQ(destructionCounter, 0)
      << "Scheduler should own CommandPtr after scheduling";
  scheduler.Run();
  EXPECT_EQ(runCounter, 1);
  EXPECT_EQ(destructionCounter, 0) << "Scheduler should not destroy CommandPtr "
                                      "until command lifetime is complete";
  finish = true;
  scheduler.Run();
  EXPECT_EQ(runCounter, 1);
  EXPECT_EQ(destructionCounter, 1)
      << "Scheduler should delete command after command completes";
}
