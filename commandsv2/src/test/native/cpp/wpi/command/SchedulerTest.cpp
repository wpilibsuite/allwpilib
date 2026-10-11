// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <numeric>
#include <utility>
#include <vector>

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/InstantCommand.hpp"

using namespace wpi::cmd;
class SchedulerTest : public CommandTestBase {};

TEST_CASE_METHOD(SchedulerTest, "SchedulerTest SchedulerLambdaTestNoInterrupt",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  auto command = None();

  int counter = 0;

  scheduler.OnCommandInitialize([&counter](const Command&) { counter++; });
  scheduler.OnCommandExecute([&counter](const Command&) { counter++; });
  scheduler.OnCommandFinish([&counter](const Command&) { counter++; });

  scheduler.Schedule(command);
  scheduler.Run();

  CHECK(counter == 3);
}

TEST_CASE_METHOD(SchedulerTest, "SchedulerTest SchedulerLambdaInterrupt",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  auto command = Idle();

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const Command&) { counter++; });

  scheduler.Schedule(command);
  scheduler.Run();
  scheduler.Cancel(command);

  CHECK(counter == 1);
}

TEST_CASE_METHOD(SchedulerTest, "SchedulerTest SchedulerLambdaInterruptNoCause",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  scheduler.OnCommandInterrupt(
      [&counter](const Command&, const std::optional<Command*>& interruptor) {
        CHECK_FALSE(interruptor);
        counter++;
      });

  auto command = Idle();

  scheduler.Schedule(command);
  scheduler.Cancel(command);

  CHECK(1 == counter);
}

TEST_CASE_METHOD(SchedulerTest, "SchedulerTest SchedulerLambdaInterruptCause",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  TestSubsystem subsystem{};
  auto command = Idle({&subsystem});
  InstantCommand interruptor([] {}, {&subsystem});

  scheduler.OnCommandInterrupt(
      [&](const Command&, const std::optional<Command*>& cause) {
        REQUIRE(cause);
        CHECK(&interruptor == *cause);
        counter++;
      });

  scheduler.Schedule(command);
  scheduler.Schedule(&interruptor);

  CHECK(1 == counter);
}

TEST_CASE_METHOD(SchedulerTest,
                 "SchedulerTest SchedulerLambdaInterruptCauseInRunLoop",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  TestSubsystem subsystem{};
  auto command = Idle({&subsystem});
  InstantCommand interruptor([] {}, {&subsystem});
  // This command will schedule interruptor in execute() inside the run loop
  auto interruptorScheduler =
      RunOnce([&] { scheduler.Schedule(&interruptor); });

  scheduler.OnCommandInterrupt(
      [&](const Command&, const std::optional<Command*>& cause) {
        REQUIRE(cause);
        CHECK(&interruptor == *cause);
        counter++;
      });

  scheduler.Schedule(command);
  scheduler.Schedule(interruptorScheduler);

  scheduler.Run();

  CHECK(1 == counter);
}

TEST_CASE_METHOD(SchedulerTest, "SchedulerTest RegisterSubsystem",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;
  TestSubsystem system{[&counter] { counter++; }};

  CHECK_NOTHROW(scheduler.RegisterSubsystem(&system));

  scheduler.Run();
  CHECK(counter == 1);
}

TEST_CASE_METHOD(SchedulerTest, "SchedulerTest UnregisterSubsystem",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;
  TestSubsystem system{[&counter] { counter++; }};

  scheduler.RegisterSubsystem(&system);

  CHECK_NOTHROW(scheduler.UnregisterSubsystem(&system));

  scheduler.Run();
  REQUIRE(counter == 0);
}

TEST_CASE_METHOD(SchedulerTest, "SchedulerTest SchedulerCancelAll",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  auto command1 = Idle();
  auto command2 = Idle();

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const Command&) { counter++; });
  scheduler.OnCommandInterrupt(
      [](const Command&, const std::optional<Command*>& interruptor) {
        CHECK_FALSE(interruptor);
      });

  scheduler.Schedule(command1);
  scheduler.Schedule(command2);
  scheduler.Run();
  scheduler.CancelAll();

  CHECK(counter == 2);
}

