// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc2/command/Commands.h>

#include <utility>

#include <gtest/gtest.h>

#include "CommandTestBase.h"
#include "frc2/command/Command.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/FunctionalCommand.h"
#include "frc2/command/RunCommand.h"

using namespace frc2;

class SchedulingRecursionTest
    : public CommandTestBaseWithParam<Command::InterruptionBehavior> {};

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
TEST_P(SchedulingRecursionTest, CancelFromInitialize) {
  CommandScheduler scheduler = GetScheduler();
  bool hasOtherRun = false;
  int counter = 0;
  TestSubsystem requirement;
  SelfCancellingCommand selfCancels{&scheduler, counter, &requirement,
                                    GetParam()};
  auto other = cmd::Run([&hasOtherRun] { hasOtherRun = true; }, {&requirement});

  scheduler.Schedule(&selfCancels);
  scheduler.Run();
  scheduler.Schedule(other);

  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
  EXPECT_TRUE(scheduler.IsScheduled(other));
  EXPECT_EQ(1, counter);
  scheduler.Run();
  EXPECT_TRUE(hasOtherRun);
}

TEST_F(SchedulingRecursionTest, CancelFromInitializeAction) {
  CommandScheduler scheduler = GetScheduler();
  bool hasOtherRun = false;
  int counter = 0;
  TestSubsystem requirement;
  FunctionalCommand selfCancels{[] {},
                                [] {},
                                [&counter](bool) { counter++; },
                                [] { return false; },
                                {&requirement}};
  auto other = cmd::Run([&hasOtherRun] { hasOtherRun = true; }, {&requirement});
  scheduler.OnCommandInitialize([&scheduler, &selfCancels](const Command&) {
    scheduler.Cancel(&selfCancels);
  });
  scheduler.Schedule(&selfCancels);
  scheduler.Run();
  scheduler.Schedule(other);

  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
  EXPECT_TRUE(scheduler.IsScheduled(other));
  EXPECT_EQ(1, counter);
  scheduler.Run();
  EXPECT_TRUE(hasOtherRun);
}

