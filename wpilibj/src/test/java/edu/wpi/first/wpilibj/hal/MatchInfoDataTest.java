/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import org.junit.jupiter.api.Test;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.MatchInfoData;
import edu.wpi.first.hal.simulation.DriverStationDataJNI;
import edu.wpi.first.wpilibj.DriverStation.MatchType;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class MatchInfoDataTest {
  @Test
  void testSetMatchInfo() {

    MatchType matchType = MatchType.Qualification;
    DriverStationDataJNI.setMatchInfo("Event Name", "Game Message", 174, 191, matchType.ordinal());

    MatchInfoData outMatchInfo = new MatchInfoData();
    HAL.getMatchInfo(outMatchInfo);

    assertAll(
        () -> assertEquals("Event Name", outMatchInfo.eventName),
        () -> assertEquals(matchType.ordinal(), outMatchInfo.matchType),
        () -> assertEquals(174, outMatchInfo.matchNumber),
        () -> assertEquals(191, outMatchInfo.replayNumber),
        () -> assertEquals("Game Message", outMatchInfo.gameSpecificMessage)
    );
  }
}
