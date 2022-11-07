// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc2/command/ConditionalCommand.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/SelectCommand.h"

using namespace frc2;
class ConditionalCommandTest : public CommandTestBase {};

TEST_F(ConditionalCommandTest, ConditionalCommandSchedule) {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> mock = std::make_unique<MockCommand>();
  MockCommand* mockptr = mock.get();

  EXPECT_CALL(*mock, Initialize());
  EXPECT_CALL(*mock, Execute()).Times(2);
  EXPECT_CALL(*mock, End(false));

  ConditionalCommand conditional(
      std::move(mock), std::make_unique<InstantCommand>(), [] { return true; });

  scheduler.Schedule(&conditional);
  scheduler.Run();
  mockptr->SetFinished(true);
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&conditional));
}

TEST_F(ConditionalCommandTest, ConditionalCommandRequirement) {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  InstantCommand command1([] {}, {&requirement1, &requirement2});
  InstantCommand command2([] {}, {&requirement3});
  InstantCommand command3([] {}, {&requirement3, &requirement4});

  ConditionalCommand conditional(std::move(command1), std::move(command2),
                                 [] { return true; });
  scheduler.Schedule(&conditional);
  scheduler.Schedule(&command3);

  EXPECT_TRUE(scheduler.IsScheduled(&command3));
  EXPECT_FALSE(scheduler.IsScheduled(&conditional));
}
