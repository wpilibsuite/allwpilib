// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <regex>

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ScheduleCommand.h"
#include "frc2/command/SequentialCommandGroup.h"

using namespace frc2;
class ScheduleCommandTest : public CommandTestBase {};

TEST_F(ScheduleCommandTest, ScheduleCommandSchedule) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool scheduled = false;

  InstantCommand toSchedule([&scheduled] { scheduled = true; }, {});

  ScheduleCommand command(&toSchedule);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduled);
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
