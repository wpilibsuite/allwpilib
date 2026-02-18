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
import org.wpilib.driverstation.backend.DriverStationBackend;
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

    assertFalse(DriverStationBackend.isEnabled());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerEnabledCallback(callback, false)) {
      DriverStationSim.setEnabled(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getEnabled());
      assertTrue(DriverStationBackend.isEnabled());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testAutonomous() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(DriverStationBackend.isAutonomous());
    EnumCallback callback = new EnumCallback();
    try (CallbackStore cb = DriverStationSim.registerRobotModeCallback(callback, false)) {
      DriverStationSim.setRobotMode(RobotMode.AUTONOMOUS);
      DriverStationSim.notifyNewData();
      assertEquals(RobotMode.AUTONOMOUS, DriverStationSim.getRobotMode());
      assertTrue(DriverStationBackend.isAutonomous());
      assertEquals(RobotMode.AUTONOMOUS, DriverStationBackend.getRobotMode());
      assertTrue(callback.wasTriggered());
      assertEquals(RobotMode.AUTONOMOUS.getValue(), callback.getSetValue());
    }
  }

  @Test
  void testTest() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(DriverStationBackend.isTest());
    EnumCallback callback = new EnumCallback();
    try (CallbackStore cb = DriverStationSim.registerRobotModeCallback(callback, false)) {
      DriverStationSim.setRobotMode(RobotMode.TEST);
      DriverStationSim.notifyNewData();
      assertEquals(RobotMode.TEST, DriverStationSim.getRobotMode());
      assertTrue(DriverStationBackend.isTest());
      assertEquals(RobotMode.TEST, DriverStationBackend.getRobotMode());
      assertTrue(callback.wasTriggered());
      assertEquals(RobotMode.TEST.getValue(), callback.getSetValue());
    }
  }

  @Test
  void testEstop() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(DriverStationBackend.isEStopped());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerEStopCallback(callback, false)) {
      DriverStationSim.setEStop(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getEStop());
      assertTrue(DriverStationBackend.isEStopped());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testFmsAttached() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(DriverStationBackend.isFMSAttached());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerFmsAttachedCallback(callback, false)) {
      DriverStationSim.setFmsAttached(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getFmsAttached());
      assertTrue(DriverStationBackend.isFMSAttached());
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
    assertFalse(DriverStationBackend.isDSAttached());
    DriverStationSim.notifyNewData();
    assertTrue(DriverStationSim.getDsAttached());
    assertTrue(DriverStationBackend.isDSAttached());

    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerDsAttachedCallback(callback, false)) {
      DriverStationSim.setDsAttached(false);
      DriverStationBackend.refreshData();
      assertFalse(DriverStationSim.getDsAttached());
      assertFalse(DriverStationBackend.isDSAttached());
      assertTrue(callback.wasTriggered());
      assertFalse(callback.getSetValue());
    }
  }

  @Test
  void testAllianceStationId() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    EnumCallback callback = new EnumCallback();

    AllianceStationID allianceStation = AllianceStationID.Blue2;
    DriverStationSim.setAllianceStationId(allianceStation);

    try (CallbackStore cb = DriverStationSim.registerAllianceStationIdCallback(callback, false)) {
      // Unknown
      allianceStation = AllianceStationID.Unknown;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertFalse(DriverStationBackend.getAlliance().isPresent());
      assertFalse(DriverStationBackend.getLocation().isPresent());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // B1
      allianceStation = AllianceStationID.Blue1;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStationBackend.Alliance.Blue, DriverStationBackend.getAlliance().get());
      assertEquals(1, DriverStationBackend.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // B2
      allianceStation = AllianceStationID.Blue2;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStationBackend.Alliance.Blue, DriverStationBackend.getAlliance().get());
      assertEquals(2, DriverStationBackend.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // B3
      allianceStation = AllianceStationID.Blue3;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStationBackend.Alliance.Blue, DriverStationBackend.getAlliance().get());
      assertEquals(3, DriverStationBackend.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // R1
      allianceStation = AllianceStationID.Red1;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStationBackend.Alliance.Red, DriverStationBackend.getAlliance().get());
      assertEquals(1, DriverStationBackend.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // R2
      allianceStation = AllianceStationID.Red2;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStationBackend.Alliance.Red, DriverStationBackend.getAlliance().get());
      assertEquals(2, DriverStationBackend.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // R3
      allianceStation = AllianceStationID.Red3;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStationBackend.Alliance.Red, DriverStationBackend.getAlliance().get());
      assertEquals(3, DriverStationBackend.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());
    }
  }

  @ParameterizedTest
  @EnumSource(DriverStationBackend.MatchType.class)
  void testMatchType(DriverStationBackend.MatchType matchType) {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setMatchType(matchType);
    DriverStationSim.notifyNewData();
    assertEquals(matchType, DriverStationBackend.getMatchType());
  }

  @Test
  void testReplayNumber() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setReplayNumber(4);
    DriverStationSim.notifyNewData();
    assertEquals(4, DriverStationBackend.getReplayNumber());
  }

  @Test
  void testMatchNumber() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setMatchNumber(3);
    DriverStationSim.notifyNewData();
    assertEquals(3, DriverStationBackend.getMatchNumber());
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
      assertEquals(testTime, DriverStationSim.getMatchTime());
      assertEquals(testTime, DriverStationBackend.getMatchTime());
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
    var gameData = DriverStationBackend.getGameData();
    assertTrue(gameData.isPresent());
    assertEquals(message, gameData.get());
  }

  @Test
  void testSetGameDataEmpty() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setGameData("");
    DriverStationSim.notifyNewData();
    assertTrue(DriverStationBackend.getGameData().isEmpty());
  }

  @Test
  void testSetGameDataNull() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setGameData(null);
    DriverStationSim.notifyNewData();
    assertTrue(DriverStationBackend.getGameData().isEmpty());
  }

  @Test
  void testSetEventName() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    final String message = "The Best Event";
    DriverStationSim.setEventName(message);
    DriverStationSim.notifyNewData();
    assertEquals(message, DriverStationBackend.getEventName());
  }
}
