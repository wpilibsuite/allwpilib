// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"

using namespace frc2;
class CommandScheduleTest : public CommandTestBase {};

TEST_F(CommandScheduleTest, InstantSchedule) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  EXPECT_CALL(command, Initialize());
  EXPECT_CALL(command, Execute());
  EXPECT_CALL(command, End(false));

  command.SetFinished(true);
  scheduler.Schedule(&command);
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(CommandScheduleTest, SingleIterationSchedule) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  EXPECT_CALL(command, Initialize());
  EXPECT_CALL(command, Execute()).Times(2);
  EXPECT_CALL(command, End(false));

  scheduler.Schedule(&command);
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  scheduler.Run();
  command.SetFinished(true);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(CommandScheduleTest, MultiSchedule) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command1;
  MockCommand command2;
  MockCommand command3;

  EXPECT_CALL(command1, Initialize());
  EXPECT_CALL(command1, Execute()).Times(2);
  EXPECT_CALL(command1, End(false));

  EXPECT_CALL(command2, Initialize());
  EXPECT_CALL(command2, Execute()).Times(3);
  EXPECT_CALL(command2, End(false));

  EXPECT_CALL(command3, Initialize());
  EXPECT_CALL(command3, Execute()).Times(4);
  EXPECT_CALL(command3, End(false));

  scheduler.Schedule(&command1);
  scheduler.Schedule(&command2);
  scheduler.Schedule(&command3);
  EXPECT_TRUE(scheduler.IsScheduled({&command1, &command2, &command3}));
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled({&command1, &command2, &command3}));
  command1.SetFinished(true);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled({&command2, &command3}));
  EXPECT_FALSE(scheduler.IsScheduled(&command1));
  command2.SetFinished(true);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command3));
  EXPECT_FALSE(scheduler.IsScheduled({&command1, &command2}));
  command3.SetFinished(true);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled({&command1, &command2, &command3}));
}

TEST_F(CommandScheduleTest, SchedulerCancel) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  EXPECT_CALL(command, Initialize());
  EXPECT_CALL(command, Execute());
  EXPECT_CALL(command, End(false)).Times(0);
  EXPECT_CALL(command, End(true));

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  scheduler.Cancel(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(CommandScheduleTest, NotScheduledCancel) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  EXPECT_NO_FATAL_FAILURE(scheduler.Cancel(&command));
}
