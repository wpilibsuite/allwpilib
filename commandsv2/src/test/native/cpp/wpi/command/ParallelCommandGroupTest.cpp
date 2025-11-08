// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <utility>

#include "CommandTestBase.h"
#include "CompositionTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelCommandGroup.h"
#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;
class ParallelCommandGroupTest : public CommandTestBase {};

TEST_F(ParallelCommandGroupTest, ParallelGroupSchedule) {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();

  ParallelCommandGroup group(make_vector<std::unique_ptr<Command>>(
      std::move(command1Holder), std::move(command2Holder)));

  EXPECT_CALL(*command1, Initialize());
  EXPECT_CALL(*command1, Execute()).Times(1);
  EXPECT_CALL(*command1, End(false));

  EXPECT_CALL(*command2, Initialize());
  EXPECT_CALL(*command2, Execute()).Times(2);
  EXPECT_CALL(*command2, End(false));

  scheduler.Schedule(&group);

  command1->SetFinished(true);
  scheduler.Run();
  command2->SetFinished(true);
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

TEST_F(ParallelCommandGroupTest, ParallelGroupInterrupt) {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();

  ParallelCommandGroup group(make_vector<std::unique_ptr<Command>>(
      std::move(command1Holder), std::move(command2Holder)));

  EXPECT_CALL(*command1, Initialize());
  EXPECT_CALL(*command1, Execute()).Times(1);
  EXPECT_CALL(*command1, End(false));

  EXPECT_CALL(*command2, Initialize());
  EXPECT_CALL(*command2, Execute()).Times(2);
  EXPECT_CALL(*command2, End(false)).Times(0);
  EXPECT_CALL(*command2, End(true));

  scheduler.Schedule(&group);

  command1->SetFinished(true);
  scheduler.Run();
  scheduler.Run();
  scheduler.Cancel(&group);

  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

TEST_F(ParallelCommandGroupTest, ParallelGroupNotScheduledCancel) {
  CommandScheduler scheduler = GetScheduler();

  auto group = cmd::Parallel(cmd::None(), cmd::None());

  EXPECT_NO_FATAL_FAILURE(scheduler.Cancel(group));
}

TEST_F(ParallelCommandGroupTest, ParallelGroupCopy) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = cmd::WaitUntil([&finished] { return finished; });

  auto group = cmd::Parallel(std::move(command));
  scheduler.Schedule(group);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(group));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(group));
}

TEST_F(ParallelCommandGroupTest, ParallelGroupRequirement) {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  auto command1 = cmd::RunOnce([] {}, {&requirement1, &requirement2});
  auto command2 = cmd::RunOnce([] {}, {&requirement3});
  auto command3 = cmd::RunOnce([] {}, {&requirement3, &requirement4});

  auto group = cmd::Parallel(std::move(command1), std::move(command2));

  scheduler.Schedule(group);
  scheduler.Schedule(command3);

  EXPECT_TRUE(scheduler.IsScheduled(command3));
  EXPECT_FALSE(scheduler.IsScheduled(group));
}

INSTANTIATE_MULTI_COMMAND_COMPOSITION_TEST_SUITE(ParallelCommandGroupTest,
                                                 ParallelCommandGroup);
