// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"

using namespace wpi::cmd;
class RunCommandTest : public CommandTestBase {};

TEST_CASE_METHOD(RunCommandTest, "RunCommandTest RunCommandSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  auto command = wpi::cmd::Run([&counter] { counter++; });

  scheduler.Schedule(command);
  scheduler.Run();
  scheduler.Run();
  scheduler.Run();

  CHECK(3 == counter);
}
