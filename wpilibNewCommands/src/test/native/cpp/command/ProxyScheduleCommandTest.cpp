// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include "CommandTestBase.h"
#include "frc/command/InstantCommand.h"
#include "frc/command/ProxyScheduleCommand.h"
#include "frc/command/WaitUntilCommand.h"

using namespace frc;
class ProxyScheduleCommandTest : public CommandTestBase {};

TEST_F(ProxyScheduleCommandTest, NonOwningCommandSchedule) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool scheduled = false;

  InstantCommand toSchedule([&scheduled] { scheduled = true; }, {});

  ProxyScheduleCommand command(&toSchedule);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduled);
}

TEST_F(ProxyScheduleCommandTest, NonOwningCommandEnd) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool finished = false;

  WaitUntilCommand toSchedule([&finished] { return finished; });

  ProxyScheduleCommand command(&toSchedule);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(ProxyScheduleCommandTest, OwningCommandSchedule) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool scheduled = false;

  ProxyScheduleCommand command(
      std::make_unique<InstantCommand>([&scheduled] { scheduled = true; }));

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduled);
}

TEST_F(ProxyScheduleCommandTest, OwningCommandEnd) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool finished = false;

  ProxyScheduleCommand command(
      std::make_unique<WaitUntilCommand>([&finished] { return finished; }));

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
