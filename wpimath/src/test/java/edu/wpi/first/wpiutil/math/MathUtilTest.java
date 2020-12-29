// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpiutil.math;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class MathUtilTest {
  @Test
  void testInputModulus() {
    // These tests check error wrapping. That is, the result of wrapping the
    // result of an angle reference minus the measurement.

    // Test symmetric range
    assertEquals(-20.0, MathUtil.inputModulus(170.0 - (-170.0), -180.0, 180.0));
    assertEquals(-20.0, MathUtil.inputModulus(170.0 + 360.0 - (-170.0), -180.0, 180.0));
    assertEquals(-20.0, MathUtil.inputModulus(170.0 - (-170.0 + 360.0), -180.0, 180.0));
    assertEquals(20.0, MathUtil.inputModulus(-170.0 - 170.0, -180.0, 180.0));
    assertEquals(20.0, MathUtil.inputModulus(-170.0 + 360.0 - 170.0, -180.0, 180.0));
    assertEquals(20.0, MathUtil.inputModulus(-170.0 - (170.0 + 360.0), -180.0, 180.0));

    // Test range start at zero
    assertEquals(-20.0, MathUtil.inputModulus(170.0 - 190.0, 0.0, 360.0));
    assertEquals(-20.0, MathUtil.inputModulus(170.0 + 360.0 - 190.0, 0.0, 360.0));
    assertEquals(-20.0, MathUtil.inputModulus(170.0 - (190.0 + 360), 0.0, 360.0));

    // Test asymmetric range that doesn't start at zero
    assertEquals(-20.0, MathUtil.inputModulus(170.0 - (-170.0), -170.0, 190.0));
  }

  @Test
  void testAngleModulus() {
    assertEquals(MathUtil.angleModulus(Math.toRadians(-2000)), Math.toRadians(160), 1e-6);
    assertEquals(MathUtil.angleModulus(Math.toRadians(358)), Math.toRadians(-2), 1e-6);
    assertEquals(MathUtil.angleModulus(Math.toRadians(360)), 0, 1e-6);

    assertEquals(MathUtil.angleModulus(5 * Math.PI), Math.PI);
    assertEquals(MathUtil.angleModulus(-5 * Math.PI), Math.PI);
    assertEquals(MathUtil.angleModulus(Math.PI / 2), Math.PI / 2);
    assertEquals(MathUtil.angleModulus(-Math.PI / 2), -Math.PI / 2);
  }
}
