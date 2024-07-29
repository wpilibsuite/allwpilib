// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.params.provider.Arguments.arguments;

import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import java.util.stream.Stream;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class DriverStationTest {
  @ParameterizedTest
  @MethodSource("isConnectedProvider")
  void testIsConnected(int axisCount, int buttonCount, int povCount, boolean expected) {
    DriverStationSim.setJoystickAxisCount(1, axisCount);
    DriverStationSim.setJoystickButtonCount(1, buttonCount);
    DriverStationSim.setJoystickPOVCount(1, povCount);

    DriverStationSim.notifyNewData();

    assertEquals(expected, DriverStation.isJoystickConnected(1));
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
}
