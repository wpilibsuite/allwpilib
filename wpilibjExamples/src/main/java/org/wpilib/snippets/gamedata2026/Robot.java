// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.gamedata2026;

import java.util.Optional;
import org.wpilib.driverstation.Alliance;
import org.wpilib.driverstation.MatchState;
import org.wpilib.driverstation.RobotState;
import org.wpilib.framework.TimedRobot;
import org.wpilib.telemetry.Telemetry;

/**
 * Game Data from 2026 snippets for wpilib-docs.
 * https://docs.wpilib.org/en/latest/docs/yearly-overview/2026-game-data.html
 */
public class Robot extends TimedRobot {
  /**
   * Determines if the hub is active based on the current match state.
   *
   * <p>Uses the alliance, Game Data and Match Time to determine if the hub is active. The hub is
   * always active in autonomous, and in teleop it is active based on the game data and match time
   * to computer the shifts.
   *
   * @return true if the hub is active, false otherwise
   */
  public static boolean isHubActive() {
    Optional<Alliance> alliance = MatchState.getAlliance();
    // If we have no alliance, we cannot be enabled, therefore no hub.
    if (alliance.isEmpty()) {
      return false;
    }
    // Hub is always enabled in autonomous.
    if (RobotState.isAutonomousEnabled()) {
      return true;
    }
    // At this point, if we're not teleop enabled, there is no hub.
    if (!RobotState.isTeleopEnabled()) {
      return false;
    }

    // We're teleop enabled, compute.
    double matchTime = MatchState.getMatchTime();
    Optional<String> gameData = MatchState.getGameData();
    // If we have no game data, we cannot compute, assume hub is active, as its
    // likely early in teleop
    if (!gameData.isPresent() || gameData.get().isEmpty()) {
      return true;
    }
    boolean redInactiveFirst = false;
    switch (gameData.get().charAt(0)) {
      case 'R' -> redInactiveFirst = true;
      case 'B' -> redInactiveFirst = false;
      default -> {
        // If we have invalid game data, assume hub is active.
        return true;
      }
    }

    // Shift was is active for blue if red won auto, or red if blue won auto.
    boolean shift1Active =
        switch (alliance.get()) {
          case Alliance.RED -> !redInactiveFirst;
          case Alliance.BLUE -> redInactiveFirst;
        };

    if (matchTime > 130) {
      // Transition shift, hub is active.
      return true;
    } else if (matchTime > 105) {
      // Shift 1
      return shift1Active;
    } else if (matchTime > 80) {
      // Shift 2
      return !shift1Active;
    } else if (matchTime > 55) {
      // Shift 3
      return shift1Active;
    } else if (matchTime > 30) {
      // Shift 4
      return !shift1Active;
    } else {
      // End game, hub always active.
      return true;
    }
  }

  @Override
  public void teleopPeriodic() {
    Telemetry.log("Hub active", isHubActive());
    Telemetry.log("Match time", MatchState.getMatchTime());
    Telemetry.log("Game data", MatchState.getGameData().orElse("No game data"));
    Telemetry.log("Alliance", MatchState.getAlliance().map(Enum::name).orElse("No alliance"));
  }

  /** Example of Handling the game data received from the driver station for 2026 game. */
  public void handleGameData() {
    Optional<String> gameData = MatchState.getGameData();
    if (gameData.isPresent() && gameData.get().length() > 0) {
      switch (gameData.get().charAt(0)) {
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
}
