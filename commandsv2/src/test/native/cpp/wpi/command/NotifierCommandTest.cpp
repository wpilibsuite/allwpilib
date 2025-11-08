// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/NotifierCommand.hpp"
#include "wpi/simulation/SimHooks.hpp"

using namespace wpi::cmd;
using namespace std::chrono_literals;

class NotifierCommandTest : public CommandTestBase {};

TEST_F(NotifierCommandTest, NotifierCommandSchedule) {
  CommandScheduler scheduler = GetScheduler();

  wpi::sim::PauseTiming();

  int counter = 0;
  NotifierCommand command([&] { counter++; }, 10_ms, {});

  scheduler.Schedule(&command);
  for (int i = 0; i < 5; ++i) {
    wpi::sim::StepTiming(5_ms);
  }
  scheduler.Cancel(&command);

  wpi::sim::ResumeTiming();

  EXPECT_EQ(counter, 2);
}
