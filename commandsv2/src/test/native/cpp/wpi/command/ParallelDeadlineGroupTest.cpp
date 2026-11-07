// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/ParallelDeadlineGroup.hpp"

#include <memory>
#include <utility>
#include <vector>

#include "CommandTestBase.hpp"
#include "CompositionTestBase.hpp"
#include "wpi/commands2/InstantCommand.hpp"
#include "wpi/commands2/WaitUntilCommand.hpp"

using namespace wpi::cmd;
class ParallelDeadlineGroupTest : public CommandTestBase {};

TEST_CASE_METHOD(ParallelDeadlineGroupTest,
                 "ParallelDeadlineGroupTest DeadlineGroupSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command3Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();
  MockCommand* command3 = command3Holder.get();

  ParallelDeadlineGroup group(
      std::move(command1Holder),
      make_vector<std::unique_ptr<Command>>(std::move(command2Holder),
                                            std::move(command3Holder)));

  command1->ExpectInitialize(1);
  command1->ExpectExecute(2);
  command1->ExpectEnd(false, 1);

  command2->ExpectInitialize(1);
  command2->ExpectExecute(1);
  command2->ExpectEnd(false, 1);

  command3->ExpectInitialize(1);
  command3->ExpectExecute(2);
  command3->ExpectEnd(true, 1);

  scheduler.Schedule(&group);

  command2->SetFinished(true);
  scheduler.Run();
  command1->SetFinished(true);
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(&group));
}

TEST_CASE_METHOD(ParallelDeadlineGroupTest,
                 "ParallelDeadlineGroupTest SequentialGroupInterrupt",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem subsystem;

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command3Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();
  MockCommand* command3 = command3Holder.get();

  ParallelDeadlineGroup group(
      std::move(command1Holder),
      make_vector<std::unique_ptr<Command>>(std::move(command2Holder),
                                            std::move(command3Holder)));

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

TEST_CASE_METHOD(ParallelDeadlineGroupTest,
                 "ParallelDeadlineGroupTest DeadlineGroupNotScheduledCancel",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  auto group = Deadline(None(), None());

  CHECK_NOTHROW(scheduler.Cancel(group));
}

TEST_CASE_METHOD(ParallelDeadlineGroupTest,
                 "ParallelDeadlineGroupTest ParallelDeadlineCopy",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = WaitUntil([&finished] { return finished; });

  auto group = Deadline(std::move(command));
  scheduler.Schedule(group);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(group));
  finished = true;
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(group));
}

TEST_CASE_METHOD(ParallelDeadlineGroupTest,
                 "ParallelDeadlineGroupTest ParallelDeadlineRequirement",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  auto command1 = RunOnce([] {}, {&requirement1, &requirement2});
  auto command2 = RunOnce([] {}, {&requirement3});
  auto command3 = RunOnce([] {}, {&requirement3, &requirement4});

  auto group = Deadline(std::move(command1), std::move(command2));

  scheduler.Schedule(group);
  scheduler.Schedule(command3);

  CHECK(scheduler.IsScheduled(command3));
  CHECK_FALSE(scheduler.IsScheduled(group));
}

class TestableDeadlineCommand : public ParallelDeadlineGroup {
  static ParallelDeadlineGroup ToCommand(
      std::vector<std::unique_ptr<Command>>&& commands) {
    std::vector<std::unique_ptr<Command>> vec;
    std::unique_ptr<Command> deadline = std::move(commands[0]);
    for (unsigned int i = 1; i < commands.size(); i++) {
      vec.emplace_back(std::move(commands[i]));
    }
    return ParallelDeadlineGroup(std::move(deadline), std::move(vec));
  }

 public:
  explicit TestableDeadlineCommand(
      std::vector<std::unique_ptr<Command>>&& commands)
      : ParallelDeadlineGroup(ToCommand(std::move(commands))) {}
};

INSTANTIATE_MULTI_COMMAND_COMPOSITION_TEST_SUITE(ParallelDeadlineGroupTest,
                                                 TestableDeadlineCommand);
