// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/RunCommand.hpp"

using namespace wpi::cmd;
class DefaultCommandTest : public CommandTestBase {};

TEST_CASE_METHOD(DefaultCommandTest,
                 "DefaultCommandTest DefaultCommandSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem subsystem;

  auto command = Idle({&subsystem});

  scheduler.SetDefaultCommand(&subsystem, std::move(command));
  auto handle = scheduler.GetDefaultCommand(&subsystem);
  scheduler.Run();

  CHECK(scheduler.IsScheduled(handle));
}

TEST_CASE_METHOD(DefaultCommandTest,
                 "DefaultCommandTest DefaultCommandInterruptResume",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem subsystem;

  auto command1 = Idle({&subsystem});
  auto command2 = Idle({&subsystem});

  scheduler.SetDefaultCommand(&subsystem, std::move(command1));
  auto handle = scheduler.GetDefaultCommand(&subsystem);
  scheduler.Run();
  scheduler.Schedule(command2);

  CHECK(scheduler.IsScheduled(command2));
  CHECK_FALSE(scheduler.IsScheduled(handle));

  scheduler.Cancel(command2);
  scheduler.Run();

  CHECK(scheduler.IsScheduled(handle));
}
