/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/PerpetualCommand.h"

using namespace frc2;
class PerpetualCommandTest : public CommandTestBase {};

TEST_F(PerpetualCommandTest, PerpetualCommandScheduleTest) {
  CommandScheduler scheduler = GetScheduler();

  bool check = false;

  PerpetualCommand command{InstantCommand([&check] { check = true; }, {})};

  scheduler.Schedule(&command);
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  EXPECT_TRUE(check);
}
