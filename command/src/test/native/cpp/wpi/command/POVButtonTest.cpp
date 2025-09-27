// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/driverstation/DriverStation.hpp>
#include <wpi/driverstation/Joystick.hpp>
#include <wpi/simulation/JoystickSim.hpp>

#include "CommandTestBase.h"
#include "wpi/command/CommandScheduler.hpp"
#include "wpi/command/RunCommand.hpp"
#include "wpi/command/WaitUntilCommand.hpp"
#include "wpi/command/button/POVButton.hpp"

using namespace wpi::cmd;
class POVButtonTest : public CommandTestBase {};

TEST_F(POVButtonTest, SetPOV) {
  wpi::sim::JoystickSim joysim(1);
  joysim.SetPOV(wpi::DriverStation::kUp);
  joysim.NotifyNewData();

  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;

  WaitUntilCommand command([&finished] { return finished; });

  wpi::Joystick joy(1);
  POVButton(&joy, wpi::DriverStation::kRight).OnTrue(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  joysim.SetPOV(wpi::DriverStation::kRight);
  joysim.NotifyNewData();

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}
