// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include "CommandTestBase.h"
#include "frc2/command/Commands.h"
#include "frc2/command/RunCommand.h"

using namespace frc2;
class DefaultCommandTest : public CommandTestBase {};

TEST_F(DefaultCommandTest, DefaultCommandSchedule) {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem subsystem;

  auto command = cmd::Idle({&subsystem});

  scheduler.SetDefaultCommand(&subsystem, std::move(command));
  auto handle = scheduler.GetDefaultCommand(&subsystem);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(handle));
}

TEST_F(DefaultCommandTest, DefaultCommandInterruptResume) {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem subsystem;

  auto command1 = cmd::Idle({&subsystem});
  auto command2 = cmd::Idle({&subsystem});

  scheduler.SetDefaultCommand(&subsystem, std::move(command1));
  auto handle = scheduler.GetDefaultCommand(&subsystem);
  scheduler.Run();
  scheduler.Schedule(command2);

  EXPECT_TRUE(scheduler.IsScheduled(command2));
  EXPECT_FALSE(scheduler.IsScheduled(handle));

  scheduler.Cancel(command2);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(handle));
}
