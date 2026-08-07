// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/MatchState.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "wpi/driverstation/Alliance.hpp"
#include "wpi/driverstation/MatchType.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/units/time.hpp"

namespace wpi {
wpi::units::second_t MatchState::GetMatchTime() {
  int32_t status = 0;
  return wpi::units::second_t{HAL_GetMatchTime(&status)};
}

std::optional<Alliance> MatchState::GetAlliance() {
  int32_t status = 0;
  auto allianceStationID = HAL_GetAllianceStation(&status);
  switch (allianceStationID) {
    case HAL_ALLIANCE_STATION_RED_1:
    case HAL_ALLIANCE_STATION_RED_2:
    case HAL_ALLIANCE_STATION_RED_3:
      return Alliance::RED;
    case HAL_ALLIANCE_STATION_BLUE_1:
    case HAL_ALLIANCE_STATION_BLUE_2:
    case HAL_ALLIANCE_STATION_BLUE_3:
      return Alliance::BLUE;
    default:
      return {};
  }
}

std::optional<int> MatchState::GetLocation() {
  int32_t status = 0;
  auto allianceStationID = HAL_GetAllianceStation(&status);
  switch (allianceStationID) {
    case HAL_ALLIANCE_STATION_RED_1:
    case HAL_ALLIANCE_STATION_BLUE_1:
      return 1;
    case HAL_ALLIANCE_STATION_RED_2:
    case HAL_ALLIANCE_STATION_BLUE_2:
      return 2;
    case HAL_ALLIANCE_STATION_RED_3:
    case HAL_ALLIANCE_STATION_BLUE_3:
      return 3;
    default:
      return {};
  }
}

int MatchState::GetReplayNumber() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.replayNumber;
}

int MatchState::GetMatchNumber() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.matchNumber;
}

MatchType MatchState::GetMatchType() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return static_cast<MatchType>(info.matchType);
}

std::string MatchState::GetEventName() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.eventName;
}

std::optional<std::string> MatchState::GetGameData() {
  HAL_GameData info;
  HAL_GetGameData(&info);
  std::string_view gameDataView{reinterpret_cast<char*>(info.gameData)};
  if (gameDataView.empty()) {
    return std::nullopt;
  }
  return std::string(gameDataView);
}
}  // namespace wpi
