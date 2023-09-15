// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "CompositionTestBase.h"
#include "frc/command2/InstantCommand.h"
#include "frc/command2/SequentialCommandGroup.h"
#include "frc/command2/WaitUntilCommand.h"

class SequentialCommandGroupTest : public CommandTestBase {};

TEST_F(SequentialCommandGroupTest, SequentialGroupSchedule) {
  frc::CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command3Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();
  MockCommand* command3 = command3Holder.get();

  frc::SequentialCommandGroup group{
      tcb::make_vector<std::unique_ptr<frc::Command>>(
          std::move(command1Holder), std::move(command2Holder),
          std::move(command3Holder))};

  EXPECT_CALL(*command1, Initialize());
  EXPECT_CALL(*command1, Execute()).Times(1);
  EXPECT_CALL(*command1, End(false));

  EXPECT_CALL(*command2, Initialize());
  EXPECT_CALL(*command2, Execute()).Times(1);
  EXPECT_CALL(*command2, End(false));

  EXPECT_CALL(*command3, Initialize());
  EXPECT_CALL(*command3, Execute()).Times(1);
  EXPECT_CALL(*command3, End(false));

  scheduler.Schedule(&group);

  command1->SetFinished(true);
  scheduler.Run();
  command2->SetFinished(true);
  scheduler.Run();
  command3->SetFinished(true);
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

TEST_F(SequentialCommandGroupTest, SequentialGroupInterrupt) {
  frc::CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> command1Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command2Holder = std::make_unique<MockCommand>();
  std::unique_ptr<MockCommand> command3Holder = std::make_unique<MockCommand>();

  MockCommand* command1 = command1Holder.get();
  MockCommand* command2 = command2Holder.get();
  MockCommand* command3 = command3Holder.get();

  frc::SequentialCommandGroup group{
      tcb::make_vector<std::unique_ptr<frc::Command>>(
          std::move(command1Holder), std::move(command2Holder),
          std::move(command3Holder))};

  EXPECT_CALL(*command1, Initialize());
  EXPECT_CALL(*command1, Execute()).Times(1);
  EXPECT_CALL(*command1, End(false));

  EXPECT_CALL(*command2, Initialize());
  EXPECT_CALL(*command2, Execute()).Times(0);
  EXPECT_CALL(*command2, End(false)).Times(0);
  EXPECT_CALL(*command2, End(true));

  EXPECT_CALL(*command3, Initialize()).Times(0);
  EXPECT_CALL(*command3, Execute()).Times(0);
  EXPECT_CALL(*command3, End(false)).Times(0);
  EXPECT_CALL(*command3, End(true)).Times(0);

  scheduler.Schedule(&group);

  command1->SetFinished(true);
  scheduler.Run();
  scheduler.Cancel(&group);
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

TEST_F(SequentialCommandGroupTest, SequentialGroupNotScheduledCancel) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::SequentialCommandGroup group{frc::InstantCommand(),
                                    frc::InstantCommand()};

  EXPECT_NO_FATAL_FAILURE(scheduler.Cancel(&group));
}

TEST_F(SequentialCommandGroupTest, SequentialGroupCopy) {
  frc::CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  frc::WaitUntilCommand command([&finished] { return finished; });

  frc::SequentialCommandGroup group(command);
  scheduler.Schedule(&group);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&group));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

TEST_F(SequentialCommandGroupTest, SequentialGroupRequirement) {
  frc::CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  frc::InstantCommand command1([] {}, {&requirement1, &requirement2});
  frc::InstantCommand command2([] {}, {&requirement3});
  frc::InstantCommand command3([] {}, {&requirement3, &requirement4});

  frc::SequentialCommandGroup group(std::move(command1), std::move(command2));

  scheduler.Schedule(&group);
  scheduler.Schedule(&command3);

  EXPECT_TRUE(scheduler.IsScheduled(&command3));
  EXPECT_FALSE(scheduler.IsScheduled(&group));
}

INSTANTIATE_MULTI_COMMAND_COMPOSITION_TEST_SUITE(SequentialCommandGroupTest,
                                                 frc::SequentialCommandGroup);
