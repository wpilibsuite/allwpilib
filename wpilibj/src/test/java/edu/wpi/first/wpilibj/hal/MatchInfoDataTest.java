// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.hal;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.hal.MatchInfoData;
import edu.wpi.first.hal.simulation.DriverStationDataJNI;
import edu.wpi.first.wpilibj.DriverStation.MatchType;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import org.junit.jupiter.api.Test;

class MatchInfoDataTest {
  @Test
  void testSetMatchInfo() {
    MatchType matchType = MatchType.Qualification;
    DriverStationDataJNI.setMatchInfo("Event Name", "Game Message", 174, 191, matchType.ordinal());

    DriverStationSim.notifyNewData();

    MatchInfoData outMatchInfo = new MatchInfoData();
    DriverStationJNI.getMatchInfo(outMatchInfo);

    assertAll(
        () -> assertEquals("Event Name", outMatchInfo.eventName),
        () -> assertEquals(matchType.ordinal(), outMatchInfo.matchType),
        () -> assertEquals(174, outMatchInfo.matchNumber),
        () -> assertEquals(191, outMatchInfo.replayNumber),
        () -> assertEquals("Game Message", outMatchInfo.gameSpecificMessage));
  }
}
