// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/WaitCommand.hpp"

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/simulation/SimHooks.hpp"

using namespace wpi::cmd;
class WaitCommandTest : public CommandTestBase {};

TEST_CASE_METHOD(WaitCommandTest, "WaitCommandTest WaitCommandSchedule",
                 "[commandsv2][command]") {
  wpi::sim::PauseTiming();

  CommandScheduler scheduler = GetScheduler();

  auto command = wpi::cmd::Wait(100_ms);

  scheduler.Schedule(command);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(command));
  wpi::sim::StepTiming(110_ms);
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(command));

  wpi::sim::ResumeTiming();
}
