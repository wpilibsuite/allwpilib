// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import org.junit.jupiter.api.Test;

class SwerveModuleStateTest {
  private static final double EPSILON = 1E-9;

  @Test
  void testOptimize() {
    var angleA = Rotation2d.fromDegrees(45);
    var refA = new SwerveModuleState(-2.0, Rotation2d.PI);
    refA.optimize(angleA);

    assertAll(
        () -> assertEquals(2.0, refA.speed, EPSILON),
        () -> assertEquals(0.0, refA.angle.getDegrees(), EPSILON));

    var angleB = Rotation2d.fromDegrees(-50);
    var refB = new SwerveModuleState(4.7, Rotation2d.fromDegrees(41));
    refB.optimize(angleB);

    assertAll(
        () -> assertEquals(-4.7, refB.speed, EPSILON),
        () -> assertEquals(-139.0, refB.angle.getDegrees(), EPSILON));
  }

  @Test
  void testNoOptimize() {
    var angleA = Rotation2d.ZERO;
    var refA = new SwerveModuleState(2.0, Rotation2d.fromDegrees(89));
    refA.optimize(angleA);

    assertAll(
        () -> assertEquals(2.0, refA.speed, EPSILON),
        () -> assertEquals(89.0, refA.angle.getDegrees(), EPSILON));

    var angleB = Rotation2d.ZERO;
    var refB = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(-2));
    refB.optimize(angleB);

    assertAll(
        () -> assertEquals(-2.0, refB.speed, EPSILON),
        () -> assertEquals(-2.0, refB.angle.getDegrees(), EPSILON));
  }

  @Test
  void testCosineScale() {
    var angleA = Rotation2d.fromDegrees(0.0);
    var refA = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refA.cosineScale(angleA);

    assertAll(
        () -> assertEquals(Math.sqrt(2.0), refA.speed, EPSILON),
        () -> assertEquals(45.0, refA.angle.getDegrees(), EPSILON));

    var angleB = Rotation2d.fromDegrees(45.0);
    var refB = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refB.cosineScale(angleB);

    assertAll(
        () -> assertEquals(2.0, refB.speed, EPSILON),
        () -> assertEquals(45.0, refB.angle.getDegrees(), EPSILON));

    var angleC = Rotation2d.fromDegrees(-45.0);
    var refC = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refC.cosineScale(angleC);

    assertAll(
        () -> assertEquals(0.0, refC.speed, EPSILON),
        () -> assertEquals(45.0, refC.angle.getDegrees(), EPSILON));

    var angleD = Rotation2d.fromDegrees(135.0);
    var refD = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refD.cosineScale(angleD);

    assertAll(
        () -> assertEquals(0.0, refD.speed, EPSILON),
        () -> assertEquals(45.0, refD.angle.getDegrees(), EPSILON));

    var angleE = Rotation2d.fromDegrees(-135.0);
    var refE = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refE.cosineScale(angleE);

    assertAll(
        () -> assertEquals(-2.0, refE.speed, EPSILON),
        () -> assertEquals(45.0, refE.angle.getDegrees(), EPSILON));

    var angleF = Rotation2d.fromDegrees(180.0);
    var refF = new SwerveModuleState(2.0, Rotation2d.fromDegrees(45.0));
    refF.cosineScale(angleF);

    assertAll(
        () -> assertEquals(-Math.sqrt(2.0), refF.speed, EPSILON),
        () -> assertEquals(45.0, refF.angle.getDegrees(), EPSILON));

    var angleG = Rotation2d.fromDegrees(0.0);
    var refG = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refG.cosineScale(angleG);

    assertAll(
        () -> assertEquals(-Math.sqrt(2.0), refG.speed, EPSILON),
        () -> assertEquals(45.0, refG.angle.getDegrees(), EPSILON));

    var angleH = Rotation2d.fromDegrees(45.0);
    var refH = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refH.cosineScale(angleH);

    assertAll(
        () -> assertEquals(-2.0, refH.speed, EPSILON),
        () -> assertEquals(45.0, refH.angle.getDegrees(), EPSILON));

    var angleI = Rotation2d.fromDegrees(-45.0);
    var refI = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refI.cosineScale(angleI);

    assertAll(
        () -> assertEquals(0.0, refI.speed, EPSILON),
        () -> assertEquals(45.0, refI.angle.getDegrees(), EPSILON));

    var angleJ = Rotation2d.fromDegrees(135.0);
    var refJ = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refJ.cosineScale(angleJ);

    assertAll(
        () -> assertEquals(0.0, refJ.speed, EPSILON),
        () -> assertEquals(45.0, refJ.angle.getDegrees(), EPSILON));

    var angleK = Rotation2d.fromDegrees(-135.0);
    var refK = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refK.cosineScale(angleK);

    assertAll(
        () -> assertEquals(2.0, refK.speed, EPSILON),
        () -> assertEquals(45.0, refK.angle.getDegrees(), EPSILON));

    var angleL = Rotation2d.fromDegrees(180.0);
    var refL = new SwerveModuleState(-2.0, Rotation2d.fromDegrees(45.0));
    refL.cosineScale(angleL);

    assertAll(
        () -> assertEquals(Math.sqrt(2.0), refL.speed, EPSILON),
        () -> assertEquals(45.0, refL.angle.getDegrees(), EPSILON));
  }
}
