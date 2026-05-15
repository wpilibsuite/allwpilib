// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;
import org.wpilib.driverstation.Alliance;
import org.wpilib.driverstation.MatchState;
import org.wpilib.driverstation.MatchType;
import org.wpilib.driverstation.RobotState;
import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.hardware.hal.AllianceStationID;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.simulation.testutils.BooleanCallback;
import org.wpilib.simulation.testutils.DoubleCallback;
import org.wpilib.simulation.testutils.EnumCallback;

class DriverStationSimTest {
  @Test
  void testEnabled() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(RobotState.isEnabled());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerEnabledCallback(callback, false)) {
      DriverStationSim.setEnabled(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getEnabled());
      assertTrue(RobotState.isEnabled());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testAutonomous() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(RobotState.isAutonomous());
    EnumCallback callback = new EnumCallback();
    try (CallbackStore cb = DriverStationSim.registerRobotModeCallback(callback, false)) {
      DriverStationSim.setRobotMode(RobotMode.AUTONOMOUS);
      DriverStationSim.notifyNewData();
      assertEquals(RobotMode.AUTONOMOUS, DriverStationSim.getRobotMode());
      assertTrue(RobotState.isAutonomous());
      assertEquals(RobotMode.AUTONOMOUS, RobotState.getRobotMode());
      assertTrue(callback.wasTriggered());
      assertEquals(RobotMode.AUTONOMOUS.getValue(), callback.getSetValue());
    }
  }

  @Test
  void testTest() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(RobotState.isUtility());
    EnumCallback callback = new EnumCallback();
    try (CallbackStore cb = DriverStationSim.registerRobotModeCallback(callback, false)) {
      DriverStationSim.setRobotMode(RobotMode.UTILITY);
      DriverStationSim.notifyNewData();
      assertEquals(RobotMode.UTILITY, DriverStationSim.getRobotMode());
      assertTrue(RobotState.isUtility());
      assertEquals(RobotMode.UTILITY, RobotState.getRobotMode());
      assertTrue(callback.wasTriggered());
      assertEquals(RobotMode.UTILITY.getValue(), callback.getSetValue());
    }
  }

  @Test
  void testEstop() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(RobotState.isEStopped());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerEStopCallback(callback, false)) {
      DriverStationSim.setEStop(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getEStop());
      assertTrue(RobotState.isEStopped());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testFmsAttached() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(RobotState.isFMSAttached());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerFmsAttachedCallback(callback, false)) {
      DriverStationSim.setFmsAttached(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getFmsAttached());
      assertTrue(RobotState.isFMSAttached());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testDsAttached() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();
    DriverStationBackend.refreshData();

    assertFalse(DriverStationSim.getDsAttached());
    assertFalse(RobotState.isDSAttached());
    DriverStationSim.notifyNewData();
    assertTrue(DriverStationSim.getDsAttached());
    assertTrue(RobotState.isDSAttached());

    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerDsAttachedCallback(callback, false)) {
      DriverStationSim.setDsAttached(false);
      DriverStationBackend.refreshData();
      assertFalse(DriverStationSim.getDsAttached());
      assertFalse(RobotState.isDSAttached());
      assertTrue(callback.wasTriggered());
      assertFalse(callback.getSetValue());
    }
  }

  @Test
  void testAllianceStationId() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    EnumCallback callback = new EnumCallback();

    AllianceStationID allianceStation = AllianceStationID.BLUE_2;
    DriverStationSim.setAllianceStationId(allianceStation);

    try (CallbackStore cb = DriverStationSim.registerAllianceStationIdCallback(callback, false)) {
      // Unknown
      allianceStation = AllianceStationID.UNKNOWN;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertFalse(MatchState.getAlliance().isPresent());
      assertFalse(MatchState.getLocation().isPresent());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // B1
      allianceStation = AllianceStationID.BLUE_1;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(Alliance.BLUE, MatchState.getAlliance().get());
      assertEquals(1, MatchState.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // B2
      allianceStation = AllianceStationID.BLUE_2;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(Alliance.BLUE, MatchState.getAlliance().get());
      assertEquals(2, MatchState.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // B3
      allianceStation = AllianceStationID.BLUE_3;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(Alliance.BLUE, MatchState.getAlliance().get());
      assertEquals(3, MatchState.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // R1
      allianceStation = AllianceStationID.RED_1;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(Alliance.RED, MatchState.getAlliance().get());
      assertEquals(1, MatchState.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // R2
      allianceStation = AllianceStationID.RED_2;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(Alliance.RED, MatchState.getAlliance().get());
      assertEquals(2, MatchState.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // R3
      allianceStation = AllianceStationID.RED_3;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(Alliance.RED, MatchState.getAlliance().get());
      assertEquals(3, MatchState.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());
    }
  }

  @ParameterizedTest
  @EnumSource(MatchType.class)
  void testMatchType(MatchType matchType) {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setMatchType(matchType);
    DriverStationSim.notifyNewData();
    assertEquals(matchType, MatchState.getMatchType());
  }

  @Test
  void testReplayNumber() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setReplayNumber(4);
    DriverStationSim.notifyNewData();
    assertEquals(4, MatchState.getReplayNumber());
  }

  @Test
  void testMatchNumber() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setMatchNumber(3);
    DriverStationSim.notifyNewData();
    assertEquals(3, MatchState.getMatchNumber());
  }

  @Test
  void testMatchTime() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DoubleCallback callback = new DoubleCallback();
    try (CallbackStore cb = DriverStationSim.registerMatchTimeCallback(callback, false)) {
      final double testTime = 19.174;
      DriverStationSim.setMatchTime(testTime);
      DriverStationSim.notifyNewData();
      assertEquals(testTime, MatchState.getMatchTime());
      assertEquals(testTime, MatchState.getMatchTime());
      assertTrue(callback.wasTriggered());
      assertEquals(testTime, callback.getSetValue());
    }
  }

  @Test
  void testSetGameData() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    final String message = "Hello";
    DriverStationSim.setGameData(message);
    DriverStationSim.notifyNewData();
    var gameData = MatchState.getGameData();
    assertTrue(gameData.isPresent());
    assertEquals(message, gameData.get());
  }

  @Test
  void testSetGameDataEmpty() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setGameData("");
    DriverStationSim.notifyNewData();
    assertTrue(MatchState.getGameData().isEmpty());
  }

  @Test
  void testSetGameDataNull() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setGameData(null);
    DriverStationSim.notifyNewData();
    assertTrue(MatchState.getGameData().isEmpty());
  }

  @Test
  void testSetEventName() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    final String message = "The Best Event";
    DriverStationSim.setEventName(message);
    DriverStationSim.notifyNewData();
    assertEquals(message, MatchState.getEventName());
  }
}
