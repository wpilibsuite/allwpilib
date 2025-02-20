// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.wpilibj.UtilityClassTest;
import org.junit.jupiter.api.Test;

class MathUtilTest extends UtilityClassTest<MathUtil> {
  MathUtilTest() {
    super(MathUtil.class);
  }

  @Test
  void testClamp() {
    // int
    assertEquals(5, MathUtil.clamp(10, 1, 5));

    // double
    assertEquals(5.5, MathUtil.clamp(10.5, 1.5, 5.5));

    // negative int
    assertEquals(-5, MathUtil.clamp(-10, -5, -1));

    // negative double
    assertEquals(-5.5, MathUtil.clamp(-10.5, -5.5, -1.5));
  }

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

  @Test
  void testInterpolate() {
    assertEquals(50, MathUtil.interpolate(0, 100, 0.5));
    assertEquals(-50, MathUtil.interpolate(0, -100, 0.5));
    assertEquals(0, MathUtil.interpolate(-50, 50, 0.5));
    assertEquals(-25, MathUtil.interpolate(-50, 50, 0.25));
    assertEquals(25, MathUtil.interpolate(-50, 50, 0.75));

    assertEquals(0, MathUtil.interpolate(0, -100, -0.5));
  }

  @Test
  void testIsNear() {
    // The answer is always 42
    // Positive integer checks
    assertTrue(MathUtil.isNear(42, 42, 1));
    assertTrue(MathUtil.isNear(42, 41, 2));
    assertTrue(MathUtil.isNear(42, 43, 2));
    assertFalse(MathUtil.isNear(42, 44, 1));

    // Negative integer checks
    assertTrue(MathUtil.isNear(-42, -42, 1));
    assertTrue(MathUtil.isNear(-42, -41, 2));
    assertTrue(MathUtil.isNear(-42, -43, 2));
    assertFalse(MathUtil.isNear(-42, -44, 1));

    // Mixed sign integer checks
    assertFalse(MathUtil.isNear(-42, 42, 1));
    assertFalse(MathUtil.isNear(-42, 41, 2));
    assertFalse(MathUtil.isNear(-42, 43, 2));
    assertFalse(MathUtil.isNear(42, -42, 1));
    assertFalse(MathUtil.isNear(42, -41, 2));
    assertFalse(MathUtil.isNear(42, -43, 2));

    // Floating point checks
    assertTrue(MathUtil.isNear(42, 41.5, 1));
    assertTrue(MathUtil.isNear(42, 42.5, 1));
    assertTrue(MathUtil.isNear(42, 41.5, 0.75));
    assertTrue(MathUtil.isNear(42, 42.5, 0.75));

    // Wraparound checks
    assertTrue(MathUtil.isNear(0, 356, 5, 0, 360));
    assertTrue(MathUtil.isNear(0, -356, 5, 0, 360));
    assertTrue(MathUtil.isNear(0, 4, 5, 0, 360));
    assertTrue(MathUtil.isNear(0, -4, 5, 0, 360));
    assertTrue(MathUtil.isNear(400, 41, 5, 0, 360));
    assertTrue(MathUtil.isNear(400, -319, 5, 0, 360));
    assertTrue(MathUtil.isNear(400, 401, 5, 0, 360));
    assertFalse(MathUtil.isNear(0, 356, 2.5, 0, 360));
    assertFalse(MathUtil.isNear(0, -356, 2.5, 0, 360));
    assertFalse(MathUtil.isNear(0, 4, 2.5, 0, 360));
    assertFalse(MathUtil.isNear(0, -4, 2.5, 0, 360));
    assertFalse(MathUtil.isNear(400, 35, 5, 0, 360));
    assertFalse(MathUtil.isNear(400, -315, 5, 0, 360));
    assertFalse(MathUtil.isNear(400, 395, 5, 0, 360));
  }

  @Test
  void testSlewRateTranslation3d() {
    Translation3d translation1 = new Translation3d(0, 0, 0);
    Translation3d translation2 = new Translation3d(2, 2, 2);
    Translation3d translation3 = new Translation3d(1, 1, 1);
    Translation3d translation4 = new Translation3d(3, 3, 3);
    Translation3d result1 = MathUtil.slewRateLimit(translation1, translation2, 0.25, 50);
    Translation3d result2 = MathUtil.slewRateLimit(translation3, translation4, 1, 2);
    Translation3d expected1 = new Translation3d(8, 8, 8);
    Translation3d expected2 = new Translation3d(2.154700538, 2.154700538, 2.154700538);
    assertEquals(expected1, result1);
    assertEquals(expected2, result2);
  }

  @Test
  void testSlewRateTranslation2d() {
    Translation2d translation1 = new Translation2d(0, 0);
    Translation2d translation2 = new Translation2d(2, 2);
    Translation2d translation3 = new Translation2d(1, 1);
    Translation2d translation4 = new Translation2d(3, 3);
    Translation2d result1 = MathUtil.slewRateLimit(translation1, translation2, 0.25, 50);
    Translation2d result2 = MathUtil.slewRateLimit(translation3, translation4, 1, 2);
    Translation2d expected1 = new Translation2d(8, 8);
    Translation2d expected2 = new Translation2d(2.414213562, 2.414213562);
    assertEquals(expected1, result1);
    assertEquals(expected2, result2);
  }
}
