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

TEST_F(CommandScheduleTest, InstantSchedule) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  EXPECT_CALL(command, Initialize());
  EXPECT_CALL(command, Execute());
  EXPECT_CALL(command, End(false));

  command.SetFinished(true);
  scheduler.Schedule(&command);
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(CommandScheduleTest, SingleIterationSchedule) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  EXPECT_CALL(command, Initialize());
  EXPECT_CALL(command, Execute()).Times(2);
  EXPECT_CALL(command, End(false));

  scheduler.Schedule(&command);
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  scheduler.Run();
  command.SetFinished(true);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(CommandScheduleTest, MultiSchedule) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command1;
  MockCommand command2;
  MockCommand command3;

  EXPECT_CALL(command1, Initialize());
  EXPECT_CALL(command1, Execute()).Times(2);
  EXPECT_CALL(command1, End(false));

  EXPECT_CALL(command2, Initialize());
  EXPECT_CALL(command2, Execute()).Times(3);
  EXPECT_CALL(command2, End(false));

  EXPECT_CALL(command3, Initialize());
  EXPECT_CALL(command3, Execute()).Times(4);
  EXPECT_CALL(command3, End(false));

  scheduler.Schedule(&command1);
  scheduler.Schedule(&command2);
  scheduler.Schedule(&command3);
  EXPECT_TRUE(scheduler.IsScheduled({&command1, &command2, &command3}));
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled({&command1, &command2, &command3}));
  command1.SetFinished(true);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled({&command2, &command3}));
  EXPECT_FALSE(scheduler.IsScheduled(&command1));
  command2.SetFinished(true);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command3));
  EXPECT_FALSE(scheduler.IsScheduled({&command1, &command2}));
  command3.SetFinished(true);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled({&command1, &command2, &command3}));
}

TEST_F(CommandScheduleTest, SchedulerCancel) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  EXPECT_CALL(command, Initialize());
  EXPECT_CALL(command, Execute());
  EXPECT_CALL(command, End(false)).Times(0);
  EXPECT_CALL(command, End(true));

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  scheduler.Cancel(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(CommandScheduleTest, CommandKnowsWhenItEnded) {
  CommandScheduler scheduler = GetScheduler();

  wpi::cmd::FunctionalCommand* commandPtr = nullptr;
  auto command = wpi::cmd::FunctionalCommand(
      [] {}, [] {},
      [&](auto isForced) {
        EXPECT_FALSE(scheduler.IsScheduled(commandPtr))
            << "Command shouldn't be scheduled when its end is called";
      },
      [] { return true; });
  commandPtr = &command;

  scheduler.Schedule(commandPtr);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(commandPtr))
      << "Command should be removed from scheduler when its isFinished() "
         "returns true";
}

TEST_F(CommandScheduleTest, ScheduleCommandInCommand) {
  CommandScheduler scheduler = GetScheduler();
  int counter = 0;
  wpi::cmd::InstantCommand commandToGetScheduled{[&counter] { counter++; }};

  auto command =
      wpi::cmd::RunCommand([&counter, &scheduler, &commandToGetScheduled] {
        scheduler.Schedule(&commandToGetScheduled);
        EXPECT_EQ(counter, 1)
            << "Scheduled command's init was not run immediately "
               "after getting scheduled";
      });

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_EQ(counter, 1) << "Command 2 was not run when it should have been";
  EXPECT_TRUE(scheduler.IsScheduled(&commandToGetScheduled))
      << "Command 2 was not added to scheduler";

  scheduler.Run();
  EXPECT_EQ(counter, 1) << "Command 2 was run when it shouldn't have been";
  EXPECT_FALSE(scheduler.IsScheduled(&commandToGetScheduled))
      << "Command 2 did not end when it should have";
}

TEST_F(CommandScheduleTest, NotScheduledCancel) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  EXPECT_NO_FATAL_FAILURE(scheduler.Cancel(&command));
}

TEST_F(CommandScheduleTest, SmartDashboardCancel) {
  CommandScheduler scheduler = GetScheduler();
  wpi::SmartDashboard::PutData("Scheduler", &scheduler);
  wpi::SmartDashboard::UpdateValues();

  MockCommand command;
  scheduler.Schedule(&command);
  scheduler.Run();
  wpi::SmartDashboard::UpdateValues();
  EXPECT_TRUE(scheduler.IsScheduled(&command));

  uintptr_t ptrTmp = reinterpret_cast<uintptr_t>(&command);
  wpi::nt::NetworkTableInstance::GetDefault()
      .GetEntry("/SmartDashboard/Scheduler/Cancel")
      .SetIntegerArray(
          std::span<const int64_t>{{static_cast<int64_t>(ptrTmp)}});
  wpi::SmartDashboard::UpdateValues();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
