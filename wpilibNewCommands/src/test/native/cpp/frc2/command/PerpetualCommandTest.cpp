// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/PerpetualCommand.h"

using namespace frc2;
class PerpetualCommandTest : public CommandTestBase {};

TEST_F(PerpetualCommandTest, PerpetualCommandSchedule) {
  CommandScheduler scheduler = GetScheduler();

  bool check = false;

  WPI_IGNORE_DEPRECATED
  PerpetualCommand command{InstantCommand([&check] { check = true; }, {})};
  WPI_UNIGNORE_DEPRECATED

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  EXPECT_TRUE(check);
}
