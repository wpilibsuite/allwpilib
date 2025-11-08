// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class Rotation3dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testGimbalLockAccuracy() {
    var rot1 = new Rotation3d(0, 0, Math.PI / 2);
    var rot2 = new Rotation3d(Math.PI, 0, 0);
    var rot3 = new Rotation3d(-Math.PI / 2, 0, 0);
    final var result1 = rot1.plus(rot2).plus(rot3);
    final var expected1 = new Rotation3d(0, -Math.PI / 2, Math.PI / 2);
    assertAll(
        () -> assertEquals(expected1, result1),
        () -> assertEquals(Math.PI / 2, result1.getX() + result1.getZ(), kEpsilon),
        () -> assertEquals(-Math.PI / 2, result1.getY(), kEpsilon));

    rot1 = new Rotation3d(0, 0, Math.PI / 2);
    rot2 = new Rotation3d(-Math.PI, 0, 0);
    rot3 = new Rotation3d(Math.PI / 2, 0, 0);
    final var result2 = rot1.plus(rot2).plus(rot3);
    final var expected2 = new Rotation3d(0, Math.PI / 2, Math.PI / 2);
    assertAll(
        () -> assertEquals(expected2, result2),
        () -> assertEquals(Math.PI / 2, result2.getZ() - result2.getX(), kEpsilon),
        () -> assertEquals(Math.PI / 2, result2.getY(), kEpsilon));

    rot1 = new Rotation3d(0, 0, Math.PI / 2);
    rot2 = new Rotation3d(0, Math.PI / 3, 0);
    rot3 = new Rotation3d(-Math.PI / 2, 0, 0);
    final var result3 = rot1.plus(rot2).plus(rot3);
    final var expected3 = new Rotation3d(0, Math.PI / 2, Math.PI / 6);
    assertAll(
        () -> assertEquals(expected3, result3),
        () -> assertEquals(Math.PI / 6, result3.getZ() - result3.getX(), kEpsilon),
        () -> assertEquals(Math.PI / 2, result3.getY(), kEpsilon));
  }

  @Test
  void testInitAxisAngleAndRollPitchYaw() {
    final var xAxis = VecBuilder.fill(1.0, 0.0, 0.0);
    final var rot1 = new Rotation3d(xAxis, Math.PI / 3);
    final var rot2 = new Rotation3d(Math.PI / 3, 0.0, 0.0);
    final var rvec1 = new Rotation3d(xAxis.times(Math.PI / 3));
    assertEquals(rot1, rot2);
    assertEquals(rot1, rvec1);

    final var yAxis = VecBuilder.fill(0.0, 1.0, 0.0);
    final var rot3 = new Rotation3d(yAxis, Math.PI / 3);
    final var rot4 = new Rotation3d(0.0, Math.PI / 3, 0.0);
    final var rvec2 = new Rotation3d(yAxis.times(Math.PI / 3));
    assertEquals(rot3, rot4);
    assertEquals(rot3, rvec2);

    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);
    final var rot5 = new Rotation3d(zAxis, Math.PI / 3);
    final var rot6 = new Rotation3d(0.0, 0.0, Math.PI / 3);
    final var rvec3 = new Rotation3d(zAxis.times(Math.PI / 3));
    assertEquals(rot5, rot6);
    assertEquals(rot5, rvec3);
  }

  @Test
  void testInitRotationMatrix() {
    // No rotation
    final var R1 = Matrix.eye(Nat.N3());
    final var rot1 = new Rotation3d(R1);
    assertEquals(Rotation3d.kZero, rot1);

    // 90 degree CCW rotation around z-axis
    final var R2 = new Matrix<>(Nat.N3(), Nat.N3());
    R2.assignBlock(0, 0, VecBuilder.fill(0.0, 1.0, 0.0));
    R2.assignBlock(0, 1, VecBuilder.fill(-1.0, 0.0, 0.0));
    R2.assignBlock(0, 2, VecBuilder.fill(0.0, 0.0, 1.0));
    final var rot2 = new Rotation3d(R2);
    final var expected2 = new Rotation3d(0.0, 0.0, Units.degreesToRadians(90.0));
    assertEquals(expected2, rot2);

    // Matrix that isn't orthogonal
    final var R3 = MatBuilder.fill(Nat.N3(), Nat.N3(), 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    assertThrows(IllegalArgumentException.class, () -> new Rotation3d(R3));

    // Matrix that's orthogonal but not special orthogonal
    final var R4 = Matrix.eye(Nat.N3()).times(2.0);
    assertThrows(IllegalArgumentException.class, () -> new Rotation3d(R4));
  }

  @Test
  void testInitTwoVector() {
    final var xAxis = VecBuilder.fill(1.0, 0.0, 0.0);
    final var yAxis = VecBuilder.fill(0.0, 1.0, 0.0);
    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    // 90 degree CW rotation around y-axis
    final var rot1 = new Rotation3d(xAxis, zAxis);
    final var expected1 = new Rotation3d(yAxis, -Math.PI / 2.0);
    assertEquals(expected1, rot1);

    // 45 degree CCW rotation around z-axis
    final var rot2 = new Rotation3d(xAxis, VecBuilder.fill(1.0, 1.0, 0.0));
    final var expected2 = new Rotation3d(zAxis, Math.PI / 4.0);
    assertEquals(expected2, rot2);

    // 0 degree rotation of x-axes
    final var rot3 = new Rotation3d(xAxis, xAxis);
    assertEquals(Rotation3d.kZero, rot3);

    // 0 degree rotation of y-axes
    final var rot4 = new Rotation3d(yAxis, yAxis);
    assertEquals(Rotation3d.kZero, rot4);

    // 0 degree rotation of z-axes
    final var rot5 = new Rotation3d(zAxis, zAxis);
    assertEquals(Rotation3d.kZero, rot5);

    // 180 degree rotation tests. For 180 degree rotations, any quaternion with
    // an orthogonal rotation axis is acceptable. The rotation axis and initial
    // vector are orthogonal if their dot product is zero.

    // 180 degree rotation of x-axes
    final var rot6 = new Rotation3d(xAxis, xAxis.times(-1.0));
    final var q6 = rot6.getQuaternion();
    assertEquals(0.0, q6.getW());
    assertEquals(
        0.0,
        q6.getX() * xAxis.get(0, 0) + q6.getY() * xAxis.get(1, 0) + q6.getZ() * xAxis.get(2, 0));

    // 180 degree rotation of y-axes
    final var rot7 = new Rotation3d(yAxis, yAxis.times(-1.0));
    final var q7 = rot7.getQuaternion();
    assertEquals(0.0, q7.getW());
    assertEquals(
        0.0,
        q7.getX() * yAxis.get(0, 0) + q7.getY() * yAxis.get(1, 0) + q7.getZ() * yAxis.get(2, 0));

    // 180 degree rotation of z-axes
    final var rot8 = new Rotation3d(zAxis, zAxis.times(-1.0));
    final var q8 = rot8.getQuaternion();
    assertEquals(0.0, q8.getW());
    assertEquals(
        0.0,
        q8.getX() * zAxis.get(0, 0) + q8.getY() * zAxis.get(1, 0) + q8.getZ() * zAxis.get(2, 0));
  }

  @Test
  void testRadiansToDegrees() {
    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var rot1 = new Rotation3d(zAxis, Math.PI / 3);
    assertAll(
        () -> assertEquals(Units.degreesToRadians(0.0), rot1.getX(), kEpsilon),
        () -> assertEquals(Units.degreesToRadians(0.0), rot1.getY(), kEpsilon),
        () -> assertEquals(Units.degreesToRadians(60.0), rot1.getZ(), kEpsilon));

    var rot2 = new Rotation3d(zAxis, Math.PI / 4);
    assertAll(
        () -> assertEquals(Units.degreesToRadians(0.0), rot2.getX(), kEpsilon),
        () -> assertEquals(Units.degreesToRadians(0.0), rot2.getY(), kEpsilon),
        () -> assertEquals(Units.degreesToRadians(45.0), rot2.getZ(), kEpsilon));
  }

  @Test
  void testRadiansAndDegrees() {
    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var rot1 = new Rotation3d(zAxis, Units.degreesToRadians(45.0));
    assertAll(
        () -> assertEquals(0.0, rot1.getX(), kEpsilon),
        () -> assertEquals(0.0, rot1.getY(), kEpsilon),
        () -> assertEquals(Math.PI / 4.0, rot1.getZ(), kEpsilon));

    var rot2 = new Rotation3d(zAxis, Units.degreesToRadians(30.0));
    assertAll(
        () -> assertEquals(0.0, rot2.getX(), kEpsilon),
        () -> assertEquals(0.0, rot2.getY(), kEpsilon),
        () -> assertEquals(Math.PI / 6.0, rot2.getZ(), kEpsilon));
  }

  @Test
  void testRotationLoop() {
    var rot = Rotation3d.kZero;

    rot = rot.plus(new Rotation3d(Units.degreesToRadians(90.0), 0.0, 0.0));
    var expected = new Rotation3d(Units.degreesToRadians(90.0), 0.0, 0.0);
    assertEquals(expected, rot);

    rot = rot.plus(new Rotation3d(0.0, Units.degreesToRadians(90.0), 0.0));
    expected =
        new Rotation3d(
            VecBuilder.fill(1.0 / Math.sqrt(3), 1.0 / Math.sqrt(3), -1.0 / Math.sqrt(3)),
            Units.degreesToRadians(120.0));
    assertEquals(expected, rot);

    rot = rot.plus(new Rotation3d(0.0, 0.0, Units.degreesToRadians(90.0)));
    expected = new Rotation3d(0.0, Units.degreesToRadians(90.0), 0.0);
    assertEquals(expected, rot);

    rot = rot.plus(new Rotation3d(0.0, Units.degreesToRadians(-90.0), 0.0));
    assertEquals(Rotation3d.kZero, rot);
  }

  @Test
  void testRotateByFromZeroX() {
    final var xAxis = VecBuilder.fill(1.0, 0.0, 0.0);

    final var zero = Rotation3d.kZero;
    var rotated = zero.rotateBy(new Rotation3d(xAxis, Units.degreesToRadians(90.0)));

    var expected = new Rotation3d(xAxis, Units.degreesToRadians(90.0));
    assertEquals(expected, rotated);
  }

  @Test
  void testRotateByFromZeroY() {
    final var yAxis = VecBuilder.fill(0.0, 1.0, 0.0);

    final var zero = Rotation3d.kZero;
    var rotated = zero.rotateBy(new Rotation3d(yAxis, Units.degreesToRadians(90.0)));

    var expected = new Rotation3d(yAxis, Units.degreesToRadians(90.0));
    assertEquals(expected, rotated);
  }

  @Test
  void testRotateByFromZeroZ() {
    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    final var zero = Rotation3d.kZero;
    var rotated = zero.rotateBy(new Rotation3d(zAxis, Units.degreesToRadians(90.0)));

    var expected = new Rotation3d(zAxis, Units.degreesToRadians(90.0));
    assertEquals(expected, rotated);
  }

  @Test
  void testRotateByNonZeroX() {
    final var xAxis = VecBuilder.fill(1.0, 0.0, 0.0);

    var rot = new Rotation3d(xAxis, Units.degreesToRadians(90.0));
    rot = rot.plus(new Rotation3d(xAxis, Units.degreesToRadians(30.0)));

    var expected = new Rotation3d(xAxis, Units.degreesToRadians(120.0));
    assertEquals(expected, rot);
  }

  @Test
  void testRotateByNonZeroY() {
    final var yAxis = VecBuilder.fill(0.0, 1.0, 0.0);

    var rot = new Rotation3d(yAxis, Units.degreesToRadians(90.0));
    rot = rot.plus(new Rotation3d(yAxis, Units.degreesToRadians(30.0)));

    var expected = new Rotation3d(yAxis, Units.degreesToRadians(120.0));
    assertEquals(expected, rot);
  }

  @Test
  void testRotateByNonZeroZ() {
    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var rot = new Rotation3d(zAxis, Units.degreesToRadians(90.0));
    rot = rot.plus(new Rotation3d(zAxis, Units.degreesToRadians(30.0)));

    var expected = new Rotation3d(zAxis, Units.degreesToRadians(120.0));
    assertEquals(expected, rot);
  }

  @Test
  void testMinus() {
    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var rot1 = new Rotation3d(zAxis, Units.degreesToRadians(70.0));
    var rot2 = new Rotation3d(zAxis, Units.degreesToRadians(30.0));

    assertEquals(rot1.minus(rot2).getZ(), Units.degreesToRadians(40.0), kEpsilon);
  }

  @Test
  void testEquality() {
    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var rot1 = new Rotation3d(zAxis, Units.degreesToRadians(43.0));
    var rot2 = new Rotation3d(zAxis, Units.degreesToRadians(43.0));
    assertEquals(rot1, rot2);

    rot1 = new Rotation3d(zAxis, Units.degreesToRadians(-180.0));
    rot2 = new Rotation3d(zAxis, Units.degreesToRadians(180.0));
    assertEquals(rot1, rot2);
  }

  @Test
  void testAxisAngle() {
    final var xAxis = VecBuilder.fill(1.0, 0.0, 0.0);
    final var yAxis = VecBuilder.fill(0.0, 1.0, 0.0);
    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var rot1 = new Rotation3d(xAxis, Units.degreesToRadians(90.0));
    assertEquals(xAxis, rot1.getAxis());
    assertEquals(Math.PI / 2.0, rot1.getAngle(), 1e-9);

    var rot2 = new Rotation3d(yAxis, Units.degreesToRadians(45.0));
    assertEquals(yAxis, rot2.getAxis());
    assertEquals(Math.PI / 4.0, rot2.getAngle(), 1e-9);

    var rot3 = new Rotation3d(zAxis, Units.degreesToRadians(60.0));
    assertEquals(zAxis, rot3.getAxis());
    assertEquals(Math.PI / 3.0, rot3.getAngle(), 1e-9);
  }

  @Test
  void testToRotation2d() {
    var rotation =
        new Rotation3d(
            Units.degreesToRadians(20.0),
            Units.degreesToRadians(30.0),
            Units.degreesToRadians(40.0));
    var expected = new Rotation2d(Units.degreesToRadians(40.0));

    assertEquals(expected, rotation.toRotation2d());
  }

  @Test
  void testInequality() {
    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var rot1 = new Rotation3d(zAxis, Units.degreesToRadians(43.0));
    var rot2 = new Rotation3d(zAxis, Units.degreesToRadians(43.5));
    assertNotEquals(rot1, rot2);
  }

  @Test
  void testToMatrix() {
    var before =
        new Rotation3d(
            Units.degreesToRadians(10.0),
            Units.degreesToRadians(20.0),
            Units.degreesToRadians(30.0));
    var after = new Rotation3d(before.toMatrix());

    assertEquals(before, after);
  }

  @Test
  void testInterpolate() {
    final var xAxis = VecBuilder.fill(1.0, 0.0, 0.0);
    final var yAxis = VecBuilder.fill(0.0, 1.0, 0.0);
    final var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    // 50 + (70 - 50) * 0.5 = 60
    var rot1 = new Rotation3d(xAxis, Units.degreesToRadians(50));
    var rot2 = new Rotation3d(xAxis, Units.degreesToRadians(70));
    var interpolated = rot1.interpolate(rot2, 0.5);
    assertEquals(Units.degreesToRadians(60.0), interpolated.getX(), kEpsilon);
    assertEquals(Units.degreesToRadians(0.0), interpolated.getY(), kEpsilon);
    assertEquals(Units.degreesToRadians(0.0), interpolated.getZ(), kEpsilon);

    // -160 minus half distance between 170 and -160 (15) = -175
    rot1 = new Rotation3d(xAxis, Units.degreesToRadians(170));
    rot2 = new Rotation3d(xAxis, Units.degreesToRadians(-160));
    interpolated = rot1.interpolate(rot2, 0.5);
    assertEquals(Units.degreesToRadians(-175.0), interpolated.getX(), kEpsilon);
    assertEquals(Units.degreesToRadians(0.0), interpolated.getY(), kEpsilon);
    assertEquals(Units.degreesToRadians(0.0), interpolated.getZ(), kEpsilon);

    // 50 + (70 - 50) * 0.5 = 60
    rot1 = new Rotation3d(yAxis, Units.degreesToRadians(50));
    rot2 = new Rotation3d(yAxis, Units.degreesToRadians(70));
    interpolated = rot1.interpolate(rot2, 0.5);
    assertEquals(Units.degreesToRadians(0.0), interpolated.getX(), kEpsilon);
    assertEquals(Units.degreesToRadians(60.0), interpolated.getY(), kEpsilon);
    assertEquals(Units.degreesToRadians(0.0), interpolated.getZ(), kEpsilon);

    // -160 minus half distance between 170 and -160 (165) = 5
    rot1 = new Rotation3d(yAxis, Units.degreesToRadians(170));
    rot2 = new Rotation3d(yAxis, Units.degreesToRadians(-160));
    interpolated = rot1.interpolate(rot2, 0.5);
    assertEquals(Units.degreesToRadians(180.0), interpolated.getX(), kEpsilon);
    assertEquals(Units.degreesToRadians(-5.0), interpolated.getY(), kEpsilon);
    assertEquals(Units.degreesToRadians(180.0), interpolated.getZ(), kEpsilon);

    // 50 + (70 - 50) * 0.5 = 60
    rot1 = new Rotation3d(zAxis, Units.degreesToRadians(50));
    rot2 = new Rotation3d(zAxis, Units.degreesToRadians(70));
    interpolated = rot1.interpolate(rot2, 0.5);
    assertEquals(Units.degreesToRadians(0.0), interpolated.getX(), kEpsilon);
    assertEquals(Units.degreesToRadians(0.0), interpolated.getY(), kEpsilon);
    assertEquals(Units.degreesToRadians(60.0), interpolated.getZ(), kEpsilon);

    // -160 minus half distance between 170 and -160 (15) = -175
    rot1 = new Rotation3d(zAxis, Units.degreesToRadians(170));
    rot2 = new Rotation3d(zAxis, Units.degreesToRadians(-160));
    interpolated = rot1.interpolate(rot2, 0.5);
    assertEquals(Units.degreesToRadians(0.0), interpolated.getX(), kEpsilon);
    assertEquals(Units.degreesToRadians(0.0), interpolated.getY(), kEpsilon);
    assertEquals(Units.degreesToRadians(-175.0), interpolated.getZ(), kEpsilon);
  }
}
