// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc2/command/Command.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/RunCommand.h"
#include "gtest/gtest.h"

using namespace frc2;

class SchedulingRecursionTest
    : public CommandTestBaseWithParam<Command::InterruptionBehavior> {};

class SelfCancellingCommand
    : public CommandHelper<CommandBase, SelfCancellingCommand> {
 public:
  SelfCancellingCommand(CommandScheduler* scheduler, Subsystem* requirement,
                        Command::InterruptionBehavior interruptionBehavior =
                            Command::InterruptionBehavior::kCancelSelf)
      : m_scheduler(scheduler), m_interrupt(interruptionBehavior) {
    AddRequirements(requirement);
  }

  void Initialize() override { m_scheduler->Cancel(this); }

  InterruptionBehavior GetInterruptionBehavior() const override {
    return m_interrupt;
  }

 private:
  CommandScheduler* m_scheduler;
  InterruptionBehavior m_interrupt;
};

/**
 * Checks <a
 * href="https://github.com/wpilibsuite/allwpilib/issues/4259">wpilibsuite/allwpilib#4259</a>.
 */
TEST_F(SchedulingRecursionTest, CancelFromInitialize) {
  CommandScheduler scheduler = GetScheduler();
  bool hasOtherRun = false;
  TestSubsystem requirement;
  auto selfCancels = SelfCancellingCommand(&scheduler, &requirement);
  RunCommand other =
      RunCommand([&hasOtherRun] { hasOtherRun = true; }, {&requirement});

  scheduler.Schedule(&selfCancels);
  scheduler.Run();
  scheduler.Schedule(&other);

  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
  EXPECT_TRUE(scheduler.IsScheduled(&other));
  scheduler.Run();
  EXPECT_TRUE(hasOtherRun);
}

TEST_P(SchedulingRecursionTest,
       DefaultCommandGetsRescheduledAfterSelfCanceling) {
  CommandScheduler scheduler = GetScheduler();
  bool hasOtherRun = false;
  TestSubsystem requirement;
  auto selfCancels =
      SelfCancellingCommand(&scheduler, &requirement, GetParam());
  RunCommand other =
      RunCommand([&hasOtherRun] { hasOtherRun = true; }, {&requirement});
  scheduler.SetDefaultCommand(&requirement, std::move(other));

  scheduler.Schedule(&selfCancels);
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
  EXPECT_TRUE(scheduler.IsScheduled(scheduler.GetDefaultCommand(&requirement)));
  scheduler.Run();
  EXPECT_TRUE(hasOtherRun);
}

class CancelEndCommand : public CommandHelper<CommandBase, CancelEndCommand> {
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

INSTANTIATE_TEST_SUITE_P(
    SchedulingRecursionTests, SchedulingRecursionTest,
    testing::Values(Command::InterruptionBehavior::kCancelSelf,
                    Command::InterruptionBehavior::kCancelIncoming));
