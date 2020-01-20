/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/Joystick.h>
#include <mockdata/DriverStationData.h>

#include "CommandTestBase.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/WaitUntilCommand.h"
#include "frc2/command/button/POVButton.h"
#include "gtest/gtest.h"

using namespace frc2;
class POVButtonTest : public CommandTestBase {};

TEST_F(POVButtonTest, SetPOVTest) {
  HAL_JoystickPOVs povs;
  povs.count = 1;
  povs.povs[0] = 0;
  HALSIM_SetJoystickPOVs(1, &povs);
  HALSIM_NotifyDriverStationNewData();

  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;

  WaitUntilCommand command([&finished] { return finished; });

  frc::Joystick joy(1);
  POVButton(&joy, 90).WhenPressed(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  povs.povs[0] = 90;
  HALSIM_SetJoystickPOVs(1, &povs);
  HALSIM_NotifyDriverStationNewData();

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
