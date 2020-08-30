/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.UtilityClassTest;

import static org.junit.jupiter.api.Assertions.assertEquals;

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
}
