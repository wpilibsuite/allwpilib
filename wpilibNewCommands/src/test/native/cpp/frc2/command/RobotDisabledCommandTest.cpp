// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc2/command/ConditionalCommand.h"
#include "frc2/command/ParallelCommandGroup.h"
#include "frc2/command/ParallelDeadlineGroup.h"
#include "frc2/command/ParallelRaceGroup.h"
#include "frc2/command/SelectCommand.h"
#include "frc2/command/SequentialCommandGroup.h"

using namespace frc2;
class RobotDisabledCommandTest : public CommandTestBase {};

TEST_F(RobotDisabledCommandTest, RobotDisabledCommandCancel) {
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
  SequentialCommandGroup runWhenDisabled{MockCommand(), MockCommand()};
  SequentialCommandGroup dontRunWhenDisabled{MockCommand(),
                                             MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_F(RobotDisabledCommandTest, ParallelGroupRunWhenDisabled) {
  ParallelCommandGroup runWhenDisabled{MockCommand(), MockCommand()};
  ParallelCommandGroup dontRunWhenDisabled{MockCommand(),
                                           MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_F(RobotDisabledCommandTest, ParallelRaceRunWhenDisabled) {
  ParallelRaceGroup runWhenDisabled{MockCommand(), MockCommand()};
  ParallelRaceGroup dontRunWhenDisabled{MockCommand(),
                                        MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_F(RobotDisabledCommandTest, ParallelDeadlineRunWhenDisabled) {
  ParallelDeadlineGroup runWhenDisabled{MockCommand(), MockCommand()};
  ParallelDeadlineGroup dontRunWhenDisabled{MockCommand(),
                                            MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_F(RobotDisabledCommandTest, ConditionalCommandRunWhenDisabled) {
  ConditionalCommand runWhenDisabled{MockCommand(), MockCommand(),
                                     [] { return true; }};
  ConditionalCommand dontRunWhenDisabled{
      MockCommand(), MockCommand({}, false, false), [] { return true; }};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_F(RobotDisabledCommandTest, SelectCommandRunWhenDisabled) {
  SelectCommand<int> runWhenDisabled{[] { return 1; },
                                     std::pair(1, MockCommand()),
                                     std::pair(1, MockCommand())};
  SelectCommand<int> dontRunWhenDisabled{
      [] { return 1; }, std::pair(1, MockCommand()),
      std::pair(1, MockCommand({}, false, false))};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  EXPECT_TRUE(scheduler.IsScheduled(&runWhenDisabled));
  EXPECT_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}
