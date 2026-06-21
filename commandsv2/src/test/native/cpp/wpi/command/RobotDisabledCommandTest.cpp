// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/ConditionalCommand.hpp"
#include "wpi/commands2/ParallelCommandGroup.hpp"
#include "wpi/commands2/ParallelDeadlineGroup.hpp"
#include "wpi/commands2/ParallelRaceGroup.hpp"
#include "wpi/commands2/SelectCommand.hpp"
#include "wpi/commands2/SequentialCommandGroup.hpp"

using namespace wpi::cmd;
class RobotDisabledCommandTest : public CommandTestBase {};

TEST_CASE_METHOD(RobotDisabledCommandTest,
                 "RobotDisabledCommandTest RobotDisabledCommandCancel",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  MockCommand command({}, false, false);

  command.ExpectEnd(true, 1);

  SetDSEnabled(true);

  scheduler.Schedule(&command);
  scheduler.Run();

  CHECK(scheduler.IsScheduled(&command));

  SetDSEnabled(false);

  scheduler.Run();

  CHECK_FALSE(scheduler.IsScheduled(&command));
}

TEST_CASE_METHOD(RobotDisabledCommandTest,
                 "RobotDisabledCommandTest RunWhenDisabled",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  MockCommand command1;
  MockCommand command2;

  scheduler.Schedule(&command1);

  SetDSEnabled(false);

  scheduler.Run();

  scheduler.Schedule(&command2);

  CHECK(scheduler.IsScheduled(&command1));
  CHECK(scheduler.IsScheduled(&command2));
}

TEST_CASE_METHOD(RobotDisabledCommandTest,
                 "RobotDisabledCommandTest SequentialGroupRunWhenDisabled",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  SequentialCommandGroup runWhenDisabled{MockCommand(), MockCommand()};
  SequentialCommandGroup dontRunWhenDisabled{MockCommand(),
                                             MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  CHECK(scheduler.IsScheduled(&runWhenDisabled));
  CHECK_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_CASE_METHOD(RobotDisabledCommandTest,
                 "RobotDisabledCommandTest ParallelGroupRunWhenDisabled",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  ParallelCommandGroup runWhenDisabled{MockCommand(), MockCommand()};
  ParallelCommandGroup dontRunWhenDisabled{MockCommand(),
                                           MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  CHECK(scheduler.IsScheduled(&runWhenDisabled));
  CHECK_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_CASE_METHOD(RobotDisabledCommandTest,
                 "RobotDisabledCommandTest ParallelRaceRunWhenDisabled",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  ParallelRaceGroup runWhenDisabled{MockCommand(), MockCommand()};
  ParallelRaceGroup dontRunWhenDisabled{MockCommand(),
                                        MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  CHECK(scheduler.IsScheduled(&runWhenDisabled));
  CHECK_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_CASE_METHOD(RobotDisabledCommandTest,
                 "RobotDisabledCommandTest ParallelDeadlineRunWhenDisabled",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  ParallelDeadlineGroup runWhenDisabled{MockCommand(), MockCommand()};
  ParallelDeadlineGroup dontRunWhenDisabled{MockCommand(),
                                            MockCommand({}, false, false)};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  CHECK(scheduler.IsScheduled(&runWhenDisabled));
  CHECK_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_CASE_METHOD(RobotDisabledCommandTest,
                 "RobotDisabledCommandTest ConditionalCommandRunWhenDisabled",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  ConditionalCommand runWhenDisabled{MockCommand(), MockCommand(),
                                     [] { return true; }};
  ConditionalCommand dontRunWhenDisabled{
      MockCommand(), MockCommand({}, false, false), [] { return true; }};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  CHECK(scheduler.IsScheduled(&runWhenDisabled));
  CHECK_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}

TEST_CASE_METHOD(RobotDisabledCommandTest,
                 "RobotDisabledCommandTest SelectCommandRunWhenDisabled",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  SelectCommand<int> runWhenDisabled{[] { return 1; },
                                     std::pair(1, MockCommand()),
                                     std::pair(1, MockCommand())};
  SelectCommand<int> dontRunWhenDisabled{
      [] { return 1; }, std::pair(1, MockCommand()),
      std::pair(1, MockCommand({}, false, false))};

  SetDSEnabled(false);

  scheduler.Schedule(&runWhenDisabled);
  scheduler.Schedule(&dontRunWhenDisabled);

  CHECK(scheduler.IsScheduled(&runWhenDisabled));
  CHECK_FALSE(scheduler.IsScheduled(&dontRunWhenDisabled));
}
