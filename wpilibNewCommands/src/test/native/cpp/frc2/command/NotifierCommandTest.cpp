/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/simulation/SimHooks.h>

#include "CommandTestBase.h"
#include "frc2/command/NotifierCommand.h"

using namespace frc2;
using namespace std::chrono_literals;

class NotifierCommandTest : public CommandTestBase {};

TEST_F(NotifierCommandTest, NotifierCommandScheduleTest) {
  CommandScheduler scheduler = GetScheduler();

  frc::sim::PauseTiming();

  int counter = 0;
  NotifierCommand command([&] { counter++; }, 0.01_s, {});

  scheduler.Schedule(&command);
  for (int i = 0; i < 5; ++i) {
    frc::sim::StepTiming(0.005_s);
  }
  scheduler.Cancel(&command);

  frc::sim::ResumeTiming();

  EXPECT_EQ(counter, 2);
}
