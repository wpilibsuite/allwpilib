// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "CommandTestBase.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandHelper.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"
#include "wpi/commands2/RunCommand.hpp"

using namespace wpi::cmd;

class SchedulingRecursionTest : public CommandTestBase {};

class SelfCancellingCommand
    : public CommandHelper<Command, SelfCancellingCommand> {
 public:
  SelfCancellingCommand(CommandScheduler* scheduler, int& counter,
                        Subsystem* requirement,
                        Command::InterruptionBehavior interruptionBehavior =
                            Command::InterruptionBehavior::kCancelSelf)
      : m_scheduler(scheduler),
        m_counter(counter),
        m_interrupt(interruptionBehavior) {
    AddRequirements(requirement);
  }

  void Initialize() override { m_scheduler->Cancel(this); }

  void End(bool interrupted) override { m_counter++; }

  InterruptionBehavior GetInterruptionBehavior() const override {
    return m_interrupt;
  }

 private:
  CommandScheduler* m_scheduler;
  int& m_counter;
  InterruptionBehavior m_interrupt;
};

/**
 * Checks <a
 * href="https://github.com/wpilibsuite/allwpilib/issues/4259">wpilibsuite/allwpilib#4259</a>.
 */
void CheckCancelFromInitialize(
    Command::InterruptionBehavior interruptionBehavior) {
  SchedulingRecursionTest testBase;
  CommandScheduler scheduler = testBase.GetScheduler();
  bool hasOtherRun = false;
  int counter = 0;
  TestSubsystem requirement;
  SelfCancellingCommand selfCancels{&scheduler, counter, &requirement,
                                    interruptionBehavior};
  auto other =
      wpi::cmd::Run([&hasOtherRun] { hasOtherRun = true; }, {&requirement});

  scheduler.Schedule(&selfCancels);
  scheduler.Run();
  scheduler.Schedule(other);

  CHECK_FALSE(scheduler.IsScheduled(&selfCancels));
  CHECK(scheduler.IsScheduled(other));
  CHECK(1 == counter);
  scheduler.Run();
  CHECK(hasOtherRun);
}

TEST_CASE_METHOD(SchedulingRecursionTest,
                 "SchedulingRecursionTest CancelFromInitializeAction",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  bool hasOtherRun = false;
  int counter = 0;
  TestSubsystem requirement;
  FunctionalCommand selfCancels{[] {},
                                [] {},
                                [&counter](bool) { counter++; },
                                [] { return false; },
                                {&requirement}};
  auto other =
      wpi::cmd::Run([&hasOtherRun] { hasOtherRun = true; }, {&requirement});
  scheduler.OnCommandInitialize([&scheduler, &selfCancels](const Command&) {
    scheduler.Cancel(&selfCancels);
  });
  scheduler.Schedule(&selfCancels);
  scheduler.Run();
  scheduler.Schedule(other);

  CHECK_FALSE(scheduler.IsScheduled(&selfCancels));
  CHECK(scheduler.IsScheduled(other));
  CHECK(1 == counter);
  scheduler.Run();
  CHECK(hasOtherRun);
}

void CheckDefaultCommandGetsRescheduledAfterSelfCanceling(
    Command::InterruptionBehavior interruptionBehavior) {
  SchedulingRecursionTest testBase;
  CommandScheduler scheduler = testBase.GetScheduler();
  bool hasOtherRun = false;
  int counter = 0;
  TestSubsystem requirement;
  SelfCancellingCommand selfCancels{&scheduler, counter, &requirement,
                                    interruptionBehavior};
  auto other =
      wpi::cmd::Run([&hasOtherRun] { hasOtherRun = true; }, {&requirement});
  scheduler.SetDefaultCommand(&requirement, std::move(other));

  scheduler.Schedule(&selfCancels);
  scheduler.Run();
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&selfCancels));
  CHECK(scheduler.IsScheduled(scheduler.GetDefaultCommand(&requirement)));
  CHECK(1 == counter);
  scheduler.Run();
  CHECK(hasOtherRun);
}

class CancelEndCommand : public CommandHelper<Command, CancelEndCommand> {
 public:
  CancelEndCommand(CommandScheduler* scheduler, int& counter)
      : m_scheduler(scheduler), m_counter(counter) {}

  void End(bool interrupted) override {
    m_counter++;
    m_scheduler->Cancel(this);
  }

 private:
  CommandScheduler* m_scheduler;
  int& m_counter;
};

TEST_CASE_METHOD(SchedulingRecursionTest,
                 "SchedulingRecursionTest CancelFromEnd",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  CancelEndCommand selfCancels{&scheduler, counter};

  scheduler.Schedule(&selfCancels);

  CHECK_NOTHROW(scheduler.Cancel(&selfCancels));
  CHECK(1 == counter);
  CHECK_FALSE(scheduler.IsScheduled(&selfCancels));
}

