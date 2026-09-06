// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.params.provider.Arguments.arguments;

import java.util.Arrays;
import java.util.stream.Stream;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.wpilib.driverstation.internal.DriverStationBackend;
import org.wpilib.hardware.hal.simulation.DriverStationDataJNI;
import org.wpilib.simulation.AlertSim;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.SimHooks;
import org.wpilib.util.Alert;

class DriverStationTest {
  @ParameterizedTest
  @MethodSource("isConnectedProvider")
  void testIsConnected(int axisCount, int buttonCount, int povCount, boolean expected) {
    DriverStationSim.setJoystickAxesMaximumIndex(1, axisCount);
    DriverStationSim.setJoystickButtonsMaximumIndex(1, buttonCount);
    DriverStationSim.setJoystickPOVsMaximumIndex(1, povCount);

    DriverStationSim.notifyNewData();

    assertEquals(expected, DriverStationBackend.isJoystickConnected(1));
  }

  @Test
  void getOpmodeIdReturnsZeroUntilUserProgramStarts() {
    DriverStationSim.setOpMode(0x1234);
    DriverStationSim.notifyNewData();
    assertEquals(0, RobotState.getOpModeId());

    RobotState.observeUserProgramStarting();
    // need to manually mask because the upper eight bits include robot mode information
    assertEquals(0x1234, RobotState.getOpModeId() & 0xFFFF);
  }

  @Test
  void getOpmodeReturnsEmptyStringUntilUserProgramStarts() {
    DriverStationSim.setOpMode(0x1234);
    DriverStationSim.notifyNewData();
    assertEquals("", RobotState.getOpMode());

    RobotState.observeUserProgramStarting();
    // in Sim, the opmode string is just the stringified version of the opmode i64 "<0000...0000>"
    // we need to parse the string to get the
    // need to manually mask because the upper eight bits include robot mode information
    String opmodeName = RobotState.getOpMode();
    assertEquals(
        "0x1234",
        String.format(
            "0x%x", Long.parseLong(opmodeName.substring(1, opmodeName.length() - 1)) & 0xFFFF));
  }

  static Stream<Arguments> isConnectedProvider() {
    return Stream.of(
        arguments(0, 0, 0, false),
        arguments(1, 0, 0, true),
        arguments(0, 1, 0, true),
        arguments(0, 0, 1, true),
        arguments(1, 1, 1, true),
        arguments(4, 10, 1, true));
  }

  @ParameterizedTest
  @MethodSource("connectionAlertProvider")
  void testConnectionAlerts(boolean fms, boolean silence, boolean expected, boolean alertActive) {
    disconnectJoystick(0);
    DriverStationSim.setFmsAttached(fms);
    DriverStationSim.notifyNewData();

    DriverStationBackend.silenceJoystickConnectionAlert(silence);

    Joystick joystick = new Joystick(0);
    joystick.getRawButton(1);

    SimHooks.stepTiming(1.0);
    assertEquals(expected, DriverStationBackend.isJoystickConnectionAlertSilenced());

    assertEquals(alertActive, isJoystickDisconnectedAlertActive(0), DriverStationTest::alertList);
  }

  @Test
  void joystickResourceAlerts() {
    DriverStationSim.setJoystickButtonsAvailable(0, 1);
    DriverStationSim.setJoystickAxesAvailable(0, 1);
    DriverStationSim.setJoystickPOVsAvailable(0, 1);
    DriverStationDataJNI.setTouchpadCounts(0, 1, new int[] {1, 0});
    DriverStationSim.notifyNewData();

    DriverStationBackend.getStickButton(0, 1);
    DriverStationBackend.getStickButton(0, 2);
    DriverStationBackend.getStickAxis(0, 1);
    DriverStationBackend.getStickPOV(0, 1);
    DriverStationBackend.getStickTouchpadFinger(0, 0, 1);

    assertDriverStationAlertActive(
        "joystick0ButtonUnavailable",
        "Joystick Button 2 on port 0 not available",
        Alert.Level.MEDIUM);
    assertDriverStationAlertActive(
        "joystick0AxisUnavailable", "Joystick axis 1 on port 0 not available", Alert.Level.MEDIUM);
    assertDriverStationAlertActive(
        "joystick0POVUnavailable", "Joystick POV 1 on port 0 not available", Alert.Level.MEDIUM);
    assertDriverStationAlertActive(
        "joystick0TouchpadFingerUnavailable",
        "Joystick touchpad finger 1 on touchpad 0 on port 0 not available",
        Alert.Level.MEDIUM);
    assertFalse(isJoystickDisconnectedAlertActive(0));
  }

