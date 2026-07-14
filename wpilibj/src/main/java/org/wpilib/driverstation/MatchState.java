// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import java.util.Map;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.concurrent.locks.ReentrantLock;
import org.wpilib.hardware.hal.AllianceStationID;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.MatchInfoData;

/** Provides access to match state information from the Driver Station. */
public final class MatchState {
  static {
    HAL.initialize(500, 0);
  }

  private static MatchInfoData m_matchInfo = new MatchInfoData();
  private static MatchInfoData m_matchInfoCache = new MatchInfoData();
  private static String m_gameData = "";
  private static final ReentrantLock m_dataMutex = new ReentrantLock();

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
    return DriverStationJNI.getMatchTime();
  }

  private static Map<AllianceStationID, Optional<Alliance>> m_allianceMap =
      Map.of(
          AllianceStationID.UNKNOWN, Optional.empty(),
          AllianceStationID.RED_1, Optional.of(Alliance.RED),
          AllianceStationID.RED_2, Optional.of(Alliance.RED),
          AllianceStationID.RED_3, Optional.of(Alliance.RED),
          AllianceStationID.BLUE_1, Optional.of(Alliance.BLUE),
          AllianceStationID.BLUE_2, Optional.of(Alliance.BLUE),
          AllianceStationID.BLUE_3, Optional.of(Alliance.BLUE));

  private static Map<AllianceStationID, OptionalInt> m_stationMap =
      Map.of(
          AllianceStationID.UNKNOWN, OptionalInt.empty(),
          AllianceStationID.RED_1, OptionalInt.of(1),
          AllianceStationID.RED_2, OptionalInt.of(2),
          AllianceStationID.RED_3, OptionalInt.of(3),
          AllianceStationID.BLUE_1, OptionalInt.of(1),
          AllianceStationID.BLUE_2, OptionalInt.of(2),
          AllianceStationID.BLUE_3, OptionalInt.of(3));

  /**
   * Get the current alliance from the FMS.
   *
   * <p>If the FMS is not connected, it is set from the team alliance setting on the driver station.
   *
   * @return The alliance (red or blue) or an empty optional if the alliance is invalid
   */
  public static Optional<Alliance> getAlliance() {
    AllianceStationID allianceStationID = DriverStationJNI.getAllianceStation();
    if (allianceStationID == null) {
      allianceStationID = AllianceStationID.UNKNOWN;
    }

    return m_allianceMap.get(allianceStationID);
  }

  /**
   * Gets the location of the team's driver station controls from the FMS.
   *
   * <p>If the FMS is not connected, it is set from the team alliance setting on the driver station.
   *
   * @return the location of the team's driver station controls: 1, 2, or 3
   */
  public static OptionalInt getLocation() {
    AllianceStationID allianceStationID = DriverStationJNI.getAllianceStation();
    if (allianceStationID == null) {
      allianceStationID = AllianceStationID.UNKNOWN;
    }

    return m_stationMap.get(allianceStationID);
  }

  /**
   * Get the replay number from the FMS.
   *
   * @return the replay number
   */
  public static int getReplayNumber() {
    m_dataMutex.lock();
    try {
      return m_matchInfo.replayNumber;
    } finally {
      m_dataMutex.unlock();
    }
  }

  /**
   * Get the match number from the FMS.
   *
   * @return the match number
   */
  public static int getMatchNumber() {
    m_dataMutex.lock();
    try {
      return m_matchInfo.matchNumber;
    } finally {
      m_dataMutex.unlock();
    }
  }

  /**
   * Get the match type from the FMS.
   *
   * @return the match type
   */
  public static MatchType getMatchType() {
    int matchType;
    m_dataMutex.lock();
    try {
      matchType = m_matchInfo.matchType;
    } finally {
      m_dataMutex.unlock();
    }
    return switch (matchType) {
      case 1 -> MatchType.PRACTICE;
      case 2 -> MatchType.QUALIFICATION;
      case 3 -> MatchType.ELIMINATION;
      default -> MatchType.NONE;
    };
  }

  /**
   * Get the event name from the FMS.
   *
   * @return the event name
   */
  public static String getEventName() {
    m_dataMutex.lock();
    try {
      return m_matchInfo.eventName;
    } finally {
      m_dataMutex.unlock();
    }
  }

  /**
   * Get the game specific message from the FMS.
   *
   * <p>If the FMS is not connected, it is set from the game data setting on the driver station.
   *
   * @return the game specific message
   */
  public static Optional<String> getGameData() {
    m_dataMutex.lock();
    try {
      if (m_gameData == null || m_gameData.isEmpty()) {
        return Optional.empty();
      }
      return Optional.of(m_gameData);
    } finally {
      m_dataMutex.unlock();
    }
  }

  /**
   * Updates match info.
   *
   * @param gameData The current game data.
   */
  public static void refreshMatchInfo(String gameData) {
    DriverStationJNI.getMatchInfo(m_matchInfoCache);
    m_dataMutex.lock();
    try {
      var matchInfo = m_matchInfo;
      m_matchInfo = m_matchInfoCache;
      m_matchInfoCache = matchInfo;

      m_gameData = gameData;
    } finally {
      m_dataMutex.unlock();
    }
  }
}
