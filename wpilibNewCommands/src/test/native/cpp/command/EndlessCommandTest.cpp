// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc/command/EndlessCommand.h"
#include "frc/command/InstantCommand.h"

using namespace frc;
class EndlessCommandTest : public CommandTestBase {};

TEST_F(EndlessCommandTest, EndlessCommandSchedule) {
  CommandScheduler scheduler = GetScheduler();

  bool check = false;

  EndlessCommand command{InstantCommand([&check] { check = true; }, {})};

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  EXPECT_TRUE(check);
}