  @Test
  void joystickAlertCollisionDoesNotEscapeGetter() {
    AlertSim.resetData();
    DriverStationSim.setJoystickButtonsAvailable(0, 1);
    DriverStationSim.setJoystickAxesAvailable(0, 1);
    DriverStationSim.setJoystickPOVsAvailable(0, 1);
    DriverStationSim.notifyNewData();

    try (Alert collision =
        new Alert("DriverStation", "joystick0AxisUnavailable", "collision", Alert.Level.MEDIUM)) {
      assertEquals(0.0, DriverStationBackend.getStickAxis(0, 1));
      assertEquals(1, AlertSim.getAll().length, DriverStationTest::alertList);
    }

    assertEquals(0.0, DriverStationBackend.getStickAxis(0, 1));
    assertDriverStationAlertActive(
        "joystick0AxisUnavailable", "Joystick axis 1 on port 0 not available", Alert.Level.MEDIUM);
  }

  static Stream<Arguments> connectionAlertProvider() {
    return Stream.of(
        arguments(false, true, true, false),
        arguments(false, false, false, true),
        arguments(true, true, false, true),
        arguments(true, false, false, true));
  }

  private static boolean isJoystickDisconnectedAlertActive(int stick) {
    return isDriverStationAlertActive(
        "joystick" + stick + "Disconnected",
        "Joystick on port " + stick + " not available, check if controller is plugged in",
        Alert.Level.HIGH);
  }

  private static void assertDriverStationAlertActive(String id, String text, Alert.Level level) {
    assertTrue(isDriverStationAlertActive(id, text, level));
  }

  private static boolean isDriverStationAlertActive(String id, String text, Alert.Level level) {
    return Arrays.stream(AlertSim.getActive())
        .anyMatch(
            alert ->
                "DriverStation".equals(alert.group)
                    && alert.id.equals(id)
                    && alert.text.equals(text)
                    && alert.level == level);
  }

  private static String alertList() {
    return Arrays.stream(AlertSim.getAll())
        .map(
            alert ->
                alert.group
                    + "/"
                    + alert.id
                    + "/"
                    + alert.level
                    + "/"
                    + alert.isActive()
                    + "/"
                    + alert.text)
        .toList()
        .toString();
  }

  private static void disconnectJoystick(int stick) {
    DriverStationSim.setJoystickButtonsAvailable(stick, 0);
    DriverStationSim.setJoystickAxesAvailable(stick, 0);
    DriverStationSim.setJoystickPOVsAvailable(stick, 0);
  }

  @AfterEach
  @SuppressWarnings("PMD.AvoidAccessibilityAlteration")
  void resetUserProgramFlag() throws ReflectiveOperationException {
    disconnectJoystick(0);
    DriverStationSim.setFmsAttached(false);
    DriverStationSim.notifyNewData();
    DriverStationBackend.silenceJoystickConnectionAlert(true);
    AlertSim.resetData();
    DriverStationBackend.silenceJoystickConnectionAlert(false);
    assertFalse(isJoystickDisconnectedAlertActive(0));

    var field = RobotState.class.getDeclaredField("m_userProgramStarted");
    field.setAccessible(true);
    field.set(null, false);
  }
}
