/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/RunCommand.h"

using namespace frc2;
class SchedulerTest : public CommandTestBase {};

TEST_F(SchedulerTest, SchedulerLambdaTestNoInterrupt) {
  CommandScheduler scheduler = GetScheduler();

  InstantCommand command;

  int counter = 0;

  scheduler.OnCommandInitialize([&counter](const Command&) { counter++; });
  scheduler.OnCommandExecute([&counter](const Command&) { counter++; });
  scheduler.OnCommandFinish([&counter](const Command&) { counter++; });

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_EQ(counter, 3);
}

TEST_F(SchedulerTest, SchedulerLambdaInterruptTest) {
  CommandScheduler scheduler = GetScheduler();

  RunCommand command([] {}, {});

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const Command&) { counter++; });

  scheduler.Schedule(&command);
  scheduler.Run();
  scheduler.Cancel(&command);

  EXPECT_EQ(counter, 1);
}

TEST_F(SchedulerTest, UnregisterSubsystemTest) {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem system;

  scheduler.RegisterSubsystem(&system);

  EXPECT_NO_FATAL_FAILURE(scheduler.UnregisterSubsystem(&system));
}

TEST_F(SchedulerTest, SchedulerCancelAllTest) {
  CommandScheduler scheduler = GetScheduler();

  RunCommand command([] {}, {});
  RunCommand command2([] {}, {});

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const Command&) { counter++; });

  scheduler.Schedule(&command);
  scheduler.Schedule(&command2);
  scheduler.Run();
  scheduler.CancelAll();

  EXPECT_EQ(counter, 2);
}
