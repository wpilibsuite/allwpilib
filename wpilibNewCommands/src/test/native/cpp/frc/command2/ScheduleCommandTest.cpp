// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <regex>

#include "CommandTestBase.h"
#include "frc/command2/InstantCommand.h"
#include "frc/command2/ScheduleCommand.h"
#include "frc/command2/SequentialCommandGroup.h"

class ScheduleCommandTest : public CommandTestBase {};

TEST_F(ScheduleCommandTest, ScheduleCommandSchedule) {
  frc::CommandScheduler& scheduler = frc::CommandScheduler::GetInstance();

  bool scheduled = false;

  frc::InstantCommand toSchedule([&scheduled] { scheduled = true; }, {});

  frc::ScheduleCommand command(&toSchedule);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduled);
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
