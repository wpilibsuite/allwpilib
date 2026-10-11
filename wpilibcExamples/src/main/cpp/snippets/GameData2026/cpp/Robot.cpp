// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/MatchState.hpp"
#include "wpi/driverstation/RobotState.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/telemetry/Telemetry.hpp"

/**
 * Determines if the hub is active based on the current match state.
 *
 * Uses the alliance, Game Data and Match Time to determine if the hub is
 * active. The hub is always active in autonomous, and in teleop it is active
 * based on the game data and match time to computer the shifts.
 *
 * @return true if the hub is active, false otherwise
 */
bool IsHubActive() {
  auto alliance = wpi::MatchState::GetAlliance();
  // If we have no alliance, we cannot be enabled, therefore no hub.
  if (!alliance.has_value()) {
    return false;
  }
  // Hub is always enabled in autonomous.
  if (wpi::RobotState::IsAutonomousEnabled()) {
    return true;
  }
  // At this point, if we're not teleop enabled, there is no hub.
  if (!wpi::RobotState::IsTeleopEnabled()) {
    return false;
  }

  // We're teleop enabled, compute.
  auto matchTime = wpi::MatchState::GetMatchTime();
  auto gameData = wpi::MatchState::GetGameData();

  // If we have no game data, we cannot compute, assume hub is active, as its
  // likely early in teleop
  if (!gameData.has_value() || gameData->empty()) {
    return true;
  }

  bool redInactiveFirst;
  switch ((*gameData)[0]) {
    case 'R':
      redInactiveFirst = true;
      break;
    case 'B':
      redInactiveFirst = false;
      break;
    default:
      // If we have invalid game data, assume hub is active.
      return true;
  }

  // Shift was is active for blue if red won auto, or red if blue won auto.
  bool shift1Active =
      *alliance == wpi::Alliance::RED ? !redInactiveFirst : redInactiveFirst;

  if (matchTime > 130_s) {
    return true;
  } else if (matchTime > 105_s) {
    return shift1Active;
  } else if (matchTime > 80_s) {
    return !shift1Active;
  } else if (matchTime > 55_s) {
    return shift1Active;
  } else if (matchTime > 30_s) {
    return !shift1Active;
  } else {
    // End game, hub always active.
    return true;
  }
}

/**
 * Game Data from 2026 snippets for wpilib-docs.
 * https://docs.wpilib.org/en/latest/docs/yearly-overview/2026-game-data.html
 */
class Robot : public wpi::TimedRobot {
 public:
  void TeleopPeriodic() override {
    auto gameData = wpi::MatchState::GetGameData();
    auto alliance = wpi::MatchState::GetAlliance();
    wpi::telemetry::Log("Hub active", IsHubActive());
    wpi::telemetry::Log("Match time", wpi::MatchState::GetMatchTime());
    wpi::telemetry::Log("Game data",
                        gameData.has_value() ? *gameData : "No game data");
    wpi::telemetry::Log("Alliance",
                        alliance.has_value()
                            ? (*alliance == wpi::Alliance::RED ? "RED" : "BLUE")
                            : "No alliance");
  }

  /** Example of Handling the game data received from the driver station for
   * 2026 game. */
  void HandleGameData() {
    auto gameData = wpi::MatchState::GetGameData();
    if (gameData.has_value() && gameData->length() > 0) {
      switch (gameData->at(0)) {
        case 'B':
          // Blue case code
          break;
        case 'R':
          // Red case code
          break;
        default:
          // This is corrupt data
          break;
      }
    } else {
      // Code for no data received yet
    }
  }
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
