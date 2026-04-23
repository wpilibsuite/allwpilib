// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Optional;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.kinematics.ChassisVelocities;

public class AntiTippingTest {
  private static final double kTolerance = 1e-6;

  @Test
  public void testBelowThresholdGeneratesNoCorrection() {
    AntiTipping antiTipping = new AntiTipping(0.1, Math.toRadians(3.0), 2.0);
    Rotation3d flat = new Rotation3d(Math.toRadians(1.0), Math.toRadians(1.0), 0.0);

    assertFalse(antiTipping.calculate(flat).isPresent());
  }

  @Test
  public void testForwardTipDrivesForward() {
    AntiTipping antiTipping = new AntiTipping(0.1, Math.toRadians(3.0), 2.0);
    Rotation3d tippingForward = new Rotation3d(0.0, Math.toRadians(10.0), 0.0);
    Optional<ChassisVelocities> correctionOpt = antiTipping.calculate(tippingForward);

    assertTrue(correctionOpt.isPresent());
    assertTrue(correctionOpt.get().vx > 0.0);
    assertEquals(0.0, correctionOpt.get().vy, kTolerance);
  }

  @Test
  public void testBackwardTipDrivesBackward() {
    AntiTipping antiTipping = new AntiTipping(0.1, Math.toRadians(3.0), 2.0);
    Rotation3d tippingBackward = new Rotation3d(0.0, Math.toRadians(-10.0), 0.0);
    Optional<ChassisVelocities> correctionOpt = antiTipping.calculate(tippingBackward);

    assertTrue(correctionOpt.isPresent());
    assertTrue(correctionOpt.get().vx < 0.0);
    assertEquals(0.0, correctionOpt.get().vy, kTolerance);
  }

  @Test
  public void testRightRollDrivesRight() {
    AntiTipping antiTipping = new AntiTipping(0.1, Math.toRadians(3.0), 2.0);
    Rotation3d rollingRight = new Rotation3d(Math.toRadians(15.0), 0.0, 0.0);
    Optional<ChassisVelocities> correctionOpt = antiTipping.calculate(rollingRight);

    assertTrue(correctionOpt.isPresent());
    assertEquals(0.0, correctionOpt.get().vx, kTolerance);
    assertTrue(correctionOpt.get().vy < 0.0);
  }

  @Test
  public void testLeftRollDrivesLeft() {
    AntiTipping antiTipping = new AntiTipping(0.1, Math.toRadians(3.0), 2.0);
    Rotation3d rollingLeft = new Rotation3d(Math.toRadians(-15.0), 0.0, 0.0);
    Optional<ChassisVelocities> correctionOpt = antiTipping.calculate(rollingLeft);

    assertTrue(correctionOpt.isPresent());
    assertEquals(0.0, correctionOpt.get().vx, kTolerance);
    assertTrue(correctionOpt.get().vy > 0.0);
  }
}
