/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <regex>

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ScheduleCommand.h"
#include "frc2/command/SequentialCommandGroup.h"

using namespace frc2;
class ScheduleCommandTest : public CommandTestBase {};

TEST_F(ScheduleCommandTest, ScheduleCommandScheduleTest) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool scheduled = false;

  InstantCommand toSchedule([&scheduled] { scheduled = true; }, {});

  ScheduleCommand command(&toSchedule);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduled);
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