TEST_P(SchedulingRecursionTest,
       DefaultCommandGetsRescheduledAfterSelfCanceling) {
  CommandScheduler scheduler = GetScheduler();
  bool hasOtherRun = false;
  int counter = 0;
  TestSubsystem requirement;
  SelfCancellingCommand selfCancels{&scheduler, counter, &requirement,
                                    GetParam()};
  auto other = cmd::Run([&hasOtherRun] { hasOtherRun = true; }, {&requirement});
  scheduler.SetDefaultCommand(&requirement, std::move(other));

  scheduler.Schedule(&selfCancels);
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
  EXPECT_TRUE(scheduler.IsScheduled(scheduler.GetDefaultCommand(&requirement)));
  EXPECT_EQ(1, counter);
  scheduler.Run();
  EXPECT_TRUE(hasOtherRun);
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

TEST_F(SchedulingRecursionTest, CancelFromEnd) {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  CancelEndCommand selfCancels{&scheduler, counter};

  scheduler.Schedule(&selfCancels);

  EXPECT_NO_THROW({ scheduler.Cancel(&selfCancels); });
  EXPECT_EQ(1, counter);
  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
}

TEST_F(SchedulingRecursionTest, CancelFromInterruptAction) {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  FunctionalCommand selfCancels{[] {}, [] {}, [](bool) {},
                                [] { return false; }};
  scheduler.OnCommandInterrupt([&](const Command&) {
    counter++;
    scheduler.Cancel(&selfCancels);
  });
  scheduler.Schedule(&selfCancels);

  EXPECT_NO_THROW({ scheduler.Cancel(&selfCancels); });
  EXPECT_EQ(1, counter);
  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
}

class EndCommand : public CommandHelper<Command, EndCommand> {
 public:
  explicit EndCommand(std::function<void(bool)> end) : m_end(end) {}
  void End(bool interrupted) override { m_end(interrupted); }
  bool IsFinished() override { return true; }

 private:
  std::function<void(bool)> m_end;
};

TEST_F(SchedulingRecursionTest, CancelFromEndLoop) {
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

  EXPECT_NO_THROW({ scheduler.Cancel(&aCancelsB); });
  EXPECT_EQ(4, counter);
  EXPECT_FALSE(scheduler.IsScheduled(&aCancelsB));
  EXPECT_FALSE(scheduler.IsScheduled(&bCancelsC));
  EXPECT_FALSE(scheduler.IsScheduled(&cCancelsD));
  EXPECT_FALSE(scheduler.IsScheduled(&dCancelsAll));
}

TEST_F(SchedulingRecursionTest, CancelFromEndLoopWhileInRunLoop) {
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

  EXPECT_NO_THROW({ scheduler.Run(); });
  EXPECT_EQ(4, counter);
  EXPECT_FALSE(scheduler.IsScheduled(&aCancelsB));
  EXPECT_FALSE(scheduler.IsScheduled(&bCancelsC));
  EXPECT_FALSE(scheduler.IsScheduled(&cCancelsD));
  EXPECT_FALSE(scheduler.IsScheduled(&dCancelsAll));
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

TEST_F(SchedulingRecursionTest, MultiCancelFromEnd) {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  EndCommand bIncrementsCounter([&counter](bool) { counter++; });
  MultiCancelCommand aCancelsB{&scheduler, counter, &bIncrementsCounter};

  scheduler.Schedule(&aCancelsB);
  scheduler.Schedule(&bIncrementsCounter);

  EXPECT_NO_THROW({ scheduler.Cancel(&aCancelsB); });
  EXPECT_EQ(2, counter);
  EXPECT_FALSE(scheduler.IsScheduled(&aCancelsB));
  EXPECT_FALSE(scheduler.IsScheduled(&bIncrementsCounter));
}

TEST_P(SchedulingRecursionTest, ScheduleFromEndCancel) {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  TestSubsystem requirement;
  SelfCancellingCommand selfCancels{&scheduler, counter, &requirement,
                                    GetParam()};

  scheduler.Schedule(&selfCancels);
  EXPECT_NO_THROW({ scheduler.Cancel(&selfCancels); });
  EXPECT_EQ(1, counter);
  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
}

TEST_P(SchedulingRecursionTest, ScheduleFromEndInterrupt) {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  TestSubsystem requirement;
  SelfCancellingCommand selfCancels{&scheduler, counter, &requirement,
                                    GetParam()};
  auto other = cmd::Idle({&requirement});

  scheduler.Schedule(&selfCancels);
  EXPECT_NO_THROW({ scheduler.Schedule(other); });
  EXPECT_EQ(1, counter);
  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
  EXPECT_TRUE(scheduler.IsScheduled(other));
}

TEST_F(SchedulingRecursionTest, ScheduleFromEndInterruptAction) {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  TestSubsystem requirement;
  auto selfCancels = cmd::Idle({&requirement});
  auto other = cmd::Idle({&requirement});
  scheduler.OnCommandInterrupt([&](const Command&) {
    counter++;
    scheduler.Schedule(other);
  });
  scheduler.Schedule(selfCancels);
  EXPECT_NO_THROW({ scheduler.Schedule(other); });
  EXPECT_EQ(1, counter);
  EXPECT_FALSE(scheduler.IsScheduled(selfCancels));
  EXPECT_TRUE(scheduler.IsScheduled(other));
}

TEST_F(SchedulingRecursionTest, CancelDefaultCommandFromEnd) {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  TestSubsystem requirement;
  FunctionalCommand defaultCommand{[] {},
                                   [] {},
                                   [&counter](bool) { counter++; },
                                   [] { return false; },
                                   {&requirement}};
  auto other = cmd::Idle({&requirement});
  FunctionalCommand cancelDefaultCommand{[] {}, [] {},
                                         [&](bool) {
                                           counter++;
                                           scheduler.Schedule(other);
                                         },
                                         [] { return false; }};

  EXPECT_NO_THROW({
    scheduler.Schedule(&cancelDefaultCommand);
    scheduler.SetDefaultCommand(&requirement, std::move(defaultCommand));

    scheduler.Run();
    scheduler.Cancel(&cancelDefaultCommand);
  });
  EXPECT_EQ(2, counter);
  EXPECT_FALSE(scheduler.IsScheduled(&defaultCommand));
  EXPECT_TRUE(scheduler.IsScheduled(other));
}

TEST_F(SchedulingRecursionTest, CancelNextCommandFromCommand) {
  CommandScheduler scheduler = GetScheduler();

  frc2::RunCommand* command1Ptr = nullptr;
  frc2::RunCommand* command2Ptr = nullptr;
  int counter = 0;

  auto command1 = frc2::RunCommand([&counter, &command2Ptr, &scheduler] {
    scheduler.Cancel(command2Ptr);
    counter++;
  });
  auto command2 = frc2::RunCommand([&counter, &command1Ptr, &scheduler] {
    scheduler.Cancel(command1Ptr);
    counter++;
  });

  command1Ptr = &command1;
  command2Ptr = &command2;

  scheduler.Schedule(&command1);
  scheduler.Schedule(&command2);
  scheduler.Run();

  EXPECT_EQ(counter, 1) << "Second command was run when it shouldn't have been";

  // only one of the commands should be canceled.
  EXPECT_FALSE(scheduler.IsScheduled(&command1) &&
               scheduler.IsScheduled(&command2))
      << "Both commands are running when only one should be";
  // one of the commands shouldn't be canceled because the other one is canceled
  // first
  EXPECT_TRUE(scheduler.IsScheduled(&command1) ||
              scheduler.IsScheduled(&command2))
      << "Both commands are canceled when only one should be";

  scheduler.Run();
  EXPECT_EQ(counter, 2);
}

INSTANTIATE_TEST_SUITE_P(
    SchedulingRecursionTests, SchedulingRecursionTest,
    testing::Values(Command::InterruptionBehavior::kCancelSelf,
                    Command::InterruptionBehavior::kCancelIncoming));
