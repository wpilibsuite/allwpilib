// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include <frc/Errors.h>

#include "CommandTestBase.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/FunctionalCommand.h"

using namespace frc2;
class CommandRequirementsTest : public CommandTestBase {};

TEST_F(CommandRequirementsTest, RequirementInterrupt) {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement;

  MockCommand command1({&requirement});
  MockCommand command2({&requirement});

  EXPECT_CALL(command1, Initialize());
  EXPECT_CALL(command1, Execute());
  EXPECT_CALL(command1, End(true));
  EXPECT_CALL(command1, End(false)).Times(0);

  EXPECT_CALL(command2, Initialize());
  EXPECT_CALL(command2, Execute());
  EXPECT_CALL(command2, End(true)).Times(0);
  EXPECT_CALL(command2, End(false)).Times(0);

  scheduler.Schedule(&command1);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command1));
  scheduler.Schedule(&command2);
  EXPECT_FALSE(scheduler.IsScheduled(&command1));
  EXPECT_TRUE(scheduler.IsScheduled(&command2));
  scheduler.Run();
}

TEST_F(CommandRequirementsTest, RequirementUninterruptible) {
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

  EXPECT_CALL(command2, Initialize()).Times(0);
  EXPECT_CALL(command2, Execute()).Times(0);
  EXPECT_CALL(command2, End(true)).Times(0);
  EXPECT_CALL(command2, End(false)).Times(0);

  scheduler.Schedule(command1);
  EXPECT_EQ(1, initCounter);
  scheduler.Run();
  EXPECT_EQ(1, exeCounter);
  EXPECT_TRUE(scheduler.IsScheduled(command1));
  scheduler.Schedule(&command2);
  EXPECT_TRUE(scheduler.IsScheduled(command1));
  EXPECT_FALSE(scheduler.IsScheduled(&command2));
  scheduler.Run();
  EXPECT_EQ(2, exeCounter);
  EXPECT_EQ(0, endCounter);
}

TEST_F(CommandRequirementsTest, DefaultCommandRequirementError) {
  TestSubsystem requirement1;

  MockCommand command1;

  ASSERT_THROW(requirement1.SetDefaultCommand(std::move(command1)),
               frc::RuntimeError);
}
