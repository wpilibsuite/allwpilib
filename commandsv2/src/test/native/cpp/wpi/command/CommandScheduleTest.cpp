// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"
#include "wpi/commands2/InstantCommand.hpp"
#include "wpi/commands2/RunCommand.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

using namespace wpi::cmd;
class CommandScheduleTest : public CommandTestBase {};

TEST_CASE_METHOD(CommandScheduleTest, "CommandScheduleTest InstantSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  command.ExpectInitialize(1);
  command.ExpectExecute(1);
  command.ExpectEnd(false, 1);

  command.SetFinished(true);
  scheduler.Schedule(&command);
  CHECK(scheduler.IsScheduled(&command));
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
}

TEST_CASE_METHOD(CommandScheduleTest,
                 "CommandScheduleTest SingleIterationSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  command.ExpectInitialize(1);
  command.ExpectExecute(2);
  command.ExpectEnd(false, 1);

  scheduler.Schedule(&command);
  CHECK(scheduler.IsScheduled(&command));
  scheduler.Run();
  command.SetFinished(true);
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
}

TEST_CASE_METHOD(CommandScheduleTest, "CommandScheduleTest MultiSchedule",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command1;
  MockCommand command2;
  MockCommand command3;

  command1.ExpectInitialize(1);
  command1.ExpectExecute(2);
  command1.ExpectEnd(false, 1);

  command2.ExpectInitialize(1);
  command2.ExpectExecute(3);
  command2.ExpectEnd(false, 1);

  command3.ExpectInitialize(1);
  command3.ExpectExecute(4);
  command3.ExpectEnd(false, 1);

  scheduler.Schedule(&command1);
  scheduler.Schedule(&command2);
  scheduler.Schedule(&command3);
  CHECK(scheduler.IsScheduled({&command1, &command2, &command3}));
  scheduler.Run();
  CHECK(scheduler.IsScheduled({&command1, &command2, &command3}));
  command1.SetFinished(true);
  scheduler.Run();
  CHECK(scheduler.IsScheduled({&command2, &command3}));
  CHECK_FALSE(scheduler.IsScheduled(&command1));
  command2.SetFinished(true);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(&command3));
  CHECK_FALSE(scheduler.IsScheduled({&command1, &command2}));
  command3.SetFinished(true);
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled({&command1, &command2, &command3}));
}

TEST_CASE_METHOD(CommandScheduleTest, "CommandScheduleTest SchedulerCancel",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  command.ExpectInitialize(1);
  command.ExpectExecute(1);
  command.ExpectEnd(false, 0);
  command.ExpectEnd(true, 1);

  scheduler.Schedule(&command);
  scheduler.Run();
  CHECK(scheduler.IsScheduled(&command));
  scheduler.Cancel(&command);
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
}

TEST_CASE_METHOD(CommandScheduleTest,
                 "CommandScheduleTest CommandKnowsWhenItEnded",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  wpi::cmd::FunctionalCommand* commandPtr = nullptr;
  auto command = wpi::cmd::FunctionalCommand(
      [] {}, [] {},
      [&](auto isForced) { CHECK_FALSE(scheduler.IsScheduled(commandPtr)); },
      [] { return true; });
  commandPtr = &command;

  scheduler.Schedule(commandPtr);
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(commandPtr));
}

TEST_CASE_METHOD(CommandScheduleTest,
                 "CommandScheduleTest ScheduleCommandInCommand",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  wpi::cmd::InstantCommand commandToGetScheduled{[&counter] { counter++; }};

  auto command =
      wpi::cmd::RunCommand([&counter, &scheduler, &commandToGetScheduled] {
        scheduler.Schedule(&commandToGetScheduled);
        CHECK(counter == 1);
      });

  scheduler.Schedule(&command);
  scheduler.Run();
  CHECK(counter == 1);
  CHECK(scheduler.IsScheduled(&commandToGetScheduled));

  scheduler.Run();
  CHECK(counter == 1);
  CHECK_FALSE(scheduler.IsScheduled(&commandToGetScheduled));
}

TEST_CASE_METHOD(CommandScheduleTest, "CommandScheduleTest NotScheduledCancel",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  CHECK_NOTHROW(scheduler.Cancel(&command));
}

TEST_CASE_METHOD(CommandScheduleTest,
                 "CommandScheduleTest SmartDashboardCancel",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();
  wpi::SmartDashboard::PutData("Scheduler", &scheduler);
  wpi::SmartDashboard::UpdateValues();

  MockCommand command;
  scheduler.Schedule(&command);
  scheduler.Run();
  wpi::SmartDashboard::UpdateValues();
  CHECK(scheduler.IsScheduled(&command));

  uintptr_t ptrTmp = reinterpret_cast<uintptr_t>(&command);
  wpi::nt::NetworkTableInstance::GetDefault()
      .GetEntry("/SmartDashboard/Scheduler/Cancel")
      .SetIntegerArray(
          std::span<const int64_t>{{static_cast<int64_t>(ptrTmp)}});
  wpi::SmartDashboard::UpdateValues();
  scheduler.Run();
  CHECK_FALSE(scheduler.IsScheduled(&command));
}
