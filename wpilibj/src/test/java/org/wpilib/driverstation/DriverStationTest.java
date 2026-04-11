// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.params.provider.Arguments.arguments;

import java.util.stream.Stream;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.wpilib.simulation.DriverStationSim;

class DriverStationTest {
  @ParameterizedTest
  @MethodSource("isConnectedProvider")
  void testIsConnected(int axisCount, int buttonCount, int povCount, boolean expected) {
    DriverStationSim.setJoystickAxesMaximumIndex(1, axisCount);
    DriverStationSim.setJoystickButtonsMaximumIndex(1, buttonCount);
    DriverStationSim.setJoystickPOVsMaximumIndex(1, povCount);

    DriverStationSim.notifyNewData();

    assertEquals(expected, DriverStation.isJoystickConnected(1));
  }

  @Test
  void getOpmodeIdReturnsZeroUntilUserProgramStarts() {
    DriverStationSim.setOpMode(0x1234);
    DriverStationSim.notifyNewData();
    assertEquals(0, DriverStation.getOpModeId());

    DriverStation.observeUserProgramStarting();
    // need to manually mask because the upper eight bits include robot mode information
    assertEquals(0x1234, DriverStation.getOpModeId() & 0xFFFF);
  }

  @Test
  void getOpmodeReturnsEmptyStringUntilUserProgramStarts() {
    DriverStationSim.setOpMode(0x1234);
    DriverStationSim.notifyNewData();
    assertEquals("", DriverStation.getOpMode());

    DriverStation.observeUserProgramStarting();
    // in Sim, the opmode string is just the stringified version of the opmode i64 "<0000...0000>"
    // we need to parse the string to get the
    // need to manually mask because the upper eight bits include robot mode information
    String opmodeName = DriverStation.getOpMode();
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

  @MethodSource("connectionWarningProvider")
  void testConnectionWarnings(boolean fms, boolean silence, boolean expected) {
    DriverStationSim.setFmsAttached(fms);
    DriverStationSim.notifyNewData();

    DriverStation.silenceJoystickConnectionWarning(silence);
    assertEquals(expected, DriverStation.isJoystickConnectionWarningSilenced());
  }

  static Stream<Arguments> connectionWarningProvider() {
    return Stream.of(
        arguments(false, true, true),
        arguments(false, false, false),
        arguments(true, true, false),
        arguments(true, false, false));
  }

  @AfterEach
  @SuppressWarnings("PMD.AvoidAccessibilityAlteration")
  void resetUserProgramFlag() throws ReflectiveOperationException {
    var field = DriverStation.class.getDeclaredField("m_userProgramStarted");
    field.setAccessible(true);
    field.set(null, false);
  }
}
