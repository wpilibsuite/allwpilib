// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.util;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.UtilityClassTest;
import org.junit.jupiter.api.Test;

class UnitsTest extends UtilityClassTest<Units> {
  UnitsTest() {
    super(Units.class);
  }

  @Test
  void metersToFeetTest() {
    assertEquals(3.28, Units.metersToFeet(1), 1e-2);
  }

  @Test
  void feetToMetersTest() {
    assertEquals(0.30, Units.feetToMeters(1), 1e-2);
  }

  @Test
  void metersToInchesTest() {
    assertEquals(39.37, Units.metersToInches(1), 1e-2);
  }

  @Test
  void inchesToMetersTest() {
    assertEquals(0.0254, Units.inchesToMeters(1), 1e-3);
  }

  @Test
  void degreesToRadiansTest() {
    assertEquals(0.017, Units.degreesToRadians(1), 1e-3);
  }

  @Test
  void radiansToDegreesTest() {
    assertEquals(114.59, Units.radiansToDegrees(2), 1e-2);
  }

  @Test
  void rotationsPerMinuteToRadiansPerSecondTest() {
    assertEquals(6.28, Units.rotationsPerMinuteToRadiansPerSecond(60), 1e-2);
  }

  @Test
  void radiansPerSecondToRotationsPerMinute() {
    assertEquals(76.39, Units.radiansPerSecondToRotationsPerMinute(8), 1e-2);
  }

  @Test
  void millisecondsToSeconds() {
    assertEquals(0.5, Units.millisecondsToSeconds(500), 1e-2);
  }

  @Test
  void secondsToMilliseconds() {
    assertEquals(1500, Units.secondsToMilliseconds(1.5), 1e-2);
  }

  @Test
  void kilogramsToLbsTest() {
    assertEquals(2.20462, Units.kilogramsToLbs(1), 1e-2);
  }

  @Test
  void lbsToKilogramsTest() {
    assertEquals(0.453592, Units.lbsToKilograms(1), 1e-2);
  }
}
