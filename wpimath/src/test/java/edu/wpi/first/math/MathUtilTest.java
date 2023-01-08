// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class MathUtilTest {
  @Test
  void testApplyDeadbandUnityScale() {
    // < 0
    assertEquals(-1.0, MathUtil.applyDeadband(-1.0, 0.02));
    assertEquals((-0.03 + 0.02) / (1.0 - 0.02), MathUtil.applyDeadband(-0.03, 0.02));
    assertEquals(0.0, MathUtil.applyDeadband(-0.02, 0.02));
    assertEquals(0.0, MathUtil.applyDeadband(-0.01, 0.02));

    // == 0
    assertEquals(0.0, MathUtil.applyDeadband(0.0, 0.02));

    // > 0
    assertEquals(0.0, MathUtil.applyDeadband(0.01, 0.02));
    assertEquals(0.0, MathUtil.applyDeadband(0.02, 0.02));
    assertEquals((0.03 - 0.02) / (1.0 - 0.02), MathUtil.applyDeadband(0.03, 0.02));
    assertEquals(1.0, MathUtil.applyDeadband(1.0, 0.02));
  }

  @Test
  void testApplyDeadbandArbitraryScale() {
    // < 0
    assertEquals(-2.5, MathUtil.applyDeadband(-2.5, 0.02, 2.5));
    assertEquals(0.0, MathUtil.applyDeadband(-0.02, 0.02, 2.5));
    assertEquals(0.0, MathUtil.applyDeadband(-0.01, 0.02, 2.5));

    // == 0
    assertEquals(0.0, MathUtil.applyDeadband(0.0, 0.02, 2.5));

    // > 0
    assertEquals(0.0, MathUtil.applyDeadband(0.01, 0.02, 2.5));
    assertEquals(0.0, MathUtil.applyDeadband(0.02, 0.02, 2.5));
    assertEquals(2.5, MathUtil.applyDeadband(2.5, 0.02, 2.5));
  }

  @Test
  void testApplyDeadbandLargeMaxMagnitude() {
    assertEquals(80.0, MathUtil.applyDeadband(100.0, 20, Double.POSITIVE_INFINITY));
  }

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
    assertEquals(340.0, MathUtil.inputModulus(170.0 - 190.0, 0.0, 360.0));
    assertEquals(340.0, MathUtil.inputModulus(170.0 + 360.0 - 190.0, 0.0, 360.0));
    assertEquals(340.0, MathUtil.inputModulus(170.0 - (190.0 + 360), 0.0, 360.0));

    // Test asymmetric range that doesn't start at zero
    assertEquals(-20.0, MathUtil.inputModulus(170.0 - (-170.0), -170.0, 190.0));

    // Test range with both positive endpoints
    assertEquals(2.0, MathUtil.inputModulus(0.0, 1.0, 3.0));
    assertEquals(3.0, MathUtil.inputModulus(1.0, 1.0, 3.0));
    assertEquals(2.0, MathUtil.inputModulus(2.0, 1.0, 3.0));
    assertEquals(3.0, MathUtil.inputModulus(3.0, 1.0, 3.0));
    assertEquals(2.0, MathUtil.inputModulus(4.0, 1.0, 3.0));
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
