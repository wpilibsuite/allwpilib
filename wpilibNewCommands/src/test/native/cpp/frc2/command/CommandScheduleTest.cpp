// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc2/command/RunCommand.h>

#include <memory>

#include <frc/smartdashboard/SmartDashboard.h>
#include <networktables/NetworkTableInstance.h>

#include "CommandTestBase.h"

using namespace frc2;
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

TEST_F(CommandScheduleTest, NotScheduledCancel) {
  CommandScheduler scheduler = GetScheduler();
  MockCommand command;

  EXPECT_NO_FATAL_FAILURE(scheduler.Cancel(&command));
}

TEST_F(CommandScheduleTest, CancelNextCommand) {
  CommandScheduler scheduler = GetScheduler();

  frc2::RunCommand* command1PtrPtr = nullptr;
  frc2::RunCommand* command2PtrPtr = nullptr;
  auto counterPtr = std::make_shared<int>(0);

  auto command1 = frc2::RunCommand([counterPtr, &command2PtrPtr, &scheduler] {
    scheduler.Cancel(command2PtrPtr);
    (*counterPtr)++;
  });
  auto command2 = frc2::RunCommand([counterPtr, &command1PtrPtr, &scheduler] {
    scheduler.Cancel(command1PtrPtr);
    (*counterPtr)++;
  });

  command1PtrPtr = &command1;
  command2PtrPtr = &command2;

  scheduler.Schedule(&command1);
  scheduler.Schedule(&command2);
  scheduler.Run();

  EXPECT_EQ(*counterPtr, 1)
      << "Second command was run when it shouldn't have been";

  // only one of the commands should be canceled.
  EXPECT_FALSE(scheduler.IsScheduled(&command1) &&
               scheduler.IsScheduled(&command2))
      << "Both commands are running when only one should be";
  // one of the commands shouldn't be canceled because the other one is canceled
  // first
  EXPECT_TRUE(scheduler.IsScheduled(&command1) ||
              scheduler.IsScheduled(&command2))
      << "Both commands are canceled when only one should be";

  scheduler.Run();
  EXPECT_EQ(*counterPtr, 2);
}

TEST_F(CommandScheduleTest, CommandKnowsWhenItEnded) {
  CommandScheduler scheduler = GetScheduler();

  frc2::FunctionalCommand* commandPtr = nullptr;
  auto command = frc2::FunctionalCommand(
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

TEST_F(CommandScheduleTest, SmartDashboardCancel) {
  CommandScheduler scheduler = GetScheduler();
  frc::SmartDashboard::PutData("Scheduler", &scheduler);
  frc::SmartDashboard::UpdateValues();

  MockCommand command;
  scheduler.Schedule(&command);
  scheduler.Run();
  frc::SmartDashboard::UpdateValues();
  EXPECT_TRUE(scheduler.IsScheduled(&command));

  uintptr_t ptrTmp = reinterpret_cast<uintptr_t>(&command);
  nt::NetworkTableInstance::GetDefault()
      .GetEntry("/SmartDashboard/Scheduler/Cancel")
      .SetIntegerArray(
          std::span<const int64_t>{{static_cast<int64_t>(ptrTmp)}});
  frc::SmartDashboard::UpdateValues();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
