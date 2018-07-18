/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import org.junit.jupiter.api.Test;

import edu.wpi.first.hal.sim.mockdata.DriverStationDataJNI;
import edu.wpi.first.wpilibj.DriverStation.MatchType;

import static org.junit.jupiter.api.Assertions.assertEquals;

class MatchInfoDataTest {
  @Test
  void matchInfoDataEventName() {
    HAL.initialize(500, 0);

    MatchInfoData inMatchInfo = new MatchInfoData();
    inMatchInfo.eventName = "Event Name";
    DriverStationDataJNI.setMatchInfo(inMatchInfo);

    MatchInfoData outMatchInfo = new MatchInfoData();
    HAL.getMatchInfo(outMatchInfo);

    assertEquals("Event Name", outMatchInfo.eventName);
  }

  @Test
  void matchInfoDataGameMessage() {
    HAL.initialize(500, 0);

    MatchInfoData inMatchInfo = new MatchInfoData();
    inMatchInfo.gameSpecificMessage = "Game Message";
    DriverStationDataJNI.setMatchInfo(inMatchInfo);

    MatchInfoData outMatchInfo = new MatchInfoData();
    HAL.getMatchInfo(outMatchInfo);

    assertEquals("Game Message", outMatchInfo.gameSpecificMessage);
  }

  @Test
  void matchInfoDataMatchnumber() {
    HAL.initialize(500, 0);

    MatchInfoData inMatchInfo = new MatchInfoData();
    inMatchInfo.matchNumber = 174;
    DriverStationDataJNI.setMatchInfo(inMatchInfo);

    MatchInfoData outMatchInfo = new MatchInfoData();
    HAL.getMatchInfo(outMatchInfo);

    assertEquals(174, outMatchInfo.matchNumber);
  }

  @Test
  void matchInfoDataMatchType() {
    HAL.initialize(500, 0);

    MatchInfoData inMatchInfo = new MatchInfoData();
    inMatchInfo.matchType = MatchType.Qualification.ordinal();
    DriverStationDataJNI.setMatchInfo(inMatchInfo);

    MatchInfoData outMatchInfo = new MatchInfoData();
    HAL.getMatchInfo(outMatchInfo);

    assertEquals(MatchType.Qualification.ordinal(), outMatchInfo.matchType);
  }

  @Test
  void matchInfoDataReplayNumber() {
    HAL.initialize(500, 0);

    MatchInfoData inMatchInfo = new MatchInfoData();
    inMatchInfo.replayNumber = 191;
    DriverStationDataJNI.setMatchInfo(inMatchInfo);

    MatchInfoData outMatchInfo = new MatchInfoData();
    HAL.getMatchInfo(outMatchInfo);

    assertEquals(191, outMatchInfo.replayNumber);
  }
}
