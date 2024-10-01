// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import org.junit.jupiter.api.Test;

class SwerveModuleStateTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testOptimize() {
    var angleA = Rotation2d.fromDegrees(45);
    var refA = new SwerveModuleState(-2.0, Rotation2d.kPi);
    refA.optimize(angleA);

    assertAll(
        () -> assertEquals(2.0, refA.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, refA.angle.getDegrees(), kEpsilon));

    var angleB = Rotation2d.fromDegrees(-50);
    var refB = new SwerveModuleState(4.7, Rotation2d.fromDegrees(41));
    refB.optimize(angleB);

    assertAll(
        () -> assertEquals(-4.7, refB.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(-139.0, refB.angle.getDegrees(), kEpsilon));
  }

  @Test
  void testNoOptimize() {
    var angleA = Rotation2d.kZero;
    var refA = new SwerveModuleState(2.0, Rotation2d.fromDegrees(89));
    refA.optimize(angleA);

    assertAll(
        () -> assertEquals(2.0, refA.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(89.0, refA.angle.getDegrees(), kEpsilon));

    var angleB = Rotation2d.kZero;
    var refB = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(-2));
    refB.optimize(angleB);

    assertAll(
        () -> assertEquals(-2.0, refB.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(-2.0, refB.angle.getDegrees(), kEpsilon));
  }

  @Test
  void testCosineScale() {
    var angleA = Rotation2d.fromDegrees(0.0);
    var refA = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refA.cosineScale(angleA);

    assertAll(
        () -> assertEquals(Math.sqrt(2.0), refA.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refA.angle.getDegrees(), kEpsilon));

    var angleB = Rotation2d.fromDegrees(45.0);
    var refB = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refB.cosineScale(angleB);

    assertAll(
        () -> assertEquals(2.0, refB.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refB.angle.getDegrees(), kEpsilon));

    var angleC = Rotation2d.fromDegrees(-45.0);
    var refC = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refC.cosineScale(angleC);

    assertAll(
        () -> assertEquals(0.0, refC.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refC.angle.getDegrees(), kEpsilon));

    var angleD = Rotation2d.fromDegrees(135.0);
    var refD = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refD.cosineScale(angleD);

    assertAll(
        () -> assertEquals(0.0, refD.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refD.angle.getDegrees(), kEpsilon));

    var angleE = Rotation2d.fromDegrees(-135.0);
    var refE = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refE.cosineScale(angleE);

    assertAll(
        () -> assertEquals(-2.0, refE.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refE.angle.getDegrees(), kEpsilon));

    var angleF = Rotation2d.fromDegrees(180.0);
    var refF = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refF.cosineScale(angleF);

    assertAll(
        () -> assertEquals(-Math.sqrt(2.0), refF.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refF.angle.getDegrees(), kEpsilon));

    var angleG = Rotation2d.fromDegrees(0.0);
    var refG = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refG.cosineScale(angleG);

    assertAll(
        () -> assertEquals(-Math.sqrt(2.0), refG.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refG.angle.getDegrees(), kEpsilon));

    var angleH = Rotation2d.fromDegrees(45.0);
    var refH = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refH.cosineScale(angleH);

    assertAll(
        () -> assertEquals(-2.0, refH.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refH.angle.getDegrees(), kEpsilon));

    var angleI = Rotation2d.fromDegrees(-45.0);
    var refI = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refI.cosineScale(angleI);

    assertAll(
        () -> assertEquals(0.0, refI.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refI.angle.getDegrees(), kEpsilon));

    var angleJ = Rotation2d.fromDegrees(135.0);
    var refJ = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refJ.cosineScale(angleJ);

    assertAll(
        () -> assertEquals(0.0, refJ.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refJ.angle.getDegrees(), kEpsilon));

    var angleK = Rotation2d.fromDegrees(-135.0);
    var refK = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refK.cosineScale(angleK);

    assertAll(
        () -> assertEquals(2.0, refK.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refK.angle.getDegrees(), kEpsilon));

    var angleL = Rotation2d.fromDegrees(180.0);
    var refL = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refL.cosineScale(angleL);

    assertAll(
        () -> assertEquals(Math.sqrt(2.0), refL.speedMetersPerSecond, kEpsilon),
        () -> assertEquals(45.0, refL.angle.getDegrees(), kEpsilon));
  }
}
