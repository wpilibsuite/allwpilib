// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include "CommandTestBase.h"
#include "frc2/command/Commands.h"
#include "frc2/command/CommandPtr.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ProxyCommand.h"
#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;
class ProxyCommandTest : public CommandTestBase {};

TEST_F(ProxyCommandTest, NonOwningCommandSchedule) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool scheduled = false;

  InstantCommand toSchedule([&scheduled] { scheduled = true; }, {});

  ProxyCommand command(&toSchedule);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduled);
}

TEST_F(ProxyCommandTest, NonOwningCommandEnd) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool finished = false;

  WaitUntilCommand toSchedule([&finished] { return finished; });

  ProxyCommand command(&toSchedule);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(ProxyCommandTest, OwningCommandSchedule) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool scheduled = false;

  CommandPtr command =
      InstantCommand([&scheduled] { scheduled = true; }).AsProxy();

  scheduler.Schedule(command);
  scheduler.Run();

  EXPECT_TRUE(scheduled);
}

TEST_F(ProxyCommandTest, OwningCommandEnd) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool finished = false;

  CommandPtr command =
      WaitUntilCommand([&finished] { return finished; }).AsProxy();

  scheduler.Schedule(command);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(command));
  finished = true;
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));
}

TEST_F(ProxyCommandTest, DeferredOwningCommandSchedule) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool allocated = false;
  bool scheduled = false;

  CommandPtr proxy = cmd::Deferred([&scheduled, &allocated] { 
    allocated = true;
    return cmd::RunOnce([&scheduled] { scheduled = true; });
  });
  EXPECT_FALSE(allocated);

  scheduler.Schedule(proxy);
  EXPECT_TRUE(allocated);
  EXPECT_TRUE(scheduled);
}

TEST_F(ProxyCommandTest, DeferredOwningCommandEnd) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool allocated = false;
  bool finished = false;

  CommandPtr command = cmd::Deferred([&finished, &allocated] { 
    allocated = true;
    return cmd::WaitUntil([&finished] { return finished; });
  });

  EXPECT_FALSE(allocated);
  scheduler.Schedule(command);
  EXPECT_TRUE(allocated);
  scheduler.Run();

  EXPECT_TRUE(scheduler.IsScheduled(command));
  finished = true;
  scheduler.Run();
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(command));
}
