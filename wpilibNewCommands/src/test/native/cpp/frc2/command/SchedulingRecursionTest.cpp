// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc2/command/CommandHelper.h"
#include "frc2/command/RunCommand.h"
#include "gtest/gtest.h"

using namespace frc2;

class SchedulingRecursionTest : public CommandTestBaseWithParam<bool> {};

class SelfCancellingCommand
    : public CommandHelper<CommandBase, SelfCancellingCommand> {
 public:
  SelfCancellingCommand(CommandScheduler* scheduler, Subsystem* requirement)
      : m_scheduler(scheduler) {
    AddRequirements(requirement);
  }

  void Initialize() override { m_scheduler->Cancel(this); }

 private:
  CommandScheduler* m_scheduler;
};

/**
 * Checks <a
 * href="https://github.com/wpilibsuite/allwpilib/issues/4259">wpilibsuite/allwpilib#4259</a>.
 */
TEST_P(SchedulingRecursionTest, CancelFromInitialize) {
  CommandScheduler scheduler = GetScheduler();
  bool hasOtherRun = false;
  TestSubsystem requirement;
  SelfCancellingCommand selfCancels{&scheduler, &requirement};
  RunCommand other =
      RunCommand([&hasOtherRun] { hasOtherRun = true; }, {&requirement});

  scheduler.Schedule(GetParam(), &selfCancels);
  scheduler.Run();
  // interruptibility of new arrival isn't checked
  scheduler.Schedule(&other);

  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
  EXPECT_TRUE(scheduler.IsScheduled(&other));
  scheduler.Run();
  EXPECT_TRUE(hasOtherRun);
}

TEST_P(SchedulingRecursionTest, DefaultCommand) {
  CommandScheduler scheduler = GetScheduler();
  bool hasOtherRun = false;
  TestSubsystem requirement;
  SelfCancellingCommand selfCancels{&scheduler, &requirement};
  RunCommand other =
      RunCommand([&hasOtherRun] { hasOtherRun = true; }, {&requirement});
  scheduler.SetDefaultCommand(&requirement, std::move(other));

  scheduler.Schedule(GetParam(), &selfCancels);
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&selfCancels));
  EXPECT_TRUE(scheduler.IsScheduled(&other));
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
  EXPECT_TRUE(scheduler.IsScheduled(&selfCancels));
}

INSTANTIATE_TEST_SUITE_P(SchedulingRecursionTests, SchedulingRecursionTest,
                         testing::Bool());
