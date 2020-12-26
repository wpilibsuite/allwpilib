// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpiutil.math;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class MathUtilTest {
  @Test
  void testAngleNormalize() {
    assertEquals(MathUtil.normalizeAngle(5 * Math.PI), Math.PI);
    assertEquals(MathUtil.normalizeAngle(-5 * Math.PI), Math.PI);
    assertEquals(MathUtil.normalizeAngle(Math.PI / 2), Math.PI / 2);
    assertEquals(MathUtil.normalizeAngle(-Math.PI / 2), -Math.PI / 2);
  }
}
