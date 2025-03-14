// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <tuple>

#include <gtest/gtest.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/DriverStation.h"
#include "frc/Joystick.h"
#include "frc/simulation/DriverStationSim.h"
#include "frc/simulation/SimHooks.h"

using namespace frc;
using namespace frc::sim;

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

  HAL_AllianceStationID allianceStation = HAL_AllianceStationID_kBlue2;
  DriverStationSim::SetAllianceStationId(allianceStation);

  auto cb = DriverStationSim::RegisterAllianceStationIdCallback(
      callback.GetCallback(), false);

  // Unknown
  allianceStation = HAL_AllianceStationID_kUnknown;
  DriverStationSim::SetAllianceStationId(allianceStation);
  frc::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_FALSE(DriverStation::GetAlliance().has_value());
  EXPECT_FALSE(DriverStation::GetLocation().has_value());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // B1
  allianceStation = HAL_AllianceStationID_kBlue1;
  DriverStationSim::SetAllianceStationId(allianceStation);
  frc::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::kBlue, DriverStation::GetAlliance());
  EXPECT_EQ(1, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // B2
  allianceStation = HAL_AllianceStationID_kBlue2;
  DriverStationSim::SetAllianceStationId(allianceStation);
  frc::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::kBlue, DriverStation::GetAlliance());
  EXPECT_EQ(2, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // B3
  allianceStation = HAL_AllianceStationID_kBlue3;
  DriverStationSim::SetAllianceStationId(allianceStation);
  frc::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::kBlue, DriverStation::GetAlliance());
  EXPECT_EQ(3, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // R1
  allianceStation = HAL_AllianceStationID_kRed1;
  DriverStationSim::SetAllianceStationId(allianceStation);
  frc::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::kRed, DriverStation::GetAlliance());
  EXPECT_EQ(1, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // R2
  allianceStation = HAL_AllianceStationID_kRed2;
  DriverStationSim::SetAllianceStationId(allianceStation);
  frc::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::kRed, DriverStation::GetAlliance());
  EXPECT_EQ(2, DriverStation::GetLocation());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(allianceStation, callback.GetLastValue());

  // R3
  allianceStation = HAL_AllianceStationID_kRed3;
  DriverStationSim::SetAllianceStationId(allianceStation);
  frc::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(allianceStation, DriverStationSim::GetAllianceStationId());
  EXPECT_EQ(DriverStation::kRed, DriverStation::GetAlliance());
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
  frc::sim::DriverStationSim::NotifyNewData();
  EXPECT_EQ(kTestTime, DriverStationSim::GetMatchTime());
  EXPECT_EQ(kTestTime, DriverStation::GetMatchTime().value());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(kTestTime, callback.GetLastValue());
}

TEST(DriverStationTest, OpMode) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  int mode1 = DriverStation::AddOpModeOption("name", "category", "desc", 0);
  int mode2 = DriverStation::AddOpModeOption("name2", "category2", "desc2", 1);

  auto modes = DriverStationSim::GetOpModeOptions();
  ASSERT_EQ(2u, modes.size());
  EXPECT_EQ("name", modes[0].name);
  EXPECT_EQ("category", modes[0].category);
  EXPECT_EQ("desc", modes[0].description);
  EXPECT_EQ(0, modes[0].flags);
  EXPECT_EQ("name2", modes[1].name);
  EXPECT_EQ("category2", modes[1].category);
  EXPECT_EQ("desc2", modes[1].description);
  EXPECT_EQ(1, modes[1].flags);

  EXPECT_EQ(0, DriverStation::GetOpModeId());
  EXPECT_EQ("", DriverStation::GetOpMode());

  DriverStationSim::SetOpMode("name");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(mode1, DriverStation::GetOpModeId());
  EXPECT_EQ("name", DriverStation::GetOpMode());

  DriverStationSim::SetOpMode("");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(0, DriverStation::GetOpModeId());
  EXPECT_EQ("", DriverStation::GetOpMode());

  DriverStationSim::SetOpMode("name2");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(mode2, DriverStation::GetOpModeId());
  EXPECT_EQ("name2", DriverStation::GetOpMode());

  DriverStationSim::SetOpMode("unknown");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(0, DriverStation::GetOpModeId());
  EXPECT_EQ("", DriverStation::GetOpMode());
}

TEST(DriverStationTest, SelectedAutonomousOpMode) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  int mode1 = DriverStation::AddOpModeOption("name", "category", "desc", 0);
  int mode2 = DriverStation::AddOpModeOption("name2", "category2", "desc2", 1);

  EXPECT_EQ(0, DriverStation::GetSelectedAutonomousOpModeId());
  EXPECT_EQ("", DriverStation::GetSelectedAutonomousOpMode());

  DriverStationSim::SetSelectedAutonomousOpMode("name");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(mode1, DriverStation::GetSelectedAutonomousOpModeId());
  EXPECT_EQ("name", DriverStation::GetSelectedAutonomousOpMode());

  DriverStationSim::SetSelectedAutonomousOpMode("");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(0, DriverStation::GetSelectedAutonomousOpModeId());
  EXPECT_EQ("", DriverStation::GetSelectedAutonomousOpMode());

  DriverStationSim::SetSelectedAutonomousOpMode("name2");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(mode2, DriverStation::GetSelectedAutonomousOpModeId());
  EXPECT_EQ("name2", DriverStation::GetSelectedAutonomousOpMode());

  DriverStationSim::SetSelectedAutonomousOpMode("unknown");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(0, DriverStation::GetSelectedAutonomousOpModeId());
  EXPECT_EQ("", DriverStation::GetSelectedAutonomousOpMode());
}

TEST(DriverStationTest, SelectedTeleoperatedOpMode) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  int mode1 = DriverStation::AddOpModeOption("name", "category", "desc", 0);
  int mode2 = DriverStation::AddOpModeOption("name2", "category2", "desc2", 1);

  EXPECT_EQ(0, DriverStation::GetSelectedTeleoperatedOpModeId());
  EXPECT_EQ("", DriverStation::GetSelectedTeleoperatedOpMode());

  DriverStationSim::SetSelectedTeleoperatedOpMode("name");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(mode1, DriverStation::GetSelectedTeleoperatedOpModeId());
  EXPECT_EQ("name", DriverStation::GetSelectedTeleoperatedOpMode());

  DriverStationSim::SetSelectedTeleoperatedOpMode("");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(0, DriverStation::GetSelectedTeleoperatedOpModeId());
  EXPECT_EQ("", DriverStation::GetSelectedTeleoperatedOpMode());

  DriverStationSim::SetSelectedTeleoperatedOpMode("name2");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(mode2, DriverStation::GetSelectedTeleoperatedOpModeId());
  EXPECT_EQ("name2", DriverStation::GetSelectedTeleoperatedOpMode());

  DriverStationSim::SetSelectedTeleoperatedOpMode("unknown");
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(0, DriverStation::GetSelectedTeleoperatedOpModeId());
  EXPECT_EQ("", DriverStation::GetSelectedTeleoperatedOpMode());
}

TEST(DriverStationTest, SetGameSpecificMessage) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  constexpr auto message = "Hello World!";
  DriverStationSim::SetGameSpecificMessage(message);
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(message, DriverStation::GetGameSpecificMessage());
}

TEST(DriverStationTest, SetEventName) {
  HAL_Initialize(500, 0);
  DriverStationSim::ResetData();

  constexpr auto message = "The Best Event";
  DriverStationSim::SetEventName(message);
  DriverStationSim::NotifyNewData();
  EXPECT_EQ(message, DriverStation::GetEventName());
}
