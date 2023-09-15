// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc/command2/Commands.h"
#include "frc/command2/ConditionalCommand.h"
#include "frc/command2/InstantCommand.h"

class ConditionalCommandTest : public CommandTestBase {};

TEST_F(ConditionalCommandTest, ConditionalCommandSchedule) {
  frc::CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> mock = std::make_unique<MockCommand>();
  MockCommand* mockptr = mock.get();

  EXPECT_CALL(*mock, Initialize());
  EXPECT_CALL(*mock, Execute()).Times(2);
  EXPECT_CALL(*mock, End(false));

  frc::ConditionalCommand conditional(std::move(mock),
                                      std::make_unique<frc::InstantCommand>(),
                                      [] { return true; });

  scheduler.Schedule(&conditional);
  scheduler.Run();
  mockptr->SetFinished(true);
  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&conditional));
}

TEST_F(ConditionalCommandTest, ConditionalCommandRequirement) {
  frc::CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement1;
  TestSubsystem requirement2;
  TestSubsystem requirement3;
  TestSubsystem requirement4;

  frc::InstantCommand command1([] {}, {&requirement1, &requirement2});
  frc::InstantCommand command2([] {}, {&requirement3});
  frc::InstantCommand command3([] {}, {&requirement3, &requirement4});

  frc::ConditionalCommand conditional(std::move(command1), std::move(command2),
                                      [] { return true; });
  scheduler.Schedule(&conditional);
  scheduler.Schedule(&command3);

  EXPECT_TRUE(scheduler.IsScheduled(&command3));
  EXPECT_FALSE(scheduler.IsScheduled(&conditional));
}

TEST_F(ConditionalCommandTest, AllTrue) {
  frc::CommandPtr command = frc::cmd::Either(
      frc::cmd::WaitUntil([] { return false; }).IgnoringDisable(true),
      frc::cmd::WaitUntil([] { return false; }).IgnoringDisable(true),
      [] { return true; });
  EXPECT_EQ(true, command.get()->RunsWhenDisabled());
}

TEST_F(ConditionalCommandTest, AllFalse) {
  frc::CommandPtr command = frc::cmd::Either(
      frc::cmd::WaitUntil([] { return false; }).IgnoringDisable(false),
      frc::cmd::WaitUntil([] { return false; }).IgnoringDisable(false),
      [] { return true; });
  EXPECT_EQ(false, command.get()->RunsWhenDisabled());
}

TEST_F(ConditionalCommandTest, OneTrueOneFalse) {
  frc::CommandPtr command = frc::cmd::Either(
      frc::cmd::WaitUntil([] { return false; }).IgnoringDisable(true),
      frc::cmd::WaitUntil([] { return false; }).IgnoringDisable(false),
      [] { return true; });
  EXPECT_EQ(false, command.get()->RunsWhenDisabled());
}

TEST_F(ConditionalCommandTest, TwoFalseOneTrue) {
  frc::CommandPtr command = frc::cmd::Either(
      frc::cmd::WaitUntil([] { return false; }).IgnoringDisable(false),
      frc::cmd::WaitUntil([] { return false; }).IgnoringDisable(true),
      [] { return true; });
  EXPECT_EQ(false, command.get()->RunsWhenDisabled());
}

TEST_F(ConditionalCommandTest, AllCancelSelf) {
  frc::CommandPtr command = frc::cmd::Either(
      frc::cmd::WaitUntil([] {
        return false;
      }).WithInterruptBehavior(frc::Command::InterruptionBehavior::kCancelSelf),
      frc::cmd::WaitUntil([] {
        return false;
      }).WithInterruptBehavior(frc::Command::InterruptionBehavior::kCancelSelf),
      [] { return true; });
  EXPECT_EQ(frc::Command::InterruptionBehavior::kCancelSelf,
            command.get()->GetInterruptionBehavior());
}

TEST_F(ConditionalCommandTest, AllCancelIncoming) {
  frc::CommandPtr command = frc::cmd::Either(
      frc::cmd::WaitUntil([] { return false; })
          .WithInterruptBehavior(
              frc::Command::InterruptionBehavior::kCancelIncoming),
      frc::cmd::WaitUntil([] { return false; })
          .WithInterruptBehavior(
              frc::Command::InterruptionBehavior::kCancelIncoming),
      [] { return false; });
  EXPECT_EQ(frc::Command::InterruptionBehavior::kCancelIncoming,
            command.get()->GetInterruptionBehavior());
}

TEST_F(ConditionalCommandTest, OneCancelSelfOneIncoming) {
  frc::CommandPtr command = frc::cmd::Either(
      frc::cmd::WaitUntil([] {
        return false;
      }).WithInterruptBehavior(frc::Command::InterruptionBehavior::kCancelSelf),
      frc::cmd::WaitUntil([] { return false; })
          .WithInterruptBehavior(
              frc::Command::InterruptionBehavior::kCancelIncoming),
      [] { return false; });
  EXPECT_EQ(frc::Command::InterruptionBehavior::kCancelSelf,
            command.get()->GetInterruptionBehavior());
}

TEST_F(ConditionalCommandTest, OneCancelIncomingOneSelf) {
  frc::CommandPtr command = frc::cmd::Either(
      frc::cmd::WaitUntil([] { return false; })
          .WithInterruptBehavior(
              frc::Command::InterruptionBehavior::kCancelIncoming),
      frc::cmd::WaitUntil([] {
        return false;
      }).WithInterruptBehavior(frc::Command::InterruptionBehavior::kCancelSelf),
      [] { return false; });
  EXPECT_EQ(frc::Command::InterruptionBehavior::kCancelSelf,
            command.get()->GetInterruptionBehavior());
}
