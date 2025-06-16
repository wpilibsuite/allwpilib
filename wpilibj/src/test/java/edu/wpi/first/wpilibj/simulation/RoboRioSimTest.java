// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HALUtil;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.RobotController.RadioLEDState;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import edu.wpi.first.wpilibj.simulation.testutils.EnumCallback;
import edu.wpi.first.wpilibj.simulation.testutils.IntCallback;
import org.junit.jupiter.api.Test;

class RoboRioSimTest {
  @Test
  void testFPGAButton() {
    RoboRioSim.resetData();

    BooleanCallback callback = new BooleanCallback();
    try (CallbackStore cb = RoboRioSim.registerFPGAButtonCallback(callback, false)) {
      RoboRioSim.setFPGAButton(true);
      assertTrue(RoboRioSim.getFPGAButton());
      assertTrue(HALUtil.getFPGAButton());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());

      callback.reset();
      RoboRioSim.setFPGAButton(false);
      assertFalse(RoboRioSim.getFPGAButton());
      assertFalse(HALUtil.getFPGAButton());
      assertTrue(callback.wasTriggered());
      assertFalse(callback.getSetValue());
    }
  }

  @Test
  void testSetVin() {
    RoboRioSim.resetData();

    DoubleCallback voltageCallback = new DoubleCallback();
    DoubleCallback currentCallback = new DoubleCallback();
    try (CallbackStore voltageCb = RoboRioSim.registerVInVoltageCallback(voltageCallback, false);
        CallbackStore currentCb = RoboRioSim.registerVInCurrentCallback(currentCallback, false)) {
      final double kTestVoltage = 1.91;
      final double kTestCurrent = 35.04;

      RoboRioSim.setVInVoltage(kTestVoltage);
      assertTrue(voltageCallback.wasTriggered());
      assertEquals(kTestVoltage, voltageCallback.getSetValue());
      assertEquals(kTestVoltage, RoboRioSim.getVInVoltage());
      assertEquals(kTestVoltage, RobotController.getInputVoltage());

      RoboRioSim.setVInCurrent(kTestCurrent);
      assertTrue(currentCallback.wasTriggered());
      assertEquals(kTestCurrent, currentCallback.getSetValue());
      assertEquals(kTestCurrent, RoboRioSim.getVInCurrent());
      assertEquals(kTestCurrent, RobotController.getInputCurrent());
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
  void test6V() {
    RoboRioSim.resetData();

    DoubleCallback voltageCallback = new DoubleCallback();
    DoubleCallback currentCallback = new DoubleCallback();
    BooleanCallback activeCallback = new BooleanCallback();
    IntCallback faultCallback = new IntCallback();
    try (CallbackStore voltageCb =
            RoboRioSim.registerUserVoltage6VCallback(voltageCallback, false);
        CallbackStore currentCb = RoboRioSim.registerUserCurrent6VCallback(currentCallback, false);
        CallbackStore activeCb = RoboRioSim.registerUserActive6VCallback(activeCallback, false);
        CallbackStore faultsCb = RoboRioSim.registerUserFaults6VCallback(faultCallback, false)) {
      final double kTestVoltage = 22.9;
      final double kTestCurrent = 174;
      final int kTestFaults = 229;

      RoboRioSim.setUserVoltage6V(kTestVoltage);
      assertTrue(voltageCallback.wasTriggered());
      assertEquals(kTestVoltage, voltageCallback.getSetValue());
      assertEquals(kTestVoltage, RoboRioSim.getUserVoltage6V());
      assertEquals(kTestVoltage, RobotController.getVoltage6V());

      RoboRioSim.setUserCurrent6V(kTestCurrent);
      assertTrue(currentCallback.wasTriggered());
      assertEquals(kTestCurrent, currentCallback.getSetValue());
      assertEquals(kTestCurrent, RoboRioSim.getUserCurrent6V());
      assertEquals(kTestCurrent, RobotController.getCurrent6V());

      RoboRioSim.setUserActive6V(false);
      assertTrue(activeCallback.wasTriggered());
      assertFalse(activeCallback.getSetValue());
      assertFalse(RoboRioSim.getUserActive6V());
      assertFalse(RobotController.getEnabled6V());

      RoboRioSim.setUserFaults6V(kTestFaults);
      assertTrue(faultCallback.wasTriggered());
      assertEquals(kTestFaults, faultCallback.getSetValue());
      assertEquals(kTestFaults, RoboRioSim.getUserFaults6V());
      assertEquals(kTestFaults, RobotController.getFaultCount6V());
    }
  }

  @Test
  void test5V() {
    RoboRioSim.resetData();

    DoubleCallback voltageCallback = new DoubleCallback();
    DoubleCallback currentCallback = new DoubleCallback();
    BooleanCallback activeCallback = new BooleanCallback();
    IntCallback faultCallback = new IntCallback();
    try (CallbackStore voltageCb =
            RoboRioSim.registerUserVoltage5VCallback(voltageCallback, false);
        CallbackStore currentCb = RoboRioSim.registerUserCurrent5VCallback(currentCallback, false);
        CallbackStore activeCb = RoboRioSim.registerUserActive5VCallback(activeCallback, false);
        CallbackStore faultsCb = RoboRioSim.registerUserFaults5VCallback(faultCallback, false)) {
      final double kTestVoltage = 22.9;
      final double kTestCurrent = 174;
      final int kTestFaults = 229;

      RoboRioSim.setUserVoltage5V(kTestVoltage);
      assertTrue(voltageCallback.wasTriggered());
      assertEquals(kTestVoltage, voltageCallback.getSetValue());
      assertEquals(kTestVoltage, RoboRioSim.getUserVoltage5V());
      assertEquals(kTestVoltage, RobotController.getVoltage5V());

      RoboRioSim.setUserCurrent5V(kTestCurrent);
      assertTrue(currentCallback.wasTriggered());
      assertEquals(kTestCurrent, currentCallback.getSetValue());
      assertEquals(kTestCurrent, RoboRioSim.getUserCurrent5V());
      assertEquals(kTestCurrent, RobotController.getCurrent5V());

      RoboRioSim.setUserActive5V(false);
      assertTrue(activeCallback.wasTriggered());
      assertFalse(activeCallback.getSetValue());
      assertFalse(RoboRioSim.getUserActive5V());
      assertFalse(RobotController.getEnabled5V());

      RoboRioSim.setUserFaults5V(kTestFaults);
      assertTrue(faultCallback.wasTriggered());
      assertEquals(kTestFaults, faultCallback.getSetValue());
      assertEquals(kTestFaults, RoboRioSim.getUserFaults5V());
      assertEquals(kTestFaults, RobotController.getFaultCount5V());
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

    final String kCommentsOverflow = "Hello! These are comments in the roboRIO web interface!"
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
