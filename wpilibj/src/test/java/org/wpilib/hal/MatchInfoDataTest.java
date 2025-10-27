// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hal;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.driverstation.DriverStation.MatchType;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.MatchInfoData;
import org.wpilib.hardware.hal.simulation.DriverStationDataJNI;
import org.wpilib.simulation.DriverStationSim;

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
