// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/MatchState.hpp"
#include "wpi/driverstation/RobotState.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/telemetry/Telemetry.hpp"

/**
 * Game Data from 2026 snippets for wpilib-docs.
 * https://docs.wpilib.org/en/latest/docs/yearly-overview/2026-game-data.html
 */
class Robot : public wpi::TimedRobot {
 public:
  bool IsHubActive() {
    auto alliance = wpi::MatchState::GetAlliance();
    if (!alliance.has_value()) {
      return false;
    }
    if (wpi::RobotState::IsAutonomousEnabled()) {
      return true;
    }
    if (!wpi::RobotState::IsTeleopEnabled()) {
      return false;
    }

    auto matchTime = wpi::MatchState::GetMatchTime().value();
    auto gameData = wpi::MatchState::GetGameData();
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
        return true;
    }

    bool shift1Active =
        *alliance == wpi::Alliance::RED ? !redInactiveFirst : redInactiveFirst;

    if (matchTime > 130) {
      return true;
    } else if (matchTime > 105) {
      return shift1Active;
    } else if (matchTime > 80) {
      return !shift1Active;
    } else if (matchTime > 55) {
      return shift1Active;
    } else if (matchTime > 30) {
      return !shift1Active;
    } else {
      return true;
    }
  }

  void TeleopPeriodic() override {
    auto gameData = wpi::MatchState::GetGameData();
    auto alliance = wpi::MatchState::GetAlliance();
    wpi::telemetry::Log("Hub active", IsHubActive());
    wpi::telemetry::Log("Match time", wpi::MatchState::GetMatchTime().value());
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
