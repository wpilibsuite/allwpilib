// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/ParallelRaceGroup.hpp"

#include <memory>
#include <utility>

#include "CommandTestBase.hpp"
#include "CompositionTestBase.hpp"
#include "wpi/commands2/Commands.hpp"

using namespace wpi::cmd;
class ParallelRaceGroupTest : public CommandTestBase {};

TEST_CASE_METHOD(ParallelRaceGroupTest,
                 "ParallelRaceGroupTest ParallelRaceSchedule",
                 "[commandsv2][command]") {
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

  command1->ExpectInitialize(1);
  command1->ExpectExecute(2);
  command1->ExpectEnd(true, 1);

  command2->ExpectInitialize(1);
  command2->ExpectExecute(2);
  command2->ExpectEnd(false, 1);

  command3->ExpectInitialize(1);
  command3->ExpectExecute(2);
  command3->ExpectEnd(true, 1);

  scheduler.Schedule(&group);

  scheduler.Run();
  command2->SetFinished(true);
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(&group));
}

TEST_CASE_METHOD(ParallelRaceGroupTest,
                 "ParallelRaceGroupTest ParallelRaceInterrupt",
                 "[commandsv2][command]") {
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

  command1->ExpectInitialize(1);
  command1->ExpectExecute(1);
  command1->ExpectEnd(true, 1);

  command2->ExpectInitialize(1);
  command2->ExpectExecute(1);
  command2->ExpectEnd(true, 1);

  command3->ExpectInitialize(1);
  command3->ExpectExecute(1);
  command3->ExpectEnd(true, 1);

  scheduler.Schedule(&group);

  scheduler.Run();
  scheduler.Cancel(&group);
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(&group));
}

TEST_CASE_METHOD(ParallelRaceGroupTest,
                 "ParallelRaceGroupTest ParallelRaceNotScheduledCancel",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  auto group = Race(None(), None());

  CHECK_NOTHROW(scheduler.Cancel(group));
}

TEST_CASE_METHOD(ParallelRaceGroupTest,
                 "ParallelRaceGroupTest ParallelRaceCopy",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = WaitUntil([&finished] { return finished; });

  auto group = Race(std::move(command));
  scheduler.Schedule(group);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(group));
  finished = true;
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(group));
}

TEST_CASE_METHOD(ParallelRaceGroupTest,
                 "ParallelRaceGroupTest RaceGroupRequirement",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  auto command1 = RunOnce([] {}, {&requirement1, &requirement2});
  auto command2 = RunOnce([] {}, {&requirement3});
  auto command3 = RunOnce([] {}, {&requirement3, &requirement4});

  auto group = Race(std::move(command1), std::move(command2));

  scheduler.Schedule(group);
  scheduler.Schedule(command3);

  CHECK(scheduler.IsScheduled(command3));
  CHECK_FALSE(scheduler.IsScheduled(group));
}

TEST_CASE_METHOD(ParallelRaceGroupTest,
                 "ParallelRaceGroupTest ParallelRaceOnlyCallsEndOnce",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finished1 = false;
  bool finished2 = false;
  bool finished3 = false;

  auto command1 = WaitUntil([&finished1] { return finished1; });
  auto command2 = WaitUntil([&finished2] { return finished2; });
  auto command3 = WaitUntil([&finished3] { return finished3; });

  auto group1 = Sequence(std::move(command1), std::move(command2));
  auto group2 = Race(std::move(group1), std::move(command3));

  scheduler.Schedule(group2);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(group2));
  finished1 = true;
  scheduler.Run();
  finished2 = true;
  CHECK_NOTHROW(scheduler.Run());
  CHECK_FALSE(scheduler.IsScheduled(group2));
}

TEST_CASE_METHOD(ParallelRaceGroupTest,
                 "ParallelRaceGroupTest ParallelRaceScheduleTwice",
                 "[commandsv2][command]") {
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

  command1->ExpectInitialize(2);
  command1->ExpectExecute(5);
  command1->ExpectEnd(true, 2);

  command2->ExpectInitialize(2);
  command2->ExpectExecute(5);
  command2->ExpectEnd(false, 2);

  command3->ExpectInitialize(2);
  command3->ExpectExecute(5);
  command3->ExpectEnd(true, 2);

  scheduler.Schedule(&group);

  scheduler.Run();
  command2->SetFinished(true);
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(&group));

  command2->SetFinished(false);

  scheduler.Schedule(&group);

  scheduler.Run();
  CHECK(scheduler.IsScheduled(&group));

  scheduler.Run();
  CHECK(scheduler.IsScheduled(&group));

  command2->SetFinished(true);
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(&group));
}

INSTANTIATE_MULTI_COMMAND_COMPOSITION_TEST_SUITE(ParallelRaceGroupTest,
                                                 ParallelRaceGroup);
