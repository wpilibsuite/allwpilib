// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc/command2/ConditionalCommand.h"
#include "frc/command2/ParallelCommandGroup.h"
#include "frc/command2/ParallelDeadlineGroup.h"
#include "frc/command2/ParallelRaceGroup.h"
#include "frc/command2/SelectCommand.h"
#include "frc/command2/SequentialCommandGroup.h"

class RobotDisabledCommandTest : public CommandTestBase {};

TEST_F(RobotDisabledCommandTest, RobotDisabledCommandCancel) {
  frc::CommandScheduler scheduler = GetScheduler();

  MockCommand command({}, false, false);

  EXPECT_CALL(command, End(true));

  SetDSEnabled(true);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(&command));

  SetDSEnabled(false);

  scheduler.Run();

  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(RobotDisabledCommandTest, RunWhenDisabled) {
  frc::CommandScheduler scheduler = GetScheduler();

  MockCommand command1;
  MockCommand command2;

  scheduler.Schedule(&command1);

  SetDSEnabled(false);

  scheduler.Run();

  scheduler.Schedule(&command2);

  EXPECT_TRUE(scheduler.IsScheduled(&command1));
  EXPECT_TRUE(scheduler.IsScheduled(&command2));
}

TEST_F(RobotDisabledCommandTest, SequentialGroupRunWhenDisabled) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::SequentialCommandGroup runWhenDisabled{MockCommand(), MockCommand()};
  frc::SequentialCommandGroup dontRunWhenDisabled{
      MockCommand(), MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_F(RobotDisabledCommandTest, ParallelGroupRunWhenDisabled) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::ParallelCommandGroup runWhenDisabled{MockCommand(), MockCommand()};
  frc::ParallelCommandGroup dontRunWhenDisabled{MockCommand(),
                                                MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_F(RobotDisabledCommandTest, ParallelRaceRunWhenDisabled) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::ParallelRaceGroup runWhenDisabled{MockCommand(), MockCommand()};
  frc::ParallelRaceGroup dontRunWhenDisabled{MockCommand(),
                                             MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_F(RobotDisabledCommandTest, ParallelDeadlineRunWhenDisabled) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::ParallelDeadlineGroup runWhenDisabled{MockCommand(), MockCommand()};
  frc::ParallelDeadlineGroup dontRunWhenDisabled{MockCommand(),
                                                 MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_F(RobotDisabledCommandTest, ConditionalCommandRunWhenDisabled) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::ConditionalCommand runWhenDisabled{MockCommand(), MockCommand(),
                                          [] { return true; }};
  frc::ConditionalCommand dontRunWhenDisabled{
      MockCommand(), MockCommand({}, false, false), [] { return true; }};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_F(RobotDisabledCommandTest, SelectCommandRunWhenDisabled) {
  frc::CommandScheduler scheduler = GetScheduler();

  frc::SelectCommand<int> runWhenDisabled{[] { return 1; },
                                          std::pair(1, MockCommand()),
                                          std::pair(1, MockCommand())};
  frc::SelectCommand<int> dontRunWhenDisabled{
      [] { return 1; }, std::pair(1, MockCommand()),
      std::pair(1, MockCommand({}, false, false))};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}
