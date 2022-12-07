// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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

  ParallelCommandGroup group(tcb::make_vector<std::unique_ptr<Command>>(
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

  ParallelCommandGroup group(tcb::make_vector<std::unique_ptr<Command>>(
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

  ParallelCommandGroup group((InstantCommand(), InstantCommand()));

  EXPECT_NO_FATAL_FAILURE(scheduler.Cancel(&group));
}

TEST_F(ParallelCommandGroupTest, ParallelGroupCopy) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  WaitUntilCommand command([&finished] { return finished; });

  ParallelCommandGroup group(command);
  scheduler.Schedule(&group);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&group));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

TEST_F(ParallelCommandGroupTest, ParallelGroupRequirement) {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  InstantCommand command1([] {}, {&requirement1, &requirement2});
  InstantCommand command2([] {}, {&requirement3});
  InstantCommand command3([] {}, {&requirement3, &requirement4});

  ParallelCommandGroup group(std::move(command1), std::move(command2));

  scheduler.Schedule(&group);
  scheduler.Schedule(&command3);

  EXPECT_TRUE(scheduler.IsScheduled(&command3));
  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

INSTANTIATE_MULTI_COMMAND_COMPOSITION_TEST_SUITE(ParallelCommandGroupTest,
                                                 ParallelCommandGroup);
