// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.kinematics.ChassisVelocities;

class AntiTippingTest {
  private static final double kTolerance = 1e-6;

  @Test
  void testBelowThresholdGeneratesNoCorrection() {
    AntiTipping antiTipping = new AntiTipping(0.1, Math.toRadians(3.0), 2.0);
    Rotation3d flat = new Rotation3d(Math.toRadians(1.0), Math.toRadians(1.0), 0.0);
    ChassisVelocities correction = antiTipping.calculate(flat);

    assertEquals(0.0, correction.vx, kTolerance);
    assertEquals(0.0, correction.vy, kTolerance);
  }

  @Test
  void testForwardTipDrivesForward() {
    AntiTipping antiTipping = new AntiTipping(0.1, Math.toRadians(3.0), 2.0);
    Rotation3d tippingForward = new Rotation3d(0.0, Math.toRadians(10.0), 0.0);
    ChassisVelocities correction = antiTipping.calculate(tippingForward);

    assertTrue(correction.vx > 0.0);
    assertEquals(0.0, correction.vy, kTolerance);
  }

  @Test
  void testBackwardTipDrivesBackward() {
    AntiTipping antiTipping = new AntiTipping(0.1, Math.toRadians(3.0), 2.0);
    Rotation3d tippingBackward = new Rotation3d(0.0, Math.toRadians(-10.0), 0.0);
    ChassisVelocities correction = antiTipping.calculate(tippingBackward);

    assertTrue(correction.vx < 0.0);
    assertEquals(0.0, correction.vy, kTolerance);
  }

  @Test
  void testRightRollDrivesRight() {
    AntiTipping antiTipping = new AntiTipping(0.1, Math.toRadians(3.0), 2.0);
    Rotation3d rollingRight = new Rotation3d(Math.toRadians(15.0), 0.0, 0.0);
    ChassisVelocities correction = antiTipping.calculate(rollingRight);

    assertEquals(0.0, correction.vx, kTolerance);
    assertTrue(correction.vy < 0.0);
  }

  @Test
  void testLeftRollDrivesLeft() {
    AntiTipping antiTipping = new AntiTipping(0.1, Math.toRadians(3.0), 2.0);
    Rotation3d rollingLeft = new Rotation3d(Math.toRadians(-15.0), 0.0, 0.0);
    ChassisVelocities correction = antiTipping.calculate(rollingLeft);

    assertEquals(0.0, correction.vx, kTolerance);
    assertTrue(correction.vy > 0.0);
  }
}