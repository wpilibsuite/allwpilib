// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/ParallelCommandGroup.hpp"

#include <memory>
#include <utility>

#include "CommandTestBase.hpp"
#include "CompositionTestBase.hpp"
#include "wpi/commands2/Commands.hpp"

using namespace wpi::cmd;
class ParallelCommandGroupTest : public CommandTestBase {};

TEST_CASE_METHOD(ParallelCommandGroupTest,
                 "ParallelCommandGroupTest ParallelGroupSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();

  ParallelCommandGroup group(make_vector<std::unique_ptr<Command>>(
      std::move(command1Holder), std::move(command2Holder)));

  command1->ExpectInitialize(1);
  command1->ExpectExecute(1);
  command1->ExpectEnd(false, 1);

  command2->ExpectInitialize(1);
  command2->ExpectExecute(2);
  command2->ExpectEnd(false, 1);

  scheduler.Schedule(&group);

  command1->SetFinished(true);
  scheduler.Run();
  command2->SetFinished(true);
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(&group));
}

TEST_CASE_METHOD(ParallelCommandGroupTest,
                 "ParallelCommandGroupTest ParallelGroupInterrupt",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();

  ParallelCommandGroup group(make_vector<std::unique_ptr<Command>>(
      std::move(command1Holder), std::move(command2Holder)));

  command1->ExpectInitialize(1);
  command1->ExpectExecute(1);
  command1->ExpectEnd(false, 1);

  command2->ExpectInitialize(1);
  command2->ExpectExecute(2);
  command2->ExpectEnd(false, 0);
  command2->ExpectEnd(true, 1);

  scheduler.Schedule(&group);

  command1->SetFinished(true);
  scheduler.Run();
  scheduler.Run();
  scheduler.Cancel(&group);

  CHECK_FALSE(scheduler.IsScheduled(&group));
}

TEST_CASE_METHOD(ParallelCommandGroupTest,
                 "ParallelCommandGroupTest ParallelGroupNotScheduledCancel",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  auto group = Parallel(None(), None());

  CHECK_NOTHROW(scheduler.Cancel(group));
}

TEST_CASE_METHOD(ParallelCommandGroupTest,
                 "ParallelCommandGroupTest ParallelGroupCopy",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = WaitUntil([&finished] { return finished; });

  auto group = Parallel(std::move(command));
  scheduler.Schedule(group);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(group));
  finished = true;
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(group));
}

TEST_CASE_METHOD(ParallelCommandGroupTest,
                 "ParallelCommandGroupTest ParallelGroupRequirement",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  auto command1 = RunOnce([] {}, {&requirement1, &requirement2});
  auto command2 = RunOnce([] {}, {&requirement3});
  auto command3 = RunOnce([] {}, {&requirement3, &requirement4});

  auto group = Parallel(std::move(command1), std::move(command2));

  scheduler.Schedule(group);
  scheduler.Schedule(command3);

  CHECK(scheduler.IsScheduled(command3));
  CHECK_FALSE(scheduler.IsScheduled(group));
}

INSTANTIATE_MULTI_COMMAND_COMPOSITION_TEST_SUITE(ParallelCommandGroupTest,
                                                 ParallelCommandGroup);
