// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include "CommandTestBase.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"
#include "wpi/system/Errors.hpp"

using namespace wpi::cmd;
class CommandRequirementsTest : public CommandTestBase {};

TEST_CASE_METHOD(CommandRequirementsTest,
                 "CommandRequirementsTest RequirementInterrupt",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement;

  MockCommand command1({&requirement});
  MockCommand command2({&requirement});

  command1.ExpectInitialize(1);
  command1.ExpectExecute(1);
  command1.ExpectEnd(true, 1);
  command1.ExpectEnd(false, 0);

  command2.ExpectInitialize(1);
  command2.ExpectExecute(1);
  command2.ExpectEnd(true, 0);
  command2.ExpectEnd(false, 0);

  scheduler.Schedule(&command1);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(&command1));
  scheduler.Schedule(&command2);
  CHECK_FALSE(scheduler.IsScheduled(&command1));
  CHECK(scheduler.IsScheduled(&command2));
  scheduler.Run();
}

TEST_CASE_METHOD(CommandRequirementsTest,
                 "CommandRequirementsTest RequirementUninterruptible",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement;

  int initCounter = 0;
  int exeCounter = 0;
  int endCounter = 0;

  CommandPtr command1 =
      FunctionalCommand([&initCounter] { initCounter++; },
                        [&exeCounter] { exeCounter++; },
                        [&endCounter](bool interruptible) { endCounter++; },
                        [] { return false; }, {&requirement})
          .WithInterruptBehavior(
              Command::InterruptionBehavior::kCancelIncoming);
  MockCommand command2({&requirement});

  command2.ExpectInitialize(0);
  command2.ExpectExecute(0);
  command2.ExpectEnd(true, 0);
  command2.ExpectEnd(false, 0);

  scheduler.Schedule(command1);
  CHECK(1 == initCounter);
  scheduler.Run();
  CHECK(1 == exeCounter);
  CHECK(scheduler.IsScheduled(command1));
  scheduler.Schedule(&command2);
  CHECK(scheduler.IsScheduled(command1));
  CHECK_FALSE(scheduler.IsScheduled(&command2));
  scheduler.Run();
  CHECK(2 == exeCounter);
  CHECK(0 == endCounter);
}

TEST_CASE_METHOD(CommandRequirementsTest,
                 "CommandRequirementsTest DefaultCommandRequirementError",
                 "[commandsv2][command]") {
  TestSubsystem requirement1;

  MockCommand command1;

  REQUIRE_THROWS_AS(requirement1.SetDefaultCommand(std::move(command1)),
                    wpi::RuntimeError);
}
