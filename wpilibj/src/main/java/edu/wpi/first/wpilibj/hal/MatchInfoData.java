/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

/**
 * Structure for holding the match info data request.
 */
public class MatchInfoData {
  /**
   * Stores the event name.
   */
  @SuppressWarnings("MemberName")
  public String eventName;

  /**
   * Stores the game specific message.
   */
  @SuppressWarnings("MemberName")
  public String gameSpecificMessage;

  /**
   * Stores the match number.
   */
  @SuppressWarnings("MemberName")
  public int matchNumber;

  /**
   * Stores the replay number.
   */
  @SuppressWarnings("MemberName")
  public int replayNumber;

  /**
   * Stores the match type.
   */
  @SuppressWarnings("MemberName")
  public int matchType;

  /**
   * Called from JNI to set the structure data.
   */
  @SuppressWarnings("JavadocMethod")
  public void setData(String eventName, String gameSpecificMessage,
                      int matchNumber, int replayNumber, int matchType) {
    this.eventName = eventName;
    this.gameSpecificMessage = gameSpecificMessage;
    this.matchNumber = matchNumber;
    this.replayNumber = replayNumber;
    this.matchType = matchType;
  }
}
