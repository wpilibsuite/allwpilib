// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import edu.wpi.first.wpilibj.simulation.testutils.IntCallback;
import org.junit.jupiter.api.Test;

class RoboRioSimTest {
  @Test
  void testSetVin() {
    RoboRioSim.resetData();

    DoubleCallback voltageCallback = new DoubleCallback();
    try (CallbackStore voltageCb = RoboRioSim.registerVInVoltageCallback(voltageCallback, false)) {
      final double TEST_VOLTAGE = 1.91;

      RoboRioSim.setVInVoltage(TEST_VOLTAGE);
      assertTrue(voltageCallback.wasTriggered());
      assertEquals(TEST_VOLTAGE, voltageCallback.getSetValue());
      assertEquals(TEST_VOLTAGE, RoboRioSim.getVInVoltage());
      assertEquals(TEST_VOLTAGE, RobotController.getInputVoltage());
    }
  }

  @Test
  void testSetBrownout() {
    RoboRioSim.resetData();

    DoubleCallback voltageCallback = new DoubleCallback();
    try (CallbackStore voltageCb =
        RoboRioSim.registerBrownoutVoltageCallback(voltageCallback, false)) {
      final double TEST_VOLTAGE = 1.91;

      RoboRioSim.setBrownoutVoltage(TEST_VOLTAGE);
      assertTrue(voltageCallback.wasTriggered());
      assertEquals(TEST_VOLTAGE, voltageCallback.getSetValue());
      assertEquals(TEST_VOLTAGE, RoboRioSim.getBrownoutVoltage());
      assertEquals(TEST_VOLTAGE, RobotController.getBrownoutVoltage());
    }
  }

  @Test
  void test3V3() {
    RoboRioSim.resetData();

    DoubleCallback voltageCallback = new DoubleCallback();
    DoubleCallback currentCallback = new DoubleCallback();
    BooleanCallback activeCallback = new BooleanCallback();
    IntCallback faultCallback = new IntCallback();
    try (CallbackStore voltageCb =
            RoboRioSim.registerUserVoltage3V3Callback(voltageCallback, false);
        CallbackStore currentCb =
            RoboRioSim.registerUserCurrent3V3Callback(currentCallback, false);
        CallbackStore activeCb = RoboRioSim.registerUserActive3V3Callback(activeCallback, false);
        CallbackStore faultsCb = RoboRioSim.registerUserFaults3V3Callback(faultCallback, false)) {
      final double TEST_VOLTAGE = 22.9;
      final double TEST_CURRENT = 174;
      final int TEST_FAULTS = 229;

      RoboRioSim.setUserVoltage3V3(TEST_VOLTAGE);
      assertTrue(voltageCallback.wasTriggered());
      assertEquals(TEST_VOLTAGE, voltageCallback.getSetValue());
      assertEquals(TEST_VOLTAGE, RoboRioSim.getUserVoltage3V3());
      assertEquals(TEST_VOLTAGE, RobotController.getVoltage3V3());

      RoboRioSim.setUserCurrent3V3(TEST_CURRENT);
      assertTrue(currentCallback.wasTriggered());
      assertEquals(TEST_CURRENT, currentCallback.getSetValue());
      assertEquals(TEST_CURRENT, RoboRioSim.getUserCurrent3V3());
      assertEquals(TEST_CURRENT, RobotController.getCurrent3V3());

      RoboRioSim.setUserActive3V3(false);
      assertTrue(activeCallback.wasTriggered());
      assertFalse(activeCallback.getSetValue());
      assertFalse(RoboRioSim.getUserActive3V3());
      assertFalse(RobotController.getEnabled3V3());

      RoboRioSim.setUserFaults3V3(TEST_FAULTS);
      assertTrue(faultCallback.wasTriggered());
      assertEquals(TEST_FAULTS, faultCallback.getSetValue());
      assertEquals(TEST_FAULTS, RoboRioSim.getUserFaults3V3());
      assertEquals(TEST_FAULTS, RobotController.getFaultCount3V3());
    }
  }

  @Test
  void testCPUTemp() {
    RoboRioSim.resetData();

    DoubleCallback callback = new DoubleCallback();

    try (CallbackStore cb = RoboRioSim.registerCPUTempCallback(callback, false)) {
      final double CPU_TEMP = 100.0;

      RoboRioSim.setCPUTemp(CPU_TEMP);
      assertTrue(callback.wasTriggered());
      assertEquals(CPU_TEMP, callback.getSetValue());
      assertEquals(CPU_TEMP, RoboRioSim.getCPUTemp());
      assertEquals(CPU_TEMP, RobotController.getCPUTemp());
    }
  }

  @Test
  void testTeamNumber() {
    RoboRioSim.resetData();

    IntCallback callback = new IntCallback();

    try (CallbackStore cb = RoboRioSim.registerTeamNumberCallback(callback, false)) {
      final int TEAM_NUMBER = 9999;

      RoboRioSim.setTeamNumber(TEAM_NUMBER);
      assertTrue(callback.wasTriggered());
      assertEquals(TEAM_NUMBER, callback.getSetValue());
      assertEquals(TEAM_NUMBER, RoboRioSim.getTeamNumber());
      assertEquals(TEAM_NUMBER, RobotController.getTeamNumber());
    }
  }

  @Test
  void testSerialNumber() {
    RoboRioSim.resetData();

    final String SERIAL_NUMBER = "Hello";

    RoboRioSim.setSerialNumber(SERIAL_NUMBER);
    assertEquals(SERIAL_NUMBER, RoboRioSim.getSerialNumber());
    assertEquals(SERIAL_NUMBER, RobotController.getSerialNumber());

    // Make sure it truncates at 8 characters properly
    final String SERIAL_NUMBER_OVERFLOW = "SerialNumber";
    final String SERIAL_NUMBER_TRUNCATED = SERIAL_NUMBER_OVERFLOW.substring(0, 8);
    RoboRioSim.setSerialNumber(SERIAL_NUMBER_OVERFLOW);
    assertEquals(SERIAL_NUMBER_TRUNCATED, RoboRioSim.getSerialNumber());
    assertEquals(SERIAL_NUMBER_TRUNCATED, RobotController.getSerialNumber());
  }

  @Test
  void testComments() {
    RoboRioSim.resetData();

    final String COMMENTS = "Hello! These are comments in the roboRIO web interface!";

    RoboRioSim.setComments(COMMENTS);
    assertEquals(COMMENTS, RoboRioSim.getComments());
    assertEquals(COMMENTS, RobotController.getComments());

    final String COMMENTS_OVERFLOW =
        "Hello! These are comments in the roboRIO web interface!"
            + " This comment exceeds 64 characters!";
    final String COMMENTS_TRUNCATED = COMMENTS_OVERFLOW.substring(0, 64);
    RoboRioSim.setComments(COMMENTS_OVERFLOW);
    assertEquals(COMMENTS_TRUNCATED, RoboRioSim.getComments());
    assertEquals(COMMENTS_TRUNCATED, RobotController.getComments());
  }
}
