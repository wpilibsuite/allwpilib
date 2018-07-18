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

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class MatchInfoDataTest {
  @Test
  void matchInfoDataDoesNotThrow() {

    MatchInfoData inMatchInfo = new MatchInfoData();
    inMatchInfo.eventName = "Event Name";
    inMatchInfo.gameSpecificMessage = "Game Message";
    inMatchInfo.matchNumber = 174;
    inMatchInfo.matchType = MatchType.Qualification.ordinal();
    inMatchInfo.replayNumber = 191;
    DriverStationDataJNI.setMatchInfo(inMatchInfo);

    MatchInfoData outMatchInfo = new MatchInfoData();
    HAL.getMatchInfo(outMatchInfo);

    assertAll(
      () -> assertEquals("Event Name", outMatchInfo.eventName),
      () -> assertEquals(MatchType.Qualification.ordinal(), outMatchInfo.matchType),
      () -> assertEquals(174, outMatchInfo.matchNumber),
      () -> assertEquals(191, outMatchInfo.replayNumber),
      () -> assertEquals("Game Message", outMatchInfo.gameSpecificMessage)
    );
  }
}
