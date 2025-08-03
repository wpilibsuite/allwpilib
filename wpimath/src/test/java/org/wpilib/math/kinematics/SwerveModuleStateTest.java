// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Rotation2d;

class SwerveModuleStateTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testOptimize() {
    var angleA = Rotation2d.fromDegrees(45);
    var refA = new SwerveModuleState(-2.0, Rotation2d.kPi);
    var optimizedA = refA.optimize(angleA);

    assertAll(
        () -> assertEquals(2.0, optimizedA.speed, kEpsilon),
        () -> assertEquals(0.0, optimizedA.angle.getDegrees(), kEpsilon));

    var angleB = Rotation2d.fromDegrees(-50);
    var refB = new SwerveModuleState(4.7, Rotation2d.fromDegrees(41));
    var optimizedB = refB.optimize(angleB);

    assertAll(
        () -> assertEquals(-4.7, optimizedB.speed, kEpsilon),
        () -> assertEquals(-139.0, optimizedB.angle.getDegrees(), kEpsilon));
  }

  @Test
  void testNoOptimize() {
    var angleA = Rotation2d.kZero;
    var refA = new SwerveModuleState(2.0, Rotation2d.fromDegrees(89));
    var optimizedA = refA.optimize(angleA);

    assertAll(
        () -> assertEquals(2.0, optimizedA.speed, kEpsilon),
        () -> assertEquals(89.0, optimizedA.angle.getDegrees(), kEpsilon));

    var angleB = Rotation2d.kZero;
    var refB = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(-2));
    var optimizedB = refB.optimize(angleB);

    assertAll(
        () -> assertEquals(-2.0, optimizedB.speed, kEpsilon),
        () -> assertEquals(-2.0, optimizedB.angle.getDegrees(), kEpsilon));
  }

  @Test
  void testCosineScale() {
    var angleA = Rotation2d.fromDegrees(0.0);
    var refA = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    var optimizedA = refA.cosineScale(angleA);

    assertAll(
        () -> assertEquals(Math.sqrt(2.0), optimizedA.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedA.angle.getDegrees(), kEpsilon));

    var angleB = Rotation2d.fromDegrees(45.0);
    var refB = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    var optimizedB = refB.cosineScale(angleB);

    assertAll(
        () -> assertEquals(2.0, optimizedB.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedB.angle.getDegrees(), kEpsilon));

    var angleC = Rotation2d.fromDegrees(-45.0);
    var refC = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    var optimizedC = refC.cosineScale(angleC);

    assertAll(
        () -> assertEquals(0.0, optimizedC.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedC.angle.getDegrees(), kEpsilon));

    var angleD = Rotation2d.fromDegrees(135.0);
    var refD = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    var optimizedD = refD.cosineScale(angleD);

    assertAll(
        () -> assertEquals(0.0, optimizedD.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedD.angle.getDegrees(), kEpsilon));

    var angleE = Rotation2d.fromDegrees(-135.0);
    var refE = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    var optimizedE = refE.cosineScale(angleE);

    assertAll(
        () -> assertEquals(-2.0, optimizedE.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedE.angle.getDegrees(), kEpsilon));

    var angleF = Rotation2d.fromDegrees(180.0);
    var refF = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    var optimizedF = refF.cosineScale(angleF);

    assertAll(
        () -> assertEquals(-Math.sqrt(2.0), optimizedF.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedF.angle.getDegrees(), kEpsilon));

    var angleG = Rotation2d.fromDegrees(0.0);
    var refG = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    var optimizedG = refG.cosineScale(angleG);

    assertAll(
        () -> assertEquals(-Math.sqrt(2.0), optimizedG.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedG.angle.getDegrees(), kEpsilon));

    var angleH = Rotation2d.fromDegrees(45.0);
    var refH = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    var optimizedH = refH.cosineScale(angleH);

    assertAll(
        () -> assertEquals(-2.0, optimizedH.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedH.angle.getDegrees(), kEpsilon));

    var angleI = Rotation2d.fromDegrees(-45.0);
    var refI = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    var optimizedI = refI.cosineScale(angleI);

    assertAll(
        () -> assertEquals(0.0, optimizedI.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedI.angle.getDegrees(), kEpsilon));

    var angleJ = Rotation2d.fromDegrees(135.0);
    var refJ = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    var optimizedJ = refJ.cosineScale(angleJ);

    assertAll(
        () -> assertEquals(0.0, optimizedJ.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedJ.angle.getDegrees(), kEpsilon));

    var angleK = Rotation2d.fromDegrees(-135.0);
    var refK = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    var optimizedK = refK.cosineScale(angleK);

    assertAll(
        () -> assertEquals(2.0, optimizedK.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedK.angle.getDegrees(), kEpsilon));

    var angleL = Rotation2d.fromDegrees(180.0);
    var refL = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    var optimizedL = refL.cosineScale(angleL);

    assertAll(
        () -> assertEquals(Math.sqrt(2.0), optimizedL.speed, kEpsilon),
        () -> assertEquals(45.0, optimizedL.angle.getDegrees(), kEpsilon));
  }

  @Test
  void testInterpolate() {
    // Test basic interpolation with simple angles
    final var start = new SwerveModuleState(1.0, Rotation2d.fromDegrees(0.0));
    final var end = new SwerveModuleState(5.0, Rotation2d.fromDegrees(90.0));

    // Test interpolation at t=0 (should return start)
    final var atStart = start.interpolate(end, 0.0);
    assertAll(
        () -> assertEquals(1.0, atStart.speed, kEpsilon),
        () -> assertEquals(0.0, atStart.angle.getDegrees(), kEpsilon));

    // Test interpolation at t=1 (should return end)
    final var atEnd = start.interpolate(end, 1.0);
    assertAll(
        () -> assertEquals(5.0, atEnd.speed, kEpsilon),
        () -> assertEquals(90.0, atEnd.angle.getDegrees(), kEpsilon));

    // Test interpolation at t=0.5 (should return midpoint)
    final var atMidpoint = start.interpolate(end, 0.5);
    assertAll(
        () -> assertEquals(3.0, atMidpoint.speed, kEpsilon),
        () -> assertEquals(45.0, atMidpoint.angle.getDegrees(), kEpsilon));

    // Test interpolation at t=0.25
    final var atQuarter = start.interpolate(end, 0.25);
    assertAll(
        () -> assertEquals(2.0, atQuarter.speed, kEpsilon),
        () -> assertEquals(22.5, atQuarter.angle.getDegrees(), kEpsilon));

    // Test clamping: t < 0 should clamp to 0
    final var belowRange = start.interpolate(end, -0.5);
    assertAll(
        () -> assertEquals(1.0, belowRange.speed, kEpsilon),
        () -> assertEquals(0.0, belowRange.angle.getDegrees(), kEpsilon));

    // Test clamping: t > 1 should clamp to 1
    final var aboveRange = start.interpolate(end, 1.5);
    assertAll(
        () -> assertEquals(5.0, aboveRange.speed, kEpsilon),
        () -> assertEquals(90.0, aboveRange.angle.getDegrees(), kEpsilon));

    // Test angle wrapping with crossing 180/-180 boundary
    final var startWrap = new SwerveModuleState(2.0, Rotation2d.fromDegrees(170.0));
    final var endWrap = new SwerveModuleState(4.0, Rotation2d.fromDegrees(-170.0));
    final var midpointWrap = startWrap.interpolate(endWrap, 0.5);
    assertAll(
        () -> assertEquals(3.0, midpointWrap.speed, kEpsilon),
        () -> assertEquals(180.0, Math.abs(midpointWrap.angle.getDegrees()), kEpsilon));
  }
}
