// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "CommandTestBase.h"
#include "frc/command2/Command.h"
#include "frc/command2/CommandHelper.h"
#include "frc/command2/RunCommand.h"

class SchedulingRecursionTest
    : public CommandTestBaseWithParam<frc::Command::InterruptionBehavior> {};

class SelfCancellingCommand
    : public frc::CommandHelper<frc::Command, SelfCancellingCommand> {
 public:
  SelfCancellingCommand(
      frc::CommandScheduler* scheduler, frc::Subsystem* requirement,
      frc::Command::InterruptionBehavior interruptionBehavior =
          frc::Command::InterruptionBehavior::kCancelSelf)
      : m_scheduler(scheduler), m_interrupt(interruptionBehavior) {
    AddRequirements(requirement);
  }

  void Initialize() override { m_scheduler->Cancel(this); }

  InterruptionBehavior GetInterruptionBehavior() const override {
    return m_interrupt;
  }

 private:
  frc::CommandScheduler* m_scheduler;
  InterruptionBehavior m_interrupt;
};

/**
 * Checks <a
 * href="https://github.com/wpilibsuite/allwpilib/issues/4259">wpilibsuite/allwpilib#4259</a>.
 */
TEST_F(SchedulingRecursionTest, CancelFromInitialize) {
  frc::CommandScheduler scheduler = GetScheduler();
  bool hasOtherRun = false;
  TestSubsystem requirement;
  auto selfCancels = SelfCancellingCommand(&scheduler, &requirement);
  frc::RunCommand other =
      frc::RunCommand([&hasOtherRun] { hasOtherRun = true; }, {&requirement});

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
  frc::CommandScheduler scheduler = GetScheduler();
  bool hasOtherRun = false;
  TestSubsystem requirement;
  auto selfCancels =
      SelfCancellingCommand(&scheduler, &requirement, GetParam());
  frc::RunCommand other =
      frc::RunCommand([&hasOtherRun] { hasOtherRun = true; }, {&requirement});
  scheduler.SetDefaultCommand(&requirement, std::move(other));

  scheduler.Schedule(&selfCancels);
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
  EXPECT_TRUE(scheduler.IsScheduled(scheduler.GetDefaultCommand(&requirement)));
  scheduler.Run();
  EXPECT_TRUE(hasOtherRun);
}

class CancelEndCommand
    : public frc::CommandHelper<frc::Command, CancelEndCommand> {
 public:
  CancelEndCommand(frc::CommandScheduler* scheduler, int& counter)
      : m_scheduler(scheduler), m_counter(counter) {}

  void End(bool interrupted) override {
    m_counter++;
    m_scheduler->Cancel(this);
  }

 private:
  frc::CommandScheduler* m_scheduler;
  int& m_counter;
};

TEST_F(SchedulingRecursionTest, CancelFromEnd) {
  frc::CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  CancelEndCommand selfCancels{&scheduler, counter};

  scheduler.Schedule(&selfCancels);

  EXPECT_NO_THROW({ scheduler.Cancel(&selfCancels); });
  EXPECT_EQ(1, counter);
  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
}

INSTANTIATE_TEST_SUITE_P(
    SchedulingRecursionTests, SchedulingRecursionTest,
    testing::Values(frc::Command::InterruptionBehavior::kCancelSelf,
                    frc::Command::InterruptionBehavior::kCancelIncoming));
