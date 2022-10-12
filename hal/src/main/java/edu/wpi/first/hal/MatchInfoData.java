// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Structure for holding the match info data request. */
@SuppressWarnings("MemberName")
public class MatchInfoData {
  /** Stores the event name. */
  public String eventName = "";

  /** Stores the game specific message. */
  public String gameSpecificMessage = "";

  /** Stores the match number. */
  public int matchNumber;

  /** Stores the replay number. */
  public int replayNumber;

  /** Stores the match type. */
  public int matchType;

  /**
   * Called from JNI to set the structure data.
   *
   * @param eventName Event name.
   * @param gameSpecificMessage Game-specific message.
   * @param matchNumber Match number.
   * @param replayNumber Replay number.
   * @param matchType Match type.
   */
  public void setData(
      String eventName,
      String gameSpecificMessage,
      int matchNumber,
      int replayNumber,
      int matchType) {
    this.eventName = eventName;
    this.gameSpecificMessage = gameSpecificMessage;
    this.matchNumber = matchNumber;
    this.replayNumber = replayNumber;
    this.matchType = matchType;
  }
}
