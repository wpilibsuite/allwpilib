/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <regex>

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/ProxyScheduleCommand.h"
#include "frc2/command/WaitUntilCommand.h"

using namespace frc2;
class ProxyScheduleCommandTest : public CommandTestBase {};

TEST_F(ProxyScheduleCommandTest, ProxyScheduleCommandScheduleTest) {
  CommandScheduler& scheduler = CommandScheduler::GetInstance();

  bool scheduled = false;

  InstantCommand toSchedule([&scheduled] { scheduled = true; }, {});

  ProxyScheduleCommand command(&toSchedule);

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_TRUE(scheduled);
}

TEST_F(ProxyScheduleCommandTest, ProxyScheduleCommandEndTest) {
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
