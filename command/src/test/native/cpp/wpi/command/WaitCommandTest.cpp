// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/command/Commands.hpp>

#include <wpi/simulation/SimHooks.hpp>

#include "CommandTestBase.h"
#include "wpi/command/WaitCommand.hpp"
#include "wpi/command/WaitUntilCommand.hpp"

using namespace frc2;
class WaitCommandTest : public CommandTestBase {};

TEST_F(WaitCommandTest, WaitCommandSchedule) {
  frc::sim::PauseTiming();

  CommandScheduler scheduler = GetScheduler();

  auto command = cmd::Wait(100_ms);

  scheduler.Schedule(command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(command));
  frc::sim::StepTiming(110_ms);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));

  frc::sim::ResumeTiming();
}
