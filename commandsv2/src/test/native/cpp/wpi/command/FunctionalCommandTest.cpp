// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/FunctionalCommand.hpp"

#include "CommandTestBase.hpp"

using namespace wpi::cmd;
class FunctionalCommandTest : public CommandTestBase {};

TEST_CASE_METHOD(FunctionalCommandTest,
                 "FunctionalCommandTest FunctionalCommandSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;
  bool finished = false;

  FunctionalCommand command(
      [&counter] { counter++; }, [&counter] { counter++; },
      [&counter](bool) { counter++; }, [&finished] { return finished; });

  scheduler.Schedule(&command);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
  CHECK(4 == counter);
}
