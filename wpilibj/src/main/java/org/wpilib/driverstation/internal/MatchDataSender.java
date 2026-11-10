// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation.internal;

import org.wpilib.driverstation.RobotState;
import org.wpilib.hardware.hal.AllianceStationID;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.MatchInfoData;
import org.wpilib.networktables.BooleanPublisher;
import org.wpilib.networktables.IntegerPublisher;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.StringPublisher;
import org.wpilib.networktables.StringTopic;
import org.wpilib.networktables.StructPublisher;

class MatchDataSender {
  private static final String SMART_DASHBOARD_TYPE = "DriverStation";

  private final StringPublisher m_gameData;
  private final StringPublisher m_eventName;
  private final IntegerPublisher m_matchNumber;
  private final IntegerPublisher m_replayNumber;
  private final IntegerPublisher m_matchType;
  private final BooleanPublisher m_alliance;
  private final IntegerPublisher m_station;
  private final IntegerPublisher m_allianceStation;
  private final StructPublisher<ControlWord> m_controlWord;
  private final StringPublisher m_opMode;
  private String m_oldEventName = "";
  private String m_oldGameData = "";
  private int m_oldMatchNumber;
  private int m_oldReplayNumber;
  private int m_oldMatchType;
  private boolean m_oldIsRedAlliance = true;
  private int m_oldStationNumber = 1;
  private int m_oldAllianceStation = 0;
  private final ControlWord m_oldControlWord = new ControlWord();
  private final MatchInfoData m_matchInfo = new MatchInfoData();

  MatchDataSender() {
    var table = NetworkTableInstance.getDefault().getTable("DriverStation");
    table
        .getStringTopic(".type")
        .publishEx(StringTopic.TYPE_STRING, "{\"SmartDashboard\":\"" + SMART_DASHBOARD_TYPE + "\"}")
        .set(SMART_DASHBOARD_TYPE);
    m_gameData = table.getStringTopic("GameData").publish();
    m_gameData.set("");
    m_eventName = table.getStringTopic("EventName").publish();
    m_eventName.set("");
    m_matchNumber = table.getIntegerTopic("MatchNumber").publish();
    m_matchNumber.set(0);
    m_replayNumber = table.getIntegerTopic("ReplayNumber").publish();
    m_replayNumber.set(0);
    m_matchType = table.getIntegerTopic("MatchType").publish();
    m_matchType.set(0);
    m_alliance = table.getBooleanTopic("IsRedAlliance").publish();
    m_alliance.set(true);
    m_station = table.getIntegerTopic("StationNumber").publish();
    m_station.set(1);
    m_allianceStation = table.getIntegerTopic("AllianceStationID").publish();
    m_allianceStation.set(0);
    m_controlWord = table.getStructTopic("ControlWord", ControlWord.struct).publish();
    m_controlWord.set(m_oldControlWord);
    m_opMode = table.getStringTopic("OpMode").publish();
    m_opMode.set("");
  }

  @SuppressWarnings("VariableDeclarationUsageDistance")
  void sendMatchData(ControlWord controlWord, String currentGameData) {
    AllianceStationID allianceID = DriverStationJNI.getAllianceStation();
    final int stationNumber =
        switch (allianceID) {
          case BLUE_1, RED_1 -> 1;
          case BLUE_2, RED_2 -> 2;
          case BLUE_3, RED_3 -> 3;
          case UNKNOWN -> 0;
        };
    final boolean isRedAlliance =
        switch (allianceID) {
          case BLUE_1, BLUE_2, BLUE_3 -> false;
          case RED_1, RED_2, RED_3, UNKNOWN -> true;
        };

    DriverStationJNI.getMatchInfo(m_matchInfo);
    String currentEventName = m_matchInfo.eventName;
    int currentMatchNumber = m_matchInfo.matchNumber;
    int currentReplayNumber = m_matchInfo.replayNumber;
    int currentMatchType = m_matchInfo.matchType;

    if (m_oldIsRedAlliance != isRedAlliance) {
      m_alliance.set(isRedAlliance);
      m_oldIsRedAlliance = isRedAlliance;
    }
    if (m_oldStationNumber != stationNumber) {
      m_station.set(stationNumber);
      m_oldStationNumber = stationNumber;
    }
    if (m_oldAllianceStation != allianceID.ordinal()) {
      m_allianceStation.set(allianceID.ordinal());
      m_oldAllianceStation = allianceID.ordinal();
    }
    if (!m_oldEventName.equals(currentEventName)) {
      m_eventName.set(currentEventName);
      m_oldEventName = currentEventName;
    }
    if (!m_oldGameData.equals(currentGameData)) {
      m_gameData.set(currentGameData);
      m_oldGameData = currentGameData;
    }
    if (currentMatchNumber != m_oldMatchNumber) {
      m_matchNumber.set(currentMatchNumber);
      m_oldMatchNumber = currentMatchNumber;
    }
    if (currentReplayNumber != m_oldReplayNumber) {
      m_replayNumber.set(currentReplayNumber);
      m_oldReplayNumber = currentReplayNumber;
    }
    if (currentMatchType != m_oldMatchType) {
      m_matchType.set(currentMatchType);
      m_oldMatchType = currentMatchType;
    }
    if (!controlWord.equals(m_oldControlWord)) {
      long currentOpModeId = controlWord.getOpModeId();
      if (currentOpModeId != m_oldControlWord.getOpModeId()) {
        m_opMode.set(RobotState.opModeToString(currentOpModeId));
      }
      m_controlWord.set(controlWord);
      m_oldControlWord.update(controlWord);
    }
  }
}
