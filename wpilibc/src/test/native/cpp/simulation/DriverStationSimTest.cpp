// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/DriverStationSim.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/driverstation/MatchState.hpp"
#include "wpi/driverstation/RobotState.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/HAL.h"

using namespace wpi;
using namespace wpi::sim;

TEST_CASE("DriverStationTest Enabled", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();
  DriverStationSim::NotifyNewData();

  CHECK_FALSE(RobotState::IsEnabled());
  BooleanCallback callback;
  auto cb =
      DriverStationSim::RegisterEnabledCallback(callback.GetCallback(), false);
  DriverStationSim::SetRobotMode(RobotMode::TELEOPERATED);
  DriverStationSim::SetEnabled(true);
  DriverStationSim::NotifyNewData();
  CHECK(DriverStationSim::GetEnabled());
  CHECK(RobotState::IsEnabled());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());
}

TEST_CASE("DriverStationTest AutonomousMode", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();
  DriverStationSim::NotifyNewData();

  CHECK_FALSE(RobotState::IsAutonomous());
  EnumCallback callback;
  auto cb = DriverStationSim::RegisterRobotModeCallback(callback.GetCallback(),
                                                        false);
  DriverStationSim::SetRobotMode(RobotMode::AUTONOMOUS);
  DriverStationSim::NotifyNewData();
  CHECK(DriverStationSim::GetRobotMode() == RobotMode::AUTONOMOUS);
  CHECK(RobotState::IsAutonomous());
  CHECK(RobotState::GetRobotMode() == RobotMode::AUTONOMOUS);
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue() == HAL_ROBOT_MODE_AUTONOMOUS);
}

TEST_CASE("DriverStationTest Mode", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();
  DriverStationSim::NotifyNewData();

  CHECK_FALSE(RobotState::IsUtility());
  EnumCallback callback;
  auto cb = DriverStationSim::RegisterRobotModeCallback(callback.GetCallback(),
                                                        false);
  DriverStationSim::SetRobotMode(RobotMode::UTILITY);
  DriverStationSim::NotifyNewData();
  CHECK(DriverStationSim::GetRobotMode() == RobotMode::UTILITY);
  CHECK(RobotState::IsUtility());
  CHECK(RobotState::GetRobotMode() == RobotMode::UTILITY);
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue() == HAL_ROBOT_MODE_UTILITY);
}

TEST_CASE("DriverStationTest Estop", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();
  DriverStationSim::NotifyNewData();

  CHECK_FALSE(RobotState::IsEStopped());
  BooleanCallback callback;
  auto cb =
      DriverStationSim::RegisterEStopCallback(callback.GetCallback(), false);
  DriverStationSim::SetEStop(true);
  DriverStationSim::NotifyNewData();
  CHECK(DriverStationSim::GetEStop());
  CHECK(RobotState::IsEStopped());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());
}

TEST_CASE("DriverStationTest FmsAttached", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();
  DriverStationSim::NotifyNewData();

  CHECK_FALSE(RobotState::IsFMSAttached());
  BooleanCallback callback;
  auto cb = DriverStationSim::RegisterFmsAttachedCallback(
      callback.GetCallback(), false);
  DriverStationSim::SetFmsAttached(true);
  DriverStationSim::NotifyNewData();
  CHECK(DriverStationSim::GetFmsAttached());
  CHECK(RobotState::IsFMSAttached());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());
}

TEST_CASE("DriverStationTest DsAttached", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();
  wpi::internal::DriverStationBackend::RefreshData();

  CHECK_FALSE(DriverStationSim::GetDsAttached());
  CHECK_FALSE(RobotState::IsDSAttached());
  DriverStationSim::NotifyNewData();
  CHECK(DriverStationSim::GetDsAttached());
  CHECK(RobotState::IsDSAttached());

  BooleanCallback callback;
  auto cb = DriverStationSim::RegisterDsAttachedCallback(callback.GetCallback(),
                                                         false);
  DriverStationSim::SetDsAttached(false);
  wpi::internal::DriverStationBackend::RefreshData();
  CHECK_FALSE(DriverStationSim::GetDsAttached());
  CHECK_FALSE(RobotState::IsDSAttached());
  CHECK(callback.WasTriggered());
  CHECK_FALSE(callback.GetLastValue());
}

