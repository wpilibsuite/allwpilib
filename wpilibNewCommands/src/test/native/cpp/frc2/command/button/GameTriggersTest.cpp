// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../CommandTestBase.h"
#include "frc/simulation/DriverStationSim.h"
#include "frc/DriverStation.h"
#include "frc2/command/button/Trigger.h"
#include "frc2/command/button/GameTriggers.h"

using namespace frc2;
using namespace frc::sim;
class GameTriggersTest : public CommandTestBase {};

TEST(GameTriggersTest, Autonomous) {
    DriverStationSim::ResetData();
    DriverStationSim::SetAutonomous(true);
    DriverStationSim::SetTest(false);
    DriverStationSim::SetEnabled(true);
    frc::DriverStation::RefreshData();
    Trigger autonomous = GameTriggers::Autonomous();
    EXPECT_TRUE(autonomous.Get());
}

TEST(GameTriggersTest, Teleop) {
    DriverStationSim::ResetData();
    DriverStationSim::SetAutonomous(false);
    DriverStationSim::SetTest(false);
    DriverStationSim::SetEnabled(true);
    frc::DriverStation::RefreshData();
    Trigger teleop = GameTriggers::Teleop();
    EXPECT_TRUE(teleop.Get());
}

TEST(GameTriggersTest, Disabled) {
    DriverStationSim::ResetData();
    DriverStationSim::SetAutonomous(false);
    DriverStationSim::SetTest(false);
    DriverStationSim::SetEnabled(false);
    frc::DriverStation::RefreshData();
    Trigger disabled = GameTriggers::Disabled();
    EXPECT_TRUE(disabled.Get());
}

TEST(GameTriggersTest, TestMode) {
    DriverStationSim::ResetData();
    DriverStationSim::SetAutonomous(false);
    DriverStationSim::SetTest(true);
    DriverStationSim::SetEnabled(true);
    frc::DriverStation::RefreshData();
    Trigger test = GameTriggers::Test();
    EXPECT_TRUE(test.Get());
}