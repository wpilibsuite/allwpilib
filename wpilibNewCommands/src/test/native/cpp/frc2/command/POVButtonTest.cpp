// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/DriverStation.h>
#include <frc/Joystick.h>
#include <frc/simulation/JoystickSim.h>
#include <gtest/gtest.h>

#include "CommandTestBase.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/WaitUntilCommand.h"
#include "frc2/command/button/POVButton.h"

using namespace frc2;
class POVButtonTest : public CommandTestBase {};

TEST_F(POVButtonTest, SetPOV) {
  frc::sim::JoystickSim joysim(1);
  joysim.SetPOV(frc::DriverStation::kUp);
  joysim.NotifyNewData();

  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;

  WaitUntilCommand command([&finished] { return finished; });

  frc::Joystick joy(1);
  POVButton(&joy, frc::DriverStation::kRight).OnTrue(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  joysim.SetPOV(frc::DriverStation::kRight);
  joysim.NotifyNewData();

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
