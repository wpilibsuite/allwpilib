// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/ConditionalCommand.hpp"

#include <memory>
#include <utility>

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/InstantCommand.hpp"

using namespace wpi::cmd;
class ConditionalCommandTest : public CommandTestBase {};

TEST_CASE_METHOD(ConditionalCommandTest,
                 "ConditionalCommandTest ConditionalCommandSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  std::unique_ptr<MockCommand> mock = std::make_unique<MockCommand>();
  MockCommand* mockptr = mock.get();

  mock->ExpectInitialize(1);
  mock->ExpectExecute(2);
  mock->ExpectEnd(false, 1);

  ConditionalCommand conditional(
      std::move(mock), std::make_unique<InstantCommand>(), [] { return true; });

  scheduler.Schedule(&conditional);
  scheduler.Run();
  mockptr->SetFinished(true);
  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(&conditional));
}

TEST_CASE_METHOD(ConditionalCommandTest,
                 "ConditionalCommandTest ConditionalCommandRequirement",
                 "[commandsv2][command]") {
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

  CHECK(scheduler.IsScheduled(&command3));
  CHECK_FALSE(scheduler.IsScheduled(&conditional));
}

TEST_CASE_METHOD(ConditionalCommandTest, "ConditionalCommandTest AllTrue",
                 "[commandsv2][command]") {
  auto command = Either(Idle().IgnoringDisable(true),
                        Idle().IgnoringDisable(true), [] { return true; });
  CHECK(true == command.get()->RunsWhenDisabled());
}

TEST_CASE_METHOD(ConditionalCommandTest, "ConditionalCommandTest AllFalse",
                 "[commandsv2][command]") {
  auto command = Either(Idle().IgnoringDisable(false),
                        Idle().IgnoringDisable(false), [] { return true; });
  CHECK(false == command.get()->RunsWhenDisabled());
}

TEST_CASE_METHOD(ConditionalCommandTest,
                 "ConditionalCommandTest OneTrueOneFalse",
                 "[commandsv2][command]") {
  auto command = Either(Idle().IgnoringDisable(true),
                        Idle().IgnoringDisable(false), [] { return true; });
  CHECK(false == command.get()->RunsWhenDisabled());
}

TEST_CASE_METHOD(ConditionalCommandTest,
                 "ConditionalCommandTest TwoFalseOneTrue",
                 "[commandsv2][command]") {
  auto command = Either(Idle().IgnoringDisable(false),
                        Idle().IgnoringDisable(true), [] { return true; });
  CHECK(false == command.get()->RunsWhenDisabled());
}

TEST_CASE_METHOD(ConditionalCommandTest, "ConditionalCommandTest AllCancelSelf",
                 "[commandsv2][command]") {
  auto command = Either(
      Idle().WithInterruptBehavior(Command::InterruptionBehavior::kCancelSelf),
      Idle().WithInterruptBehavior(Command::InterruptionBehavior::kCancelSelf),
      [] { return true; });
  CHECK(Command::InterruptionBehavior::kCancelSelf ==
        command.get()->GetInterruptionBehavior());
}

TEST_CASE_METHOD(ConditionalCommandTest,
                 "ConditionalCommandTest AllCancelIncoming",
                 "[commandsv2][command]") {
  auto command = Either(Idle().WithInterruptBehavior(
                            Command::InterruptionBehavior::kCancelIncoming),
                        Idle().WithInterruptBehavior(
                            Command::InterruptionBehavior::kCancelIncoming),
                        [] { return false; });
  CHECK(Command::InterruptionBehavior::kCancelIncoming ==
        command.get()->GetInterruptionBehavior());
}

TEST_CASE_METHOD(ConditionalCommandTest,
                 "ConditionalCommandTest OneCancelSelfOneIncoming",
                 "[commandsv2][command]") {
  auto command = Either(
      Idle().WithInterruptBehavior(Command::InterruptionBehavior::kCancelSelf),
      Idle().WithInterruptBehavior(
          Command::InterruptionBehavior::kCancelIncoming),
      [] { return false; });
  CHECK(Command::InterruptionBehavior::kCancelSelf ==
        command.get()->GetInterruptionBehavior());
}

TEST_CASE_METHOD(ConditionalCommandTest,
                 "ConditionalCommandTest OneCancelIncomingOneSelf",
                 "[commandsv2][command]") {
  auto command = Either(
      Idle().WithInterruptBehavior(
          Command::InterruptionBehavior::kCancelIncoming),
      Idle().WithInterruptBehavior(Command::InterruptionBehavior::kCancelSelf),
      [] { return false; });
  CHECK(Command::InterruptionBehavior::kCancelSelf ==
        command.get()->GetInterruptionBehavior());
}
