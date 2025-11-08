// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "CommandTestBase.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/RunCommand.hpp"
#include "wpi/commands2/WaitUntilCommand.hpp"
#include "wpi/commands2/button/POVButton.hpp"
#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/simulation/JoystickSim.hpp"

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
