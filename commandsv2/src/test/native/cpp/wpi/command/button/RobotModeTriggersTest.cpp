// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/simulation/DriverStationSim.hpp"

#include "../CommandTestBase.hpp"
#include "wpi/commands2/button/RobotModeTriggers.hpp"
#include "wpi/commands2/button/Trigger.hpp"

using namespace frc2;
using namespace frc::sim;
class RobotModeTriggersTest : public CommandTestBase {};

TEST(RobotModeTriggersTest, Autonomous) {
  DriverStationSim::ResetData();
  DriverStationSim::SetAutonomous(true);
  DriverStationSim::SetTest(false);
  DriverStationSim::SetEnabled(true);
  DriverStationSim::NotifyNewData();
  Trigger autonomous = RobotModeTriggers::Autonomous();
  EXPECT_TRUE(autonomous.Get());
}

TEST(RobotModeTriggersTest, Teleop) {
  DriverStationSim::ResetData();
  DriverStationSim::SetAutonomous(false);
  DriverStationSim::SetTest(false);
  DriverStationSim::SetEnabled(true);
  DriverStationSim::NotifyNewData();
  Trigger teleop = RobotModeTriggers::Teleop();
  EXPECT_TRUE(teleop.Get());
}

TEST(RobotModeTriggersTest, Disabled) {
  DriverStationSim::ResetData();
  DriverStationSim::SetAutonomous(false);
  DriverStationSim::SetTest(false);
  DriverStationSim::SetEnabled(false);
  DriverStationSim::NotifyNewData();
  Trigger disabled = RobotModeTriggers::Disabled();
  EXPECT_TRUE(disabled.Get());
}

TEST(RobotModeTriggersTest, TestMode) {
  DriverStationSim::ResetData();
  DriverStationSim::SetAutonomous(false);
  DriverStationSim::SetTest(true);
  DriverStationSim::SetEnabled(true);
  DriverStationSim::NotifyNewData();
  Trigger test = RobotModeTriggers::Test();
  EXPECT_TRUE(test.Get());
}
