// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/SelectCommand.hpp"

#include <memory>
#include <utility>
#include <vector>

#include "CommandTestBase.hpp"
#include "CompositionTestBase.hpp"
#include "wpi/commands2/InstantCommand.hpp"

using namespace wpi::cmd;
class SelectCommandTest : public CommandTestBase {};

TEST_CASE_METHOD(SelectCommandTest, "SelectCommandTest SelectCommand",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> mock = std::make_unique<MockCommand>();
  MockCommand* mockptr = mock.get();

  mock->ExpectInitialize(1);
  mock->ExpectExecute(2);
  mock->ExpectEnd(false, 1);

  std::vector<std::pair<int, std::unique_ptr<Command>>> temp;

  temp.emplace_back(std::pair(1, std::move(mock)));
  temp.emplace_back(std::pair(2, std::make_unique<InstantCommand>()));
  temp.emplace_back(std::pair(3, std::make_unique<InstantCommand>()));

  SelectCommand<int> select([] { return 1; }, std::move(temp));

  scheduler.Schedule(&select);
  scheduler.Run();
  mockptr->SetFinished(true);
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(&select));
}

TEST_CASE_METHOD(SelectCommandTest,
                 "SelectCommandTest SelectCommandRequirement",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  auto command1 = RunOnce([] {}, {&requirement1, &requirement2});
  auto command2 = RunOnce([] {}, {&requirement3});
  auto command3 = RunOnce([] {}, {&requirement3, &requirement4});

  auto select = Select<int>([] { return 1; }, std::pair(1, std::move(command1)),
                            std::pair(2, std::move(command2)));

  scheduler.Schedule(select);
  scheduler.Schedule(command3);

  CHECK(scheduler.IsScheduled(command3));
  CHECK_FALSE(scheduler.IsScheduled(select));
}

class TestableSelectCommand : public SelectCommand<int> {
  static std::vector<std::pair<int, std::unique_ptr<Command>>> ZipVector(
      std::vector<std::unique_ptr<Command>>&& commands) {
    std::vector<std::pair<int, std::unique_ptr<Command>>> vec;
    int index = 0;
    for (auto&& command : commands) {
      vec.emplace_back(std::pair{index, std::move(command)});
      index++;
    }
    return vec;
  }

 public:
  explicit TestableSelectCommand(
      std::vector<std::unique_ptr<Command>>&& commands)
      : SelectCommand([] { return 0; }, ZipVector(std::move(commands))) {}
};

INSTANTIATE_MULTI_COMMAND_COMPOSITION_TEST_SUITE(SelectCommandTest,
                                                 TestableSelectCommand);
