// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HALUtil;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import edu.wpi.first.wpilibj.simulation.testutils.EnumCallback;
import edu.wpi.first.wpilibj.simulation.testutils.IntCallback;
import org.junit.jupiter.api.Test;

class RoboRioSimTest {
  @Test
  void testSetVin() {
    RoboRioSim.resetData();

    DoubleCallback voltageCallback = new DoubleCallback();
    try (CallbackStore voltageCb = RoboRioSim.registerVInVoltageCallback(voltageCallback, false)) {
      final double kTestVoltage = 1.91;

      RoboRioSim.setVInVoltage(kTestVoltage);
      assertTrue(voltageCallback.wasTriggered());
      assertEquals(kTestVoltage, voltageCallback.getSetValue());
      assertEquals(kTestVoltage, RoboRioSim.getVInVoltage());
      assertEquals(kTestVoltage, RobotController.getInputVoltage());
    }
  }

  @Test
  void testSetBrownout() {
    RoboRioSim.resetData();

    DoubleCallback voltageCallback = new DoubleCallback();
    try (CallbackStore voltageCb =
        RoboRioSim.registerBrownoutVoltageCallback(voltageCallback, false)) {
      final double kTestVoltage = 1.91;

      RoboRioSim.setBrownoutVoltage(kTestVoltage);
      assertTrue(voltageCallback.wasTriggered());
      assertEquals(kTestVoltage, voltageCallback.getSetValue());
      assertEquals(kTestVoltage, RoboRioSim.getBrownoutVoltage());
      assertEquals(kTestVoltage, RobotController.getBrownoutVoltage());
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
      final double kTestVoltage = 22.9;
      final double kTestCurrent = 174;
      final int kTestFaults = 229;

      RoboRioSim.setUserVoltage3V3(kTestVoltage);
      assertTrue(voltageCallback.wasTriggered());
      assertEquals(kTestVoltage, voltageCallback.getSetValue());
      assertEquals(kTestVoltage, RoboRioSim.getUserVoltage3V3());
      assertEquals(kTestVoltage, RobotController.getVoltage3V3());

      RoboRioSim.setUserCurrent3V3(kTestCurrent);
      assertTrue(currentCallback.wasTriggered());
      assertEquals(kTestCurrent, currentCallback.getSetValue());
      assertEquals(kTestCurrent, RoboRioSim.getUserCurrent3V3());
      assertEquals(kTestCurrent, RobotController.getCurrent3V3());

      RoboRioSim.setUserActive3V3(false);
      assertTrue(activeCallback.wasTriggered());
      assertFalse(activeCallback.getSetValue());
      assertFalse(RoboRioSim.getUserActive3V3());
      assertFalse(RobotController.getEnabled3V3());

      RoboRioSim.setUserFaults3V3(kTestFaults);
      assertTrue(faultCallback.wasTriggered());
      assertEquals(kTestFaults, faultCallback.getSetValue());
      assertEquals(kTestFaults, RoboRioSim.getUserFaults3V3());
      assertEquals(kTestFaults, RobotController.getFaultCount3V3());
    }
  }

  @Test
  void testCPUTemp() {
    RoboRioSim.resetData();

    DoubleCallback callback = new DoubleCallback();

    try (CallbackStore cb = RoboRioSim.registerCPUTempCallback(callback, false)) {
      final double kCPUTemp = 100.0;

      RoboRioSim.setCPUTemp(kCPUTemp);
      assertTrue(callback.wasTriggered());
      assertEquals(kCPUTemp, callback.getSetValue());
      assertEquals(kCPUTemp, RoboRioSim.getCPUTemp());
      assertEquals(kCPUTemp, RobotController.getCPUTemp());
    }
  }

  @Test
  void testTeamNumber() {
    RoboRioSim.resetData();

    IntCallback callback = new IntCallback();

    try (CallbackStore cb = RoboRioSim.registerTeamNumberCallback(callback, false)) {
      final int kTeamNumber = 9999;

      RoboRioSim.setTeamNumber(kTeamNumber);
      assertTrue(callback.wasTriggered());
      assertEquals(kTeamNumber, callback.getSetValue());
      assertEquals(kTeamNumber, RoboRioSim.getTeamNumber());
      assertEquals(kTeamNumber, RobotController.getTeamNumber());
    }
  }

  @Test
  void testSerialNumber() {
    RoboRioSim.resetData();

    final String kSerialNumber = "Hello";

    RoboRioSim.setSerialNumber(kSerialNumber);
    assertEquals(kSerialNumber, RoboRioSim.getSerialNumber());
    assertEquals(kSerialNumber, RobotController.getSerialNumber());

    // Make sure it truncates at 8 characters properly
    final String kSerialNumberOverflow = "SerialNumber";
    final String kSerialNumberTruncated = kSerialNumberOverflow.substring(0, 8);
    RoboRioSim.setSerialNumber(kSerialNumberOverflow);
    assertEquals(kSerialNumberTruncated, RoboRioSim.getSerialNumber());
    assertEquals(kSerialNumberTruncated, RobotController.getSerialNumber());
  }

  @Test
  void testComments() {
    RoboRioSim.resetData();

    final String kComments = "Hello! These are comments in the roboRIO web interface!";

    RoboRioSim.setComments(kComments);
    assertEquals(kComments, RoboRioSim.getComments());
    assertEquals(kComments, RobotController.getComments());

    final String kCommentsOverflow =
        "Hello! These are comments in the roboRIO web interface!"
            + " This comment exceeds 64 characters!";
    final String kCommentsTruncated = kCommentsOverflow.substring(0, 64);
    RoboRioSim.setComments(kCommentsOverflow);
    assertEquals(kCommentsTruncated, RoboRioSim.getComments());
    assertEquals(kCommentsTruncated, RobotController.getComments());
  }

  @Test
  void testRadioLEDState() {
    RoboRioSim.resetData();

    EnumCallback callback = new EnumCallback();
    try (CallbackStore cb = RoboRioSim.registerRadioLEDStateCallback(callback, false)) {
      RobotController.setRadioLEDState(RadioLEDState.kGreen);
      assertTrue(callback.wasTriggered());
      assertEquals(RadioLEDState.kGreen.value, callback.getSetValue());
      assertEquals(RadioLEDState.kGreen, RoboRioSim.getRadioLEDState());
      assertEquals(RadioLEDState.kGreen, RobotController.getRadioLEDState());

      callback.reset();

      RoboRioSim.setRadioLEDState(RadioLEDState.kOrange);
      assertTrue(callback.wasTriggered());
      assertEquals(RadioLEDState.kOrange.value, callback.getSetValue());
      assertEquals(RadioLEDState.kOrange, RoboRioSim.getRadioLEDState());
      assertEquals(RadioLEDState.kOrange, RobotController.getRadioLEDState());
    }
  }
}
