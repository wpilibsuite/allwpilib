// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include "CommandTestBase.h"
#include "frc/command2/CommandPtr.h"
#include "frc/command2/InstantCommand.h"
#include "frc/command2/ProxyCommand.h"
#include "frc/command2/WaitUntilCommand.h"

class ProxyCommandTest : public CommandTestBase {};

TEST_F(ProxyCommandTest, NonOwningCommandSchedule) {
  frc::CommandScheduler& scheduler = frc::CommandScheduler::GetInstance();

  bool scheduled = false;

  frc::InstantCommand toSchedule([&scheduled] { scheduled = true; }, {});

  frc::ProxyCommand command(&toSchedule);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduled);
}

TEST_F(ProxyCommandTest, NonOwningCommandEnd) {
  frc::CommandScheduler& scheduler = frc::CommandScheduler::GetInstance();

  bool finished = false;

  frc::WaitUntilCommand toSchedule([&finished] { return finished; });

  frc::ProxyCommand command(&toSchedule);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(ProxyCommandTest, OwningCommandSchedule) {
  frc::CommandScheduler& scheduler = frc::CommandScheduler::GetInstance();

  bool scheduled = false;

  frc::CommandPtr command =
      frc::InstantCommand([&scheduled] { scheduled = true; }).AsProxy();

  scheduler.Schedule(command);
  scheduler.Run();

  EXPECT_TRUE(scheduled);
}

TEST_F(ProxyCommandTest, OwningCommandEnd) {
  frc::CommandScheduler& scheduler = frc::CommandScheduler::GetInstance();

  bool finished = false;

  frc::CommandPtr command =
      frc::WaitUntilCommand([&finished] { return finished; }).AsProxy();

  scheduler.Schedule(command);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(command));
  finished = true;
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));
}
