// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/RobotModeTriggers.hpp"

#include "../CommandTestBase.hpp"
#include "wpi/commands2/button/Trigger.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/simulation/DriverStationSim.hpp"

using namespace wpi::cmd;
using namespace wpi::sim;
class RobotModeTriggersTest : public CommandTestBase {};

TEST_CASE("RobotModeTriggersTest Autonomous", "[commandsv2][command]") {
  DriverStationSim::ResetData();
  DriverStationSim::SetRobotMode(wpi::hal::RobotMode::AUTONOMOUS);
  DriverStationSim::SetEnabled(true);
  DriverStationSim::NotifyNewData();
  Trigger autonomous = RobotModeTriggers::Autonomous();
  CHECK(autonomous.Get());
}

TEST_CASE("RobotModeTriggersTest Teleop", "[commandsv2][command]") {
  DriverStationSim::ResetData();
  DriverStationSim::SetRobotMode(wpi::hal::RobotMode::TELEOPERATED);
  DriverStationSim::SetEnabled(true);
  DriverStationSim::NotifyNewData();
  Trigger teleop = RobotModeTriggers::Teleop();
  CHECK(teleop.Get());
}

TEST_CASE("RobotModeTriggersTest Disabled", "[commandsv2][command]") {
  DriverStationSim::ResetData();
  DriverStationSim::SetEnabled(false);
  DriverStationSim::NotifyNewData();
  Trigger disabled = RobotModeTriggers::Disabled();
  CHECK(disabled.Get());
}

TEST_CASE("RobotModeTriggersTest UtilityMode", "[commandsv2][command]") {
  DriverStationSim::ResetData();
  DriverStationSim::SetRobotMode(wpi::hal::RobotMode::UTILITY);
  DriverStationSim::SetEnabled(true);
  DriverStationSim::NotifyNewData();
  Trigger test = RobotModeTriggers::Utility();
  CHECK(test.Get());
}
