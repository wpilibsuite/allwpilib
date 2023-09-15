// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc/command2/RunCommand.h"

class DefaultCommandTest : public CommandTestBase {};

TEST_F(DefaultCommandTest, DefaultCommandSchedule) {
  frc::CommandScheduler scheduler = GetScheduler();

  TestSubsystem subsystem;

  frc::RunCommand command1([] {}, {&subsystem});

  scheduler.SetDefaultCommand(&subsystem, std::move(command1));
  auto handle = scheduler.GetDefaultCommand(&subsystem);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(handle));
}

TEST_F(DefaultCommandTest, DefaultCommandInterruptResume) {
  frc::CommandScheduler scheduler = GetScheduler();

  TestSubsystem subsystem;

  frc::RunCommand command1([] {}, {&subsystem});
  frc::RunCommand command2([] {}, {&subsystem});

  scheduler.SetDefaultCommand(&subsystem, std::move(command1));
  auto handle = scheduler.GetDefaultCommand(&subsystem);
  scheduler.Run();
  scheduler.Schedule(&command2);

  EXPECT_TRUE(scheduler.IsScheduled(&command2));
  EXPECT_FALSE(scheduler.IsScheduled(handle));

  scheduler.Cancel(&command2);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(handle));
}
