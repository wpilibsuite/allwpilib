/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
