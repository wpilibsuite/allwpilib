// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import java.util.Optional;
import java.util.OptionalInt;
import org.wpilib.driverstation.internal.DriverStationBackend;

/** Provides access to match state information from the Driver Station. */
public final class MatchState {
  private MatchState() {}

  /**
   * Return the approximate match time. The FMS does not send an official match time to the robots,
   * but does send an approximate match time. The value will count down the time remaining in the
   * current period (auto or teleop). Warning: This is not an official time (so it cannot be used to
   * dispute ref calls or guarantee that a function will trigger before the match ends).
   *
   * <p>When connected to the real field, this number only changes in full integer increments, and
   * always counts down.
   *
   * <p>When the DS is in practice mode, this number is a floating point number, and counts down.
   *
   * <p>When the DS is in teleop or autonomous mode, this number returns -1.0.
   *
   * <p>Simulation matches DS behavior without an FMS connected.
   *
   * @return Time remaining in current match period (auto or teleop) in seconds
   */
  public static double getMatchTime() {
    return DriverStationBackend.getMatchTime();
  }

  /**
   * Get the current alliance from the FMS.
   *
   * <p>If the FMS is not connected, it is set from the team alliance setting on the driver station.
   *
   * @return The alliance (red or blue) or an empty optional if the alliance is invalid
   */
  public static Optional<Alliance> getAlliance() {
    return DriverStationBackend.getAlliance();
  }

  /**
   * Gets the location of the team's driver station controls from the FMS.
   *
   * <p>If the FMS is not connected, it is set from the team alliance setting on the driver station.
   *
   * @return the location of the team's driver station controls: 1, 2, or 3
   */
  public static OptionalInt getLocation() {
    return DriverStationBackend.getLocation();
  }

  /**
   * Get the replay number from the FMS.
   *
   * @return the replay number
   */
  public static int getReplayNumber() {
    return DriverStationBackend.getReplayNumber();
  }

  /**
   * Get the match number from the FMS.
   *
   * @return the match number
   */
  public static int getMatchNumber() {
    return DriverStationBackend.getMatchNumber();
  }

  /**
   * Get the match type from the FMS.
   *
   * @return the match type
   */
  public static MatchType getMatchType() {
    return DriverStationBackend.getMatchType();
  }

  /**
   * Get the event name from the FMS.
   *
   * @return the event name
   */
  public static String getEventName() {
    return DriverStationBackend.getEventName();
  }

  /**
   * Get the game specific message from the FMS.
   *
   * <p>If the FMS is not connected, it is set from the game data setting on the driver station.
   *
   * @return the game specific message
   */
  public static Optional<String> getGameData() {
    return DriverStationBackend.getGameData();
  }
}