TEST_CASE("DriverStationTest AllianceStationId", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();

  EnumCallback callback;

  hal::AllianceStationID allianceStation = hal::AllianceStationID::BLUE_2;
  DriverStationSim::SetAllianceStationId(allianceStation);

  auto cb = DriverStationSim::RegisterAllianceStationIdCallback(
      callback.GetCallback(), false);

  // Unknown
  allianceStation = hal::AllianceStationID::UNKNOWN;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  CHECK(allianceStation == DriverStationSim::GetAllianceStationId());
  CHECK_FALSE(MatchState::GetAlliance().has_value());
  CHECK_FALSE(MatchState::GetLocation().has_value());
  CHECK(callback.WasTriggered());
  CHECK(static_cast<HAL_AllianceStationID>(allianceStation) ==
        callback.GetLastValue());

  // B1
  allianceStation = hal::AllianceStationID::BLUE_1;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  CHECK(allianceStation == DriverStationSim::GetAllianceStationId());
  CHECK(Alliance::BLUE == MatchState::GetAlliance());
  CHECK(1 == MatchState::GetLocation());
  CHECK(callback.WasTriggered());
  CHECK(static_cast<HAL_AllianceStationID>(allianceStation) ==
        callback.GetLastValue());

  // B2
  allianceStation = hal::AllianceStationID::BLUE_2;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  CHECK(allianceStation == DriverStationSim::GetAllianceStationId());
  CHECK(Alliance::BLUE == MatchState::GetAlliance());
  CHECK(2 == MatchState::GetLocation());
  CHECK(callback.WasTriggered());
  CHECK(static_cast<HAL_AllianceStationID>(allianceStation) ==
        callback.GetLastValue());

  // B3
  allianceStation = hal::AllianceStationID::BLUE_3;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  CHECK(allianceStation == DriverStationSim::GetAllianceStationId());
  CHECK(Alliance::BLUE == MatchState::GetAlliance());
  CHECK(3 == MatchState::GetLocation());
  CHECK(callback.WasTriggered());
  CHECK(static_cast<HAL_AllianceStationID>(allianceStation) ==
        callback.GetLastValue());

  // R1
  allianceStation = hal::AllianceStationID::RED_1;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  CHECK(allianceStation == DriverStationSim::GetAllianceStationId());
  CHECK(Alliance::RED == MatchState::GetAlliance());
  CHECK(1 == MatchState::GetLocation());
  CHECK(callback.WasTriggered());
  CHECK(static_cast<HAL_AllianceStationID>(allianceStation) ==
        callback.GetLastValue());

  // R2
  allianceStation = hal::AllianceStationID::RED_2;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  CHECK(allianceStation == DriverStationSim::GetAllianceStationId());
  CHECK(Alliance::RED == MatchState::GetAlliance());
  CHECK(2 == MatchState::GetLocation());
  CHECK(callback.WasTriggered());
  CHECK(static_cast<HAL_AllianceStationID>(allianceStation) ==
        callback.GetLastValue());

  // R3
  allianceStation = hal::AllianceStationID::RED_3;
  DriverStationSim::SetAllianceStationId(allianceStation);
  wpi::sim::DriverStationSim::NotifyNewData();
  CHECK(allianceStation == DriverStationSim::GetAllianceStationId());
  CHECK(Alliance::RED == MatchState::GetAlliance());
  CHECK(3 == MatchState::GetLocation());
  CHECK(callback.WasTriggered());
  CHECK(static_cast<HAL_AllianceStationID>(allianceStation) ==
        callback.GetLastValue());
}

TEST_CASE("DriverStationTest ReplayNumber", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();

  DriverStationSim::SetReplayNumber(4);
  DriverStationSim::NotifyNewData();
  CHECK(4 == MatchState::GetReplayNumber());
}

TEST_CASE("DriverStationTest MatchNumber", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();

  DriverStationSim::SetMatchNumber(3);
  DriverStationSim::NotifyNewData();
  CHECK(3 == MatchState::GetMatchNumber());
}

TEST_CASE("DriverStationTest MatchTime", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();

  DoubleCallback callback;
  auto cb = DriverStationSim::RegisterMatchTimeCallback(callback.GetCallback(),
                                                        false);
  constexpr double TEST_TIME = 19.174;
  DriverStationSim::SetMatchTime(TEST_TIME);
  wpi::sim::DriverStationSim::NotifyNewData();
  CHECK(TEST_TIME == DriverStationSim::GetMatchTime());
  CHECK(TEST_TIME == MatchState::GetMatchTime().value());
  CHECK(callback.WasTriggered());
  CHECK(TEST_TIME == callback.GetLastValue());
}

TEST_CASE("DriverStationTest SetGameData", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();

  constexpr auto message = "Hello";
  DriverStationSim::SetGameData(message);
  DriverStationSim::NotifyNewData();
  auto gameData = MatchState::GetGameData();
  REQUIRE(gameData.has_value());
  CHECK(message == gameData.value());
}

TEST_CASE("DriverStationTest SetGameDataEmpty", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();

  DriverStationSim::SetGameData("");
  DriverStationSim::NotifyNewData();
  auto gameData = MatchState::GetGameData();
  CHECK_FALSE(gameData.has_value());
}

TEST_CASE("DriverStationTest SetEventName", "[wpilibc][simulation]") {
  HAL_Initialize();
  DriverStationSim::ResetData();

  constexpr auto message = "The Best Event";
  DriverStationSim::SetEventName(message);
  DriverStationSim::NotifyNewData();
  CHECK(message == MatchState::GetEventName());
}
