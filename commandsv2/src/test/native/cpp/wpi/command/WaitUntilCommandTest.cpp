// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"

using namespace wpi::cmd;
class WaitUntilCommandTest : public CommandTestBase {};

TEST_CASE_METHOD(WaitUntilCommandTest,
                 "WaitUntilCommandTest WaitUntilCommandSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  bool finished = false;

  auto command = WaitUntil([&finished] { return finished; });

  scheduler.Schedule(command);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(command));
  finished = true;
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(command));
}
