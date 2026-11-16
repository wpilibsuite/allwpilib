// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.util;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.math.util.UnitConversions.degreesToRadians;
import static org.wpilib.math.util.UnitConversions.feetToMeters;
import static org.wpilib.math.util.UnitConversions.inchesToMeters;
import static org.wpilib.math.util.UnitConversions.kilogramsToLbs;
import static org.wpilib.math.util.UnitConversions.lbsToKilograms;
import static org.wpilib.math.util.UnitConversions.metersPerSecondToMilesPerHour;
import static org.wpilib.math.util.UnitConversions.metersToFeet;
import static org.wpilib.math.util.UnitConversions.metersToInches;
import static org.wpilib.math.util.UnitConversions.milesPerHourToMetersPerSecond;
import static org.wpilib.math.util.UnitConversions.millisecondsToSeconds;
import static org.wpilib.math.util.UnitConversions.radiansPerSecondToRotationsPerMinute;
import static org.wpilib.math.util.UnitConversions.radiansToDegrees;
import static org.wpilib.math.util.UnitConversions.rotationsPerMinuteToRadiansPerSecond;
import static org.wpilib.math.util.UnitConversions.secondsToMilliseconds;

import org.junit.jupiter.api.Test;
import org.wpilib.UtilityClassTest;

class UnitConversionsTest extends UtilityClassTest<UnitConversions> {
  UnitConversionsTest() {
    super(UnitConversions.class);
  }

  @Test
  void metersToFeetTest() {
    assertEquals(3.28, metersToFeet(1), 1e-2);
  }

  @Test
  void feetToMetersTest() {
    assertEquals(0.30, feetToMeters(1), 1e-2);
  }

  @Test
  void metersToInchesTest() {
    assertEquals(39.37, metersToInches(1), 1e-2);
  }

  @Test
  void inchesToMetersTest() {
    assertEquals(0.0254, inchesToMeters(1), 1e-3);
  }

  @Test
  void degreesToRadiansTest() {
    assertEquals(0.017, degreesToRadians(1), 1e-3);
  }

  @Test
  void radiansToDegreesTest() {
    assertEquals(114.59, radiansToDegrees(2), 1e-2);
  }

  @Test
  void rotationsPerMinuteToRadiansPerSecondTest() {
    assertEquals(6.28, rotationsPerMinuteToRadiansPerSecond(60), 1e-2);
  }

  @Test
  void radiansPerSecondToRotationsPerMinuteTest() {
    assertEquals(76.39, radiansPerSecondToRotationsPerMinute(8), 1e-2);
  }

  @Test
  void milesPerHourToMetersPerSecondTest() {
    assertEquals(0.44704, milesPerHourToMetersPerSecond(1), 1e-2);
  }

  @Test
  void metersPerSecondToMilesPerHourTest() {
    assertEquals(2.2369, metersPerSecondToMilesPerHour(1), 1e-2);
  }

  @Test
  void millisecondsToSecondsTest() {
    assertEquals(0.5, millisecondsToSeconds(500), 1e-2);
  }

  @Test
  void secondsToMillisecondsTest() {
    assertEquals(1500, secondsToMilliseconds(1.5), 1e-2);
  }

  @Test
  void kilogramsToLbsTest() {
    assertEquals(2.20462, kilogramsToLbs(1), 1e-2);
  }

  @Test
  void lbsToKilogramsTest() {
    assertEquals(0.453592, lbsToKilograms(1), 1e-2);
  }
}
