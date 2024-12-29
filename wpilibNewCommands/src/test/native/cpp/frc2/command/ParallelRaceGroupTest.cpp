// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <utility>

#include "CommandTestBase.h"
#include "CompositionTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/SequentialCommandGroup.h"
#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;
class ParallelRaceGroupTest : public CommandTestBase {};

TEST_F(ParallelRaceGroupTest, ParallelRaceSchedule) {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command3Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();
  MockCommand* command3 = command3Holder.get();

  ParallelRaceGroup group{make_vector<std::unique_ptr<Command>>(
      std::move(command1Holder), std::move(command2Holder),
      std::move(command3Holder))};

  EXPECT_CALL(*command1, Initialize());
  EXPECT_CALL(*command1, Execute()).Times(2);
  EXPECT_CALL(*command1, End(true));

  EXPECT_CALL(*command2, Initialize());
  EXPECT_CALL(*command2, Execute()).Times(2);
  EXPECT_CALL(*command2, End(false));

  EXPECT_CALL(*command3, Initialize());
  EXPECT_CALL(*command3, Execute()).Times(2);
  EXPECT_CALL(*command3, End(true));

  scheduler.Schedule(&group);

  scheduler.Run();
  command2->SetFinished(true);
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

TEST_F(ParallelRaceGroupTest, ParallelRaceInterrupt) {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command3Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();
  MockCommand* command3 = command3Holder.get();

  ParallelRaceGroup group{make_vector<std::unique_ptr<Command>>(
      std::move(command1Holder), std::move(command2Holder),
      std::move(command3Holder))};

  EXPECT_CALL(*command1, Initialize());
  EXPECT_CALL(*command1, Execute()).Times(1);
  EXPECT_CALL(*command1, End(true));

  EXPECT_CALL(*command2, Initialize());
  EXPECT_CALL(*command2, Execute()).Times(1);
  EXPECT_CALL(*command2, End(true));

  EXPECT_CALL(*command3, Initialize());
  EXPECT_CALL(*command3, Execute()).Times(1);
  EXPECT_CALL(*command3, End(true));

  scheduler.Schedule(&group);

  scheduler.Run();
  scheduler.Cancel(&group);
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

TEST_F(ParallelRaceGroupTest, ParallelRaceNotScheduledCancel) {
  CommandScheduler scheduler = GetScheduler();

  auto group = cmd::Race(cmd::None(), cmd::None());

  EXPECT_NO_FATAL_FAILURE(scheduler.Cancel(group));
}

TEST_F(ParallelRaceGroupTest, ParallelRaceCopy) {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = cmd::WaitUntil([&finished] { return finished; });

  auto group = cmd::Race(std::move(command));
  scheduler.Schedule(group);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(group));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(group));
}

TEST_F(ParallelRaceGroupTest, RaceGroupRequirement) {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  auto command1 = cmd::RunOnce([] {}, {&requirement1, &requirement2});
  auto command2 = cmd::RunOnce([] {}, {&requirement3});
  auto command3 = cmd::RunOnce([] {}, {&requirement3, &requirement4});

  auto group = cmd::Race(std::move(command1), std::move(command2));

  scheduler.Schedule(group);
  scheduler.Schedule(command3);

  EXPECT_TRUE(scheduler.IsScheduled(command3));
  EXPECT_FALSE(scheduler.IsScheduled(group));
}

TEST_F(ParallelRaceGroupTest, ParallelRaceOnlyCallsEndOnce) {
  CommandScheduler scheduler = GetScheduler();

  bool finished1 = false;
  bool finished2 = false;
  bool finished3 = false;

  auto command1 = cmd::WaitUntil([&finished1] { return finished1; });
  auto command2 = cmd::WaitUntil([&finished2] { return finished2; });
  auto command3 = cmd::WaitUntil([&finished3] { return finished3; });

  auto group1 = cmd::Sequence(std::move(command1), std::move(command2));
  auto group2 = cmd::Race(std::move(group1), std::move(command3));

  scheduler.Schedule(group2);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(group2));
  finished1 = true;
  scheduler.Run();
  finished2 = true;
  EXPECT_NO_FATAL_FAILURE(scheduler.Run());
  EXPECT_FALSE(scheduler.IsScheduled(group2));
}

TEST_F(ParallelRaceGroupTest, ParallelRaceScheduleTwice) {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command3Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();
  MockCommand* command3 = command3Holder.get();

  ParallelRaceGroup group{make_vector<std::unique_ptr<Command>>(
      std::move(command1Holder), std::move(command2Holder),
      std::move(command3Holder))};

  EXPECT_CALL(*command1, Initialize()).Times(2);
  EXPECT_CALL(*command1, Execute()).Times(5);
  EXPECT_CALL(*command1, End(true)).Times(2);

  EXPECT_CALL(*command2, Initialize()).Times(2);
  EXPECT_CALL(*command2, Execute()).Times(5);
  EXPECT_CALL(*command2, End(false)).Times(2);

  EXPECT_CALL(*command3, Initialize()).Times(2);
  EXPECT_CALL(*command3, Execute()).Times(5);
  EXPECT_CALL(*command3, End(true)).Times(2);

  scheduler.Schedule(&group);

  scheduler.Run();
  command2->SetFinished(true);
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&group));

  command2->SetFinished(false);

  scheduler.Schedule(&group);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&group));

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&group));

  command2->SetFinished(true);
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

INSTANTIATE_MULTI_COMMAND_COMPOSITION_TEST_SUITE(ParallelRaceGroupTest,
                                                 ParallelRaceGroup);