TEST_CASE_METHOD(SchedulerTest, "SchedulerTest ScheduleScheduledNoOp",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  auto command = StartEnd([&counter] { counter++; }, [] {});

  scheduler.Schedule(command);
  scheduler.Schedule(command);

  CHECK(counter == 1);
}

class TrackDestroyCommand
    : public wpi::cmd::CommandHelper<Command, TrackDestroyCommand> {
 public:
  explicit TrackDestroyCommand(wpi::util::unique_function<void()> deleteFunc)
      : m_deleteFunc{std::move(deleteFunc)} {}
  TrackDestroyCommand(TrackDestroyCommand&& other)
      : m_deleteFunc{std::exchange(other.m_deleteFunc, [] {})} {}
  TrackDestroyCommand& operator=(TrackDestroyCommand&& other) {
    m_deleteFunc = std::exchange(other.m_deleteFunc, [] {});
    return *this;
  }
  ~TrackDestroyCommand() override { m_deleteFunc(); }

 private:
  wpi::util::unique_function<void()> m_deleteFunc;
};

TEST_CASE_METHOD(SchedulerTest, "SchedulerTest ScheduleCommandPtr",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int destructionCounter = 0;
  int runCounter = 0;

  bool finish = false;

  {
    auto commandPtr =
        TrackDestroyCommand([&destructionCounter] { destructionCounter++; })
            .AlongWith(InstantCommand([&runCounter] { runCounter++; }).ToPtr())
            .Until([&finish] { return finish; });
    CHECK(destructionCounter == 0);

    scheduler.Schedule(std::move(commandPtr));
    CHECK(destructionCounter == 0);
  }
  CHECK(destructionCounter == 0);
  scheduler.Run();
  CHECK(runCounter == 1);
  CHECK(destructionCounter == 0);
  finish = true;
  scheduler.Run();
  CHECK(runCounter == 1);
  CHECK(destructionCounter == 1);
}

TEST_CASE_METHOD(SchedulerTest,
                 "SchedulerTest SubsystemPeriodicRunsInRegistrationOrder",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  constexpr int subsystemCount = 32;

  std::vector<int> periodicOrder;
  std::vector<std::unique_ptr<TestSubsystem>> subsystems;
  for (int i = 0; i < subsystemCount; ++i) {
    subsystems.emplace_back(std::make_unique<TestSubsystem>(
        [&periodicOrder, i] { periodicOrder.push_back(i); }));
    scheduler.RegisterSubsystem(subsystems.back().get());
  }

  scheduler.Run();

  std::vector<int> registrationOrder(subsystemCount);
  std::iota(registrationOrder.begin(), registrationOrder.end(), 0);
  CHECK(periodicOrder == registrationOrder);
}

TEST_CASE_METHOD(SchedulerTest, "SchedulerTest CommandsExecuteInScheduleOrder",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  constexpr int commandCount = 32;

  std::vector<int> executeOrder;
  std::vector<CommandPtr> commands;
  for (int i = 0; i < commandCount; ++i) {
    commands.emplace_back(
        wpi::cmd::Run([&executeOrder, i] { executeOrder.push_back(i); }));
    scheduler.Schedule(commands.back());
  }

  scheduler.Run();

  std::vector<int> scheduleOrder(commandCount);
  std::iota(scheduleOrder.begin(), scheduleOrder.end(), 0);
  CHECK(executeOrder == scheduleOrder);
}

TEST_CASE_METHOD(SchedulerTest,
                 "SchedulerTest DefaultCommandsScheduleInRegistrationOrder",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  constexpr int subsystemCount = 32;

  std::vector<int> initializeOrder;
  std::vector<std::unique_ptr<TestSubsystem>> subsystems;
  for (int i = 0; i < subsystemCount; ++i) {
    subsystems.emplace_back(std::make_unique<TestSubsystem>());
    scheduler.RegisterSubsystem(subsystems.back().get());
  }
  for (int i = subsystemCount - 1; i >= 0; --i) {
    scheduler.SetDefaultCommand(
        subsystems[i].get(),
        RunOnce([&initializeOrder, i] { initializeOrder.push_back(i); },
                {subsystems[i].get()}));
  }

  scheduler.Run();

  std::vector<int> registrationOrder(subsystemCount);
  std::iota(registrationOrder.begin(), registrationOrder.end(), 0);
  CHECK(initializeOrder == registrationOrder);
}
