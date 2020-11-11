/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.stream.Stream;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import edu.wpi.first.wpilibj.simulation.DriverStationSim;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.params.provider.Arguments.arguments;

class DriverStationTest {
  @ParameterizedTest
  @MethodSource("isConnectedProvider")
  void testIsConnected(int axisCount, int buttonCount, int povCount, boolean expected) {
    DriverStationSim.setJoystickAxisCount(1, axisCount);
    DriverStationSim.setJoystickButtonCount(1, buttonCount);
    DriverStationSim.setJoystickPOVCount(1, povCount);

    DriverStationSim.notifyNewData();

    assertEquals(expected, DriverStation.getInstance().isJoystickConnected(1));
  }

  static Stream<Arguments> isConnectedProvider() {
    return Stream.of(
      arguments(0, 0, 0, false),
      arguments(1, 0, 0, true),
      arguments(0, 1, 0, true),
      arguments(0, 0, 1, true),
      arguments(1, 1, 1, true),
      arguments(4, 10, 1, true)
    );
  }

  @MethodSource("connectionWarningProvider")
  void testConnectionWarnings(boolean fms, boolean silence, boolean expected) {
    DriverStationSim.setFmsAttached(fms);
    DriverStationSim.notifyNewData();

    DriverStation.getInstance().silenceJoystickConnectionWarning(silence);
    assertEquals(expected,
        DriverStation.getInstance().isJoystickConnectionWarningSilenced());
  }

  static Stream<Arguments> connectionWarningProvider() {
    return Stream.of(
      arguments(false, true, true),
      arguments(false, false, false),
      arguments(true, true, false),
      arguments(true, false, false)
    );
  }
}
