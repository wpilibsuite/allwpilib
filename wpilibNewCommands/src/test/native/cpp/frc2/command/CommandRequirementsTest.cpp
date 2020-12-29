// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/ConditionalCommand.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ParallelCommandGroup.h"
#include "frc2/command/ParallelDeadlineGroup.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/SelectCommand.h"
#include "frc2/command/SequentialCommandGroup.h"

using namespace frc2;
class CommandRequirementsTest : public CommandTestBase {};

TEST_F(CommandRequirementsTest, RequirementInterruptTest) {
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

TEST_F(CommandRequirementsTest, RequirementUninterruptibleTest) {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem requirement;

  MockCommand command1({&requirement});
  MockCommand command2({&requirement});

  EXPECT_CALL(command1, Initialize());
  EXPECT_CALL(command1, Execute()).Times(2);
  EXPECT_CALL(command1, End(true)).Times(0);
  EXPECT_CALL(command1, End(false)).Times(0);

  EXPECT_CALL(command2, Initialize()).Times(0);
  EXPECT_CALL(command2, Execute()).Times(0);
  EXPECT_CALL(command2, End(true)).Times(0);
  EXPECT_CALL(command2, End(false)).Times(0);

  scheduler.Schedule(false, &command1);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command1));
  scheduler.Schedule(&command2);
  EXPECT_TRUE(scheduler.IsScheduled(&command1));
  EXPECT_FALSE(scheduler.IsScheduled(&command2));
  scheduler.Run();
}

TEST_F(CommandRequirementsTest, DefaultCommandRequirementErrorTest) {
  TestSubsystem requirement1;
  ErrorConfirmer confirmer("require");

  MockCommand command1;

  requirement1.SetDefaultCommand(std::move(command1));

  EXPECT_TRUE(requirement1.GetDefaultCommand() == NULL);
}
