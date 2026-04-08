// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/DriverStationSim.hpp"

#include <gtest/gtest.h>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/HAL.h"

using namespace wpi;
using namespace wpi::sim;

TEST(DriverStationTest, Enabled) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  EXPECT_FALSE(DriverStation::IsEnabled());
  BooleanCallback callback;
  auto cb =
      DriverStationSim::RegisterEnabledCallback(callback.GetCallback(), false);
  DriverStationSim::SetRobotMode(HAL_ROBOT_MODE_TELEOPERATED);
  DriverStationSim::SetEnabled(true);
  DriverStationSim::NotifyNewData();
  EXPECT_TRUE(DriverStationSim::GetEnabled());
  EXPECT_TRUE(DriverStation::IsEnabled());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(DriverStationTest, AutonomousMode) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  EXPECT_FALSE(DriverStation::IsAutonomous());
  EnumCallback callback;
  auto cb = DriverStationSim::RegisterRobotModeCallback(callback.GetCallback(),
                                                        false);
  DriverStationSim::SetRobotMode(HAL_ROBOT_MODE_AUTONOMOUS);
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(DriverStationSim::GetRobotMode(), HAL_ROBOT_MODE_AUTONOMOUS);
  EXPECT_TRUE(DriverStation::IsAutonomous());
  EXPECT_EQ(DriverStation::GetRobotMode(), RobotMode::AUTONOMOUS);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(callback.GetLastValue(), HAL_ROBOT_MODE_AUTONOMOUS);
}

TEST(DriverStationTest, Mode) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  EXPECT_FALSE(DriverStation::IsTest());
  EnumCallback callback;
  auto cb = DriverStationSim::RegisterRobotModeCallback(callback.GetCallback(),
                                                        false);
  DriverStationSim::SetRobotMode(HAL_ROBOT_MODE_TEST);
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(DriverStationSim::GetRobotMode(), HAL_ROBOT_MODE_TEST);
  EXPECT_TRUE(DriverStation::IsTest());
  EXPECT_EQ(DriverStation::GetRobotMode(), RobotMode::TEST);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(callback.GetLastValue(), HAL_ROBOT_MODE_TEST);
}

TEST(DriverStationTest, Estop) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  EXPECT_FALSE(DriverStation::IsEStopped());
  BooleanCallback callback;
  auto cb =
      DriverStationSim::RegisterEStopCallback(callback.GetCallback(), false);
  DriverStationSim::SetEStop(true);
  DriverStationSim::NotifyNewData();
  EXPECT_TRUE(DriverStationSim::GetEStop());
  EXPECT_TRUE(DriverStation::IsEStopped());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(DriverStationTest, FmsAttached) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  EXPECT_FALSE(DriverStation::IsFMSAttached());
  BooleanCallback callback;
  auto cb = DriverStationSim::RegisterFmsAttachedCallback(
      callback.GetCallback(), false);
  DriverStationSim::SetFmsAttached(true);
  DriverStationSim::NotifyNewData();
  EXPECT_TRUE(DriverStationSim::GetFmsAttached());
  EXPECT_TRUE(DriverStation::IsFMSAttached());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(DriverStationTest, DsAttached) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();
  DriverStation::RefreshData();

  EXPECT_FALSE(DriverStationSim::GetDsAttached());
  EXPECT_FALSE(DriverStation::IsDSAttached());
  DriverStationSim::NotifyNewData();
  EXPECT_TRUE(DriverStationSim::GetDsAttached());
  EXPECT_TRUE(DriverStation::IsDSAttached());

  BooleanCallback callback;
  auto cb = DriverStationSim::RegisterDsAttachedCallback(callback.GetCallback(),
                                                         false);
  DriverStationSim::SetDsAttached(false);
  DriverStation::RefreshData();
  EXPECT_FALSE(DriverStationSim::GetDsAttached());
  EXPECT_FALSE(DriverStation::IsDSAttached());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_FALSE(callback.GetLastValue());
}

TEST(DriverStationTest, AllianceStationId) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  EnumCallback callback;

  HAL_AllianceStationID allianceStation = HAL_ALLIANCE_STATION_BLUE_2;
  DriverStationSim::SetAllianceStationId(allianceStation);

  auto cb = DriverStationSim::RegisterAllianceStationIdCallback(
      callback.GetCallback(), false);

  // Unknown
  allianceStation = HAL_ALLIANCE_STATION_UNKNOWN;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_FALSE(DriverStation::GetAlliance().has_value());
  EXPECT_FALSE(DriverStation::GetLocation().has_value());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // B1
  allianceStation = HAL_ALLIANCE_STATION_BLUE_1;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::Alliance::BLUE, DriverStation::GetAlliance());
  EXPECT_EQ(1, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // B2
  allianceStation = HAL_ALLIANCE_STATION_BLUE_2;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::Alliance::BLUE, DriverStation::GetAlliance());
  EXPECT_EQ(2, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // B3
  allianceStation = HAL_ALLIANCE_STATION_BLUE_3;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::Alliance::BLUE, DriverStation::GetAlliance());
  EXPECT_EQ(3, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // R1
  allianceStation = HAL_ALLIANCE_STATION_RED_1;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::Alliance::RED, DriverStation::GetAlliance());
  EXPECT_EQ(1, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // R2
  allianceStation = HAL_ALLIANCE_STATION_RED_2;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::Alliance::RED, DriverStation::GetAlliance());
  EXPECT_EQ(2, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // R3
  allianceStation = HAL_ALLIANCE_STATION_RED_3;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::Alliance::RED, DriverStation::GetAlliance());
  EXPECT_EQ(3, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());
}

TEST(DriverStationTest, ReplayNumber) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  DriverStationSim::SetReplayNumber(4);
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(4, DriverStation::GetReplayNumber());
}

TEST(DriverStationTest, MatchNumber) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  DriverStationSim::SetMatchNumber(3);
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(3, DriverStation::GetMatchNumber());
}

TEST(DriverStationTest, MatchTime) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  DoubleCallback callback;
  auto cb = DriverStationSim::RegisterMatchTimeCallback(callback.GetCallback(),
                                                        false);
  constexpr double kTestTime = 19.174;
  DriverStationSim::SetMatchTime(kTestTime);
  wpi::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(kTestTime, DriverStationSim::GetMatchTime());
  EXPECT_EQ(kTestTime, DriverStation::GetMatchTime().value());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(kTestTime, callback.GetLastValue());
}

TEST(DriverStationTest, SetGameData) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  constexpr auto message = "Hello";
  DriverStationSim::SetGameData(message);
  DriverStationSim::NotifyNewData();
  auto gameData = DriverStation::GetGameData();
  ASSERT_TRUE(gameData.has_value());
  EXPECT_EQ(message, gameData.value());
}

TEST(DriverStationTest, SetGameDataEmpty) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  DriverStationSim::SetGameData("");
  DriverStationSim::NotifyNewData();
  auto gameData = DriverStation::GetGameData();
  EXPECT_FALSE(gameData.has_value());
}

TEST(DriverStationTest, SetEventName) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  constexpr auto message = "The Best Event";
  DriverStationSim::SetEventName(message);
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(message, DriverStation::GetEventName());
}
