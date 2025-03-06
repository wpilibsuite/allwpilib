// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/DriverStation.h>
#include <frc/simulation/DriverStationSim.h>

#include "../CommandTestBase.h"
#include "frc2/command/button/RobotModeTriggers.h"
#include "frc2/command/button/Trigger.h"

using namespace frc;
using namespace frc2;
using namespace frc::sim;
class RobotModeTriggersTest : public CommandTestBase {};

TEST(RobotModeTriggersTest, OpModeId) {
  DriverStationSim::ResetData();
  int mode = DriverStation::AddOpModeOption("test", "", "", 0);
  DriverStationSim::SetOpMode("test");
  DriverStationSim::NotifyNewData();
  Trigger trigger = RobotModeTriggers::OpMode(mode);
  EXPECT_TRUE(trigger.Get());
}

TEST(RobotModeTriggersTest, OpMode) {
  DriverStationSim::ResetData();
  DriverStation::AddOpModeOption("test", "", "", 0);
  DriverStationSim::SetOpMode("test");
  DriverStationSim::NotifyNewData();
  Trigger trigger = RobotModeTriggers::OpMode("test");
  EXPECT_TRUE(trigger.Get());
}

TEST(RobotModeTriggersTest, Disabled) {
  DriverStationSim::ResetData();
  DriverStation::AddOpModeOption("test", "", "", 0);
  DriverStationSim::NotifyNewData();
  Trigger disabled = RobotModeTriggers::Disabled();
  EXPECT_TRUE(disabled.Get());
}