TEST_CASE_METHOD(SchedulingRecursionTest,
                 "SchedulingRecursionTest CancelFromInterruptAction",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  FunctionalCommand selfCancels{[] {}, [] {}, [](bool) {},
                                [] { return false; }};
  scheduler.OnCommandInterrupt([&](const Command&) {
    counter++;
    scheduler.Cancel(&selfCancels);
  });
  scheduler.Schedule(&selfCancels);

  CHECK_NOTHROW(scheduler.Cancel(&selfCancels));
  CHECK(1 == counter);
  CHECK_FALSE(scheduler.IsScheduled(&selfCancels));
}

class EndCommand : public CommandHelper<Command, EndCommand> {
 public:
  explicit EndCommand(std::function<void(bool)> end) : m_end(end) {}
  void End(bool interrupted) override { m_end(interrupted); }
  bool IsFinished() override { return true; }

 private:
  std::function<void(bool)> m_end;
};

TEST_CASE_METHOD(SchedulingRecursionTest,
                 "SchedulingRecursionTest CancelFromEndLoop",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  EndCommand dCancelsAll([&](bool) {
    counter++;
    scheduler.CancelAll();
  });
  EndCommand cCancelsD([&](bool) {
    counter++;
    scheduler.Cancel(&dCancelsAll);
  });
  EndCommand bCancelsC([&](bool) {
    counter++;
    scheduler.Cancel(&cCancelsD);
  });
  EndCommand aCancelsB([&](bool) {
    counter++;
    scheduler.Cancel(&bCancelsC);
  });
  scheduler.Schedule(&aCancelsB);
  scheduler.Schedule(&bCancelsC);
  scheduler.Schedule(&cCancelsD);
  scheduler.Schedule(&dCancelsAll);

  CHECK_NOTHROW(scheduler.Cancel(&aCancelsB));
  CHECK(4 == counter);
  CHECK_FALSE(scheduler.IsScheduled(&aCancelsB));
  CHECK_FALSE(scheduler.IsScheduled(&bCancelsC));
  CHECK_FALSE(scheduler.IsScheduled(&cCancelsD));
  CHECK_FALSE(scheduler.IsScheduled(&dCancelsAll));
}

TEST_CASE_METHOD(SchedulingRecursionTest,
                 "SchedulingRecursionTest CancelFromEndLoopWhileInRunLoop",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;

  EndCommand dCancelsAll([&](bool) {
    counter++;
    scheduler.CancelAll();
  });
  EndCommand cCancelsD([&](bool) {
    counter++;
    scheduler.Cancel(&dCancelsAll);
  });
  EndCommand bCancelsC([&](bool) {
    counter++;
    scheduler.Cancel(&cCancelsD);
  });
  EndCommand aCancelsB([&](bool) {
    counter++;
    scheduler.Cancel(&bCancelsC);
  });
  scheduler.Schedule(&aCancelsB);
  scheduler.Schedule(&bCancelsC);
  scheduler.Schedule(&cCancelsD);
  scheduler.Schedule(&dCancelsAll);

  CHECK_NOTHROW(scheduler.Run());
  CHECK(4 == counter);
  CHECK_FALSE(scheduler.IsScheduled(&aCancelsB));
  CHECK_FALSE(scheduler.IsScheduled(&bCancelsC));
  CHECK_FALSE(scheduler.IsScheduled(&cCancelsD));
  CHECK_FALSE(scheduler.IsScheduled(&dCancelsAll));
}

class MultiCancelCommand : public CommandHelper<Command, MultiCancelCommand> {
 public:
  MultiCancelCommand(CommandScheduler* scheduler, int& counter,
                     Command* command)
      : m_scheduler(scheduler), m_counter(counter), m_command(command) {}

  void End(bool interrupted) override {
    m_counter++;
    m_scheduler->Cancel(m_command);
    m_scheduler->Cancel(this);
  }

 private:
  CommandScheduler* m_scheduler;
  int& m_counter;
  Command* m_command;
};

TEST_CASE_METHOD(SchedulingRecursionTest,
                 "SchedulingRecursionTest MultiCancelFromEnd",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  EndCommand bIncrementsCounter([&counter](bool) { counter++; });
  MultiCancelCommand aCancelsB{&scheduler, counter, &bIncrementsCounter};

  scheduler.Schedule(&aCancelsB);
  scheduler.Schedule(&bIncrementsCounter);

  CHECK_NOTHROW(scheduler.Cancel(&aCancelsB));
  CHECK(2 == counter);
  CHECK_FALSE(scheduler.IsScheduled(&aCancelsB));
  CHECK_FALSE(scheduler.IsScheduled(&bIncrementsCounter));
}

void CheckScheduleFromEndCancel(
    Command::InterruptionBehavior interruptionBehavior) {
  SchedulingRecursionTest testBase;
  CommandScheduler scheduler = testBase.GetScheduler();
  int counter = 0;
  TestSubsystem requirement;
  SelfCancellingCommand selfCancels{&scheduler, counter, &requirement,
                                    interruptionBehavior};

  scheduler.Schedule(&selfCancels);
  CHECK_NOTHROW(scheduler.Cancel(&selfCancels));
  CHECK(1 == counter);
  CHECK_FALSE(scheduler.IsScheduled(&selfCancels));
}

