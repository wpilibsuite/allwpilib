// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/WaitCommand.hpp"
#include "wpi/commands2/WaitUntilCommand.hpp"
#include "wpi/simulation/SimHooks.hpp"

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
