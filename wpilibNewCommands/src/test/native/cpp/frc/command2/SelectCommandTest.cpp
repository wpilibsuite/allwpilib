// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "CompositionTestBase.h"
#include "frc/command2/ConditionalCommand.h"
#include "frc/command2/InstantCommand.h"
#include "frc/command2/SelectCommand.h"

class SelectCommandTest : public CommandTestBase {};

TEST_F(SelectCommandTest, SelectCommand) {
  frc::CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> mock = std::make_unique<MockCommand>();
  MockCommand* mockptr = mock.get();

  EXPECT_CALL(*mock, Initialize());
  EXPECT_CALL(*mock, Execute()).Times(2);
  EXPECT_CALL(*mock, End(false));

  std::vector<std::pair<int, std::unique_ptr<frc::Command>>> temp;

  temp.emplace_back(std::pair(1, std::move(mock)));
  temp.emplace_back(std::pair(2, std::make_unique<frc::InstantCommand>()));
  temp.emplace_back(std::pair(3, std::make_unique<frc::InstantCommand>()));

  frc::SelectCommand<int> select([] { return 1; }, std::move(temp));

  scheduler.Schedule(&select);
  scheduler.Run();
  mockptr->SetFinished(true);
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&select));
}

TEST_F(SelectCommandTest, SelectCommandRequirement) {
  frc::CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  frc::InstantCommand command1([] {}, {&requirement1, &requirement2});
  frc::InstantCommand command2([] {}, {&requirement3});
  frc::InstantCommand command3([] {}, {&requirement3, &requirement4});

  frc::SelectCommand<int> select([] { return 1; },
                                 std::pair(1, std::move(command1)),
                                 std::pair(2, std::move(command2)));

  scheduler.Schedule(&select);
  scheduler.Schedule(&command3);

  EXPECT_TRUE(scheduler.IsScheduled(&command3));
  EXPECT_FALSE(scheduler.IsScheduled(&select));
}

class TestableSelectCommand : public frc::SelectCommand<int> {
  static std::vector<std::pair<int, std::unique_ptr<Command>>> ZipVector(
      std::vector<std::unique_ptr<Command>>&& commands) {
    std::vector<std::pair<int, std::unique_ptr<Command>>> vec;
    int index = 0;
    for (auto&& command : commands) {
      vec.emplace_back(std::make_pair(index, std::move(command)));
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
