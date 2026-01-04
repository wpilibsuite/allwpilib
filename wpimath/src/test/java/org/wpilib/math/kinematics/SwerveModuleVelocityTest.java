// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Rotation2d;

class SwerveModuleVelocityTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testOptimize() {
    var angleA = Rotation2d.fromDegrees(45);
    var refA = new SwerveModuleVelocity(-2.0, Rotation2d.kPi);
    refA.optimize(angleA);

    assertAll(
        () -> assertEquals(2.0, refA.velocity, kEpsilon),
        () -> assertEquals(0.0, refA.angle.getDegrees(), kEpsilon));

    var angleB = Rotation2d.fromDegrees(-50);
    var refB = new SwerveModuleVelocity(4.7, Rotation2d.fromDegrees(41));
    refB.optimize(angleB);

    assertAll(
        () -> assertEquals(-4.7, refB.velocity, kEpsilon),
        () -> assertEquals(-139.0, refB.angle.getDegrees(), kEpsilon));
  }

  @Test
  void testNoOptimize() {
    var angleA = Rotation2d.kZero;
    var refA = new SwerveModuleVelocity(2.0, Rotation2d.fromDegrees(89));
    refA.optimize(angleA);

    assertAll(
        () -> assertEquals(2.0, refA.velocity, kEpsilon),
        () -> assertEquals(89.0, refA.angle.getDegrees(), kEpsilon));

    var angleB = Rotation2d.kZero;
    var refB = new SwerveModuleVelocity(-2.0, Rotation2d.fromDegrees(-2));
    refB.optimize(angleB);

    assertAll(
        () -> assertEquals(-2.0, refB.velocity, kEpsilon),
        () -> assertEquals(-2.0, refB.angle.getDegrees(), kEpsilon));
  }

  @Test
  void testCosineScale() {
    var angleA = Rotation2d.fromDegrees(0.0);
    var refA = new SwerveModuleVelocity(2.0, Rotation2d.fromDegrees(45.0));
    refA.cosineScale(angleA);

    assertAll(
        () -> assertEquals(Math.sqrt(2.0), refA.velocity, kEpsilon),
        () -> assertEquals(45.0, refA.angle.getDegrees(), kEpsilon));

    var angleB = Rotation2d.fromDegrees(45.0);
    var refB = new SwerveModuleVelocity(2.0, Rotation2d.fromDegrees(45.0));
    refB.cosineScale(angleB);

    assertAll(
        () -> assertEquals(2.0, refB.velocity, kEpsilon),
        () -> assertEquals(45.0, refB.angle.getDegrees(), kEpsilon));

    var angleC = Rotation2d.fromDegrees(-45.0);
    var refC = new SwerveModuleVelocity(2.0, Rotation2d.fromDegrees(45.0));
    refC.cosineScale(angleC);

    assertAll(
        () -> assertEquals(0.0, refC.velocity, kEpsilon),
        () -> assertEquals(45.0, refC.angle.getDegrees(), kEpsilon));

    var angleD = Rotation2d.fromDegrees(135.0);
    var refD = new SwerveModuleVelocity(2.0, Rotation2d.fromDegrees(45.0));
    refD.cosineScale(angleD);

    assertAll(
        () -> assertEquals(0.0, refD.velocity, kEpsilon),
        () -> assertEquals(45.0, refD.angle.getDegrees(), kEpsilon));

    var angleE = Rotation2d.fromDegrees(-135.0);
    var refE = new SwerveModuleVelocity(2.0, Rotation2d.fromDegrees(45.0));
    refE.cosineScale(angleE);

    assertAll(
        () -> assertEquals(-2.0, refE.velocity, kEpsilon),
        () -> assertEquals(45.0, refE.angle.getDegrees(), kEpsilon));

    var angleF = Rotation2d.fromDegrees(180.0);
    var refF = new SwerveModuleVelocity(2.0, Rotation2d.fromDegrees(45.0));
    refF.cosineScale(angleF);

    assertAll(
        () -> assertEquals(-Math.sqrt(2.0), refF.velocity, kEpsilon),
        () -> assertEquals(45.0, refF.angle.getDegrees(), kEpsilon));

    var angleG = Rotation2d.fromDegrees(0.0);
    var refG = new SwerveModuleVelocity(-2.0, Rotation2d.fromDegrees(45.0));
    refG.cosineScale(angleG);

    assertAll(
        () -> assertEquals(-Math.sqrt(2.0), refG.velocity, kEpsilon),
        () -> assertEquals(45.0, refG.angle.getDegrees(), kEpsilon));

    var angleH = Rotation2d.fromDegrees(45.0);
    var refH = new SwerveModuleVelocity(-2.0, Rotation2d.fromDegrees(45.0));
    refH.cosineScale(angleH);

    assertAll(
        () -> assertEquals(-2.0, refH.velocity, kEpsilon),
        () -> assertEquals(45.0, refH.angle.getDegrees(), kEpsilon));

    var angleI = Rotation2d.fromDegrees(-45.0);
    var refI = new SwerveModuleVelocity(-2.0, Rotation2d.fromDegrees(45.0));
    refI.cosineScale(angleI);

    assertAll(
        () -> assertEquals(0.0, refI.velocity, kEpsilon),
        () -> assertEquals(45.0, refI.angle.getDegrees(), kEpsilon));

    var angleJ = Rotation2d.fromDegrees(135.0);
    var refJ = new SwerveModuleVelocity(-2.0, Rotation2d.fromDegrees(45.0));
    refJ.cosineScale(angleJ);

    assertAll(
        () -> assertEquals(0.0, refJ.velocity, kEpsilon),
        () -> assertEquals(45.0, refJ.angle.getDegrees(), kEpsilon));

    var angleK = Rotation2d.fromDegrees(-135.0);
    var refK = new SwerveModuleVelocity(-2.0, Rotation2d.fromDegrees(45.0));
    refK.cosineScale(angleK);

    assertAll(
        () -> assertEquals(2.0, refK.velocity, kEpsilon),
        () -> assertEquals(45.0, refK.angle.getDegrees(), kEpsilon));

    var angleL = Rotation2d.fromDegrees(180.0);
    var refL = new SwerveModuleVelocity(-2.0, Rotation2d.fromDegrees(45.0));
    refL.cosineScale(angleL);

    assertAll(
        () -> assertEquals(Math.sqrt(2.0), refL.velocity, kEpsilon),
        () -> assertEquals(45.0, refL.angle.getDegrees(), kEpsilon));
  }

  @Test
  void testInterpolate() {
    // Test basic interpolation with simple angles
    final var start = new SwerveModuleVelocity(1.0, Rotation2d.fromDegrees(0.0));
    final var end = new SwerveModuleVelocity(5.0, Rotation2d.fromDegrees(90.0));

    // Test interpolation at t=0 (should return start)
    final var atStart = start.interpolate(end, 0.0);
    assertAll(
        () -> assertEquals(1.0, atStart.velocity, kEpsilon),
        () -> assertEquals(0.0, atStart.angle.getDegrees(), kEpsilon));

    // Test interpolation at t=1 (should return end)
    final var atEnd = start.interpolate(end, 1.0);
    assertAll(
        () -> assertEquals(5.0, atEnd.velocity, kEpsilon),
        () -> assertEquals(90.0, atEnd.angle.getDegrees(), kEpsilon));

    // Test interpolation at t=0.5 (should return midpoint)
    final var atMidpoint = start.interpolate(end, 0.5);
    assertAll(
        () -> assertEquals(3.0, atMidpoint.velocity, kEpsilon),
        () -> assertEquals(45.0, atMidpoint.angle.getDegrees(), kEpsilon));

    // Test interpolation at t=0.25
    final var atQuarter = start.interpolate(end, 0.25);
    assertAll(
        () -> assertEquals(2.0, atQuarter.velocity, kEpsilon),
        () -> assertEquals(22.5, atQuarter.angle.getDegrees(), kEpsilon));

    // Test clamping: t < 0 should clamp to 0
    final var belowRange = start.interpolate(end, -0.5);
    assertAll(
        () -> assertEquals(1.0, belowRange.velocity, kEpsilon),
        () -> assertEquals(0.0, belowRange.angle.getDegrees(), kEpsilon));

    // Test clamping: t > 1 should clamp to 1
    final var aboveRange = start.interpolate(end, 1.5);
    assertAll(
        () -> assertEquals(5.0, aboveRange.velocity, kEpsilon),
        () -> assertEquals(90.0, aboveRange.angle.getDegrees(), kEpsilon));

    // Test angle wrapping with crossing 180/-180 boundary
    final var startWrap = new SwerveModuleVelocity(2.0, Rotation2d.fromDegrees(170.0));
    final var endWrap = new SwerveModuleVelocity(4.0, Rotation2d.fromDegrees(-170.0));
    final var midpointWrap = startWrap.interpolate(endWrap, 0.5);
    assertAll(
        () -> assertEquals(3.0, midpointWrap.velocity, kEpsilon),
        () -> assertEquals(180.0, Math.abs(midpointWrap.angle.getDegrees()), kEpsilon));
  }
}