void CheckScheduleFromEndInterrupt(
    Command::InterruptionBehavior interruptionBehavior) {
  SchedulingRecursionTest testBase;
  CommandScheduler scheduler = testBase.GetScheduler();
  int counter = 0;
  TestSubsystem requirement;
  SelfCancellingCommand selfCancels{&scheduler, counter, &requirement,
                                    interruptionBehavior};
  auto other = Idle({&requirement});

  scheduler.Schedule(&selfCancels);
  CHECK_NOTHROW(scheduler.Schedule(other));
  CHECK(1 == counter);
  CHECK_FALSE(scheduler.IsScheduled(&selfCancels));
  CHECK(scheduler.IsScheduled(other));
}

TEST_CASE_METHOD(SchedulingRecursionTest,
                 "SchedulingRecursionTest ScheduleFromEndInterruptAction",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  TestSubsystem requirement;
  auto selfCancels = Idle({&requirement});
  auto other = Idle({&requirement});
  scheduler.OnCommandInterrupt([&](const Command&) {
    counter++;
    scheduler.Schedule(other);
  });
  scheduler.Schedule(selfCancels);
  CHECK_NOTHROW(scheduler.Schedule(other));
  CHECK(1 == counter);
  CHECK_FALSE(scheduler.IsScheduled(selfCancels));
  CHECK(scheduler.IsScheduled(other));
}

TEST_CASE_METHOD(SchedulingRecursionTest,
                 "SchedulingRecursionTest CancelDefaultCommandFromEnd",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  TestSubsystem requirement;
  FunctionalCommand defaultCommand{[] {},
                                   [] {},
                                   [&counter](bool) { counter++; },
                                   [] { return false; },
                                   {&requirement}};
  auto other = Idle({&requirement});
  FunctionalCommand cancelDefaultCommand{[] {}, [] {},
                                         [&](bool) {
                                           counter++;
                                           scheduler.Schedule(other);
                                         },
                                         [] { return false; }};

  CHECK_NOTHROW([&] {
    scheduler.Schedule(&cancelDefaultCommand);
    scheduler.SetDefaultCommand(&requirement, std::move(defaultCommand));

    scheduler.Run();
    scheduler.Cancel(&cancelDefaultCommand);
  }());
  CHECK(2 == counter);
  CHECK_FALSE(scheduler.IsScheduled(&defaultCommand));
  CHECK(scheduler.IsScheduled(other));
}

TEST_CASE_METHOD(SchedulingRecursionTest,
                 "SchedulingRecursionTest CancelNextCommandFromCommand",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  RunCommand* command1Ptr = nullptr;
  RunCommand* command2Ptr = nullptr;
  int counter = 0;

  auto command1 = RunCommand([&counter, &command2Ptr, &scheduler] {
    scheduler.Cancel(command2Ptr);
    counter++;
  });
  auto command2 = RunCommand([&counter, &command1Ptr, &scheduler] {
    scheduler.Cancel(command1Ptr);
    counter++;
  });

  command1Ptr = &command1;
  command2Ptr = &command2;

  scheduler.Schedule(&command1);
  scheduler.Schedule(&command2);
  scheduler.Run();

  CHECK(counter == 1);

  // only one of the commands should be canceled.
  CHECK_FALSE(
      (scheduler.IsScheduled(&command1) && scheduler.IsScheduled(&command2)));
  // one of the commands shouldn't be canceled because the other one is canceled
  // first
  CHECK((scheduler.IsScheduled(&command1) || scheduler.IsScheduled(&command2)));

  scheduler.Run();
  CHECK(counter == 2);
}

namespace {

#define SCHEDULING_RECURSION_TEST_CASE(Name, Behavior)      \
  TEST_CASE("SchedulingRecursionTest " #Name " " #Behavior, \
            "[commandsv2][command]") {                      \
    Check##Name(Command::InterruptionBehavior::Behavior);   \
  }

SCHEDULING_RECURSION_TEST_CASE(CancelFromInitialize, kCancelSelf)
SCHEDULING_RECURSION_TEST_CASE(CancelFromInitialize, kCancelIncoming)
SCHEDULING_RECURSION_TEST_CASE(DefaultCommandGetsRescheduledAfterSelfCanceling,
                               kCancelSelf)
SCHEDULING_RECURSION_TEST_CASE(DefaultCommandGetsRescheduledAfterSelfCanceling,
                               kCancelIncoming)
SCHEDULING_RECURSION_TEST_CASE(ScheduleFromEndCancel, kCancelSelf)
SCHEDULING_RECURSION_TEST_CASE(ScheduleFromEndCancel, kCancelIncoming)
SCHEDULING_RECURSION_TEST_CASE(ScheduleFromEndInterrupt, kCancelSelf)
SCHEDULING_RECURSION_TEST_CASE(ScheduleFromEndInterrupt, kCancelIncoming)

#undef SCHEDULING_RECURSION_TEST_CASE

}  // namespace
