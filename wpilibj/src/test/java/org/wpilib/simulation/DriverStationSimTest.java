// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.AllianceStationID;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import edu.wpi.first.wpilibj.simulation.testutils.EnumCallback;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;

class DriverStationSimTest {
  @Test
  void testEnabled() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(DriverStation.isEnabled());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerEnabledCallback(callback, false)) {
      DriverStationSim.setEnabled(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getEnabled());
      assertTrue(DriverStation.isEnabled());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testAutonomous() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(DriverStation.isAutonomous());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerAutonomousCallback(callback, false)) {
      DriverStationSim.setAutonomous(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getAutonomous());
      assertTrue(DriverStation.isAutonomous());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testTest() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(DriverStation.isTest());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerTestCallback(callback, false)) {
      DriverStationSim.setTest(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getTest());
      assertTrue(DriverStation.isTest());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testEstop() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(DriverStation.isEStopped());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerEStopCallback(callback, false)) {
      DriverStationSim.setEStop(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getEStop());
      assertTrue(DriverStation.isEStopped());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testFmsAttached() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    assertFalse(DriverStation.isFMSAttached());
    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerFmsAttachedCallback(callback, false)) {
      DriverStationSim.setFmsAttached(true);
      DriverStationSim.notifyNewData();
      assertTrue(DriverStationSim.getFmsAttached());
      assertTrue(DriverStation.isFMSAttached());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testDsAttached() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();
    DriverStation.refreshData();

    assertFalse(DriverStationSim.getDsAttached());
    assertFalse(DriverStation.isDSAttached());
    DriverStationSim.notifyNewData();
    assertTrue(DriverStationSim.getDsAttached());
    assertTrue(DriverStation.isDSAttached());

    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = DriverStationSim.registerDsAttachedCallback(callback, false)) {
      DriverStationSim.setDsAttached(false);
      DriverStation.refreshData();
      assertFalse(DriverStationSim.getDsAttached());
      assertFalse(DriverStation.isDSAttached());
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
      assertFalse(DriverStation.getAlliance().isPresent());
      assertFalse(DriverStation.getLocation().isPresent());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // B1
      allianceStation = AllianceStationID.Blue1;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStation.Alliance.Blue, DriverStation.getAlliance().get());
      assertEquals(1, DriverStation.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // B2
      allianceStation = AllianceStationID.Blue2;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStation.Alliance.Blue, DriverStation.getAlliance().get());
      assertEquals(2, DriverStation.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // B3
      allianceStation = AllianceStationID.Blue3;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStation.Alliance.Blue, DriverStation.getAlliance().get());
      assertEquals(3, DriverStation.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // R1
      allianceStation = AllianceStationID.Red1;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStation.Alliance.Red, DriverStation.getAlliance().get());
      assertEquals(1, DriverStation.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // R2
      allianceStation = AllianceStationID.Red2;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStation.Alliance.Red, DriverStation.getAlliance().get());
      assertEquals(2, DriverStation.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());

      // R3
      allianceStation = AllianceStationID.Red3;
      DriverStationSim.setAllianceStationId(allianceStation);
      DriverStationSim.notifyNewData();
      assertEquals(allianceStation, DriverStationSim.getAllianceStationId());
      assertEquals(DriverStation.Alliance.Red, DriverStation.getAlliance().get());
      assertEquals(3, DriverStation.getLocation().getAsInt());
      assertTrue(callback.wasTriggered());
      assertEquals(allianceStation.ordinal(), callback.getSetValue());
    }
  }

  @ParameterizedTest
  @EnumSource(DriverStation.MatchType.class)
  void testMatchType(DriverStation.MatchType matchType) {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setMatchType(matchType);
    DriverStationSim.notifyNewData();
    assertEquals(matchType, DriverStation.getMatchType());
  }

  @Test
  void testReplayNumber() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setReplayNumber(4);
    DriverStationSim.notifyNewData();
    assertEquals(4, DriverStation.getReplayNumber());
  }

  @Test
  void testMatchNumber() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    DriverStationSim.setMatchNumber(3);
    DriverStationSim.notifyNewData();
    assertEquals(3, DriverStation.getMatchNumber());
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
      assertEquals(testTime, DriverStation.getMatchTime());
      assertTrue(callback.wasTriggered());
      assertEquals(testTime, callback.getSetValue());
    }
  }

  @Test
  void testSetGameSpecificMessage() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    final String message = "Hello World!";
    DriverStationSim.setGameSpecificMessage(message);
    DriverStationSim.notifyNewData();
    assertEquals(message, DriverStation.getGameSpecificMessage());
  }

  @Test
  void testSetEventName() {
    HAL.initialize(500, 0);
    DriverStationSim.resetData();

    final String message = "The Best Event";
    DriverStationSim.setEventName(message);
    DriverStationSim.notifyNewData();
    assertEquals(message, DriverStation.getEventName());
  }
}
