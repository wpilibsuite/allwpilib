// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.wpilib.math.util.UnitConversions.degreesToRadians;

import org.junit.jupiter.api.Test;

class QuaternionTest {
  @Test
  void testInit() {
    // Identity
    var q1 = new Quaternion();
    assertAll(
        () -> assertEquals(1.0, q1.w),
        () -> assertEquals(0.0, q1.x),
        () -> assertEquals(0.0, q1.y),
        () -> assertEquals(0.0, q1.z));

    // Normalized
    var q2 = new Quaternion(0.5, 0.5, 0.5, 0.5);
    assertAll(
        () -> assertEquals(0.5, q2.w),
        () -> assertEquals(0.5, q2.x),
        () -> assertEquals(0.5, q2.y),
        () -> assertEquals(0.5, q2.z));

    // Unnormalized
    var q3 = new Quaternion(0.75, 0.3, 0.4, 0.5);
    assertAll(
        () -> assertEquals(0.75, q3.w),
        () -> assertEquals(0.3, q3.x),
        () -> assertEquals(0.4, q3.y),
        () -> assertEquals(0.5, q3.z));

    var q3_norm = q3.normalize();
    double norm = Math.sqrt(0.75 * 0.75 + 0.3 * 0.3 + 0.4 * 0.4 + 0.5 * 0.5);
    assertAll(
        () -> assertEquals(0.75 / norm, q3_norm.w),
        () -> assertEquals(0.3 / norm, q3_norm.x),
        () -> assertEquals(0.4 / norm, q3_norm.y),
        () -> assertEquals(0.5 / norm, q3_norm.z),
        () -> assertEquals(1.0, q3_norm.dot(q3_norm)));
  }

  @Test
  void testAddition() {
    var q = new Quaternion(0.1, 0.2, 0.3, 0.4);
    var p = new Quaternion(0.5, 0.6, 0.7, 0.8);

    var sum = q.plus(p);
    assertAll(
        () -> assertEquals(q.w + p.w, sum.w),
        () -> assertEquals(q.x + p.x, sum.x),
        () -> assertEquals(q.y + p.y, sum.y),
        () -> assertEquals(q.z + p.z, sum.z));
  }

  @Test
  void testSubtraction() {
    var q = new Quaternion(0.1, 0.2, 0.3, 0.4);
    var p = new Quaternion(0.5, 0.6, 0.7, 0.8);

    var difference = q.minus(p);

    assertAll(
        () -> assertEquals(q.w - p.w, difference.w),
        () -> assertEquals(q.x - p.x, difference.x),
        () -> assertEquals(q.y - p.y, difference.y),
        () -> assertEquals(q.z - p.z, difference.z));
  }

  @Test
  void testScalarMultiplication() {
    var q = new Quaternion(0.1, 0.2, 0.3, 0.4);
    var scalar = 2;

    var product = q.times(scalar);

    assertAll(
        () -> assertEquals(q.w * scalar, product.w),
        () -> assertEquals(q.x * scalar, product.x),
        () -> assertEquals(q.y * scalar, product.y),
        () -> assertEquals(q.z * scalar, product.z));
  }

  @Test
  void testScalarDivision() {
    var q = new Quaternion(0.1, 0.2, 0.3, 0.4);
    var scalar = 2;

    var product = q.divide(scalar);

    assertAll(
        () -> assertEquals(q.w / scalar, product.w),
        () -> assertEquals(q.x / scalar, product.x),
        () -> assertEquals(q.y / scalar, product.y),
        () -> assertEquals(q.z / scalar, product.z));
  }

  @Test
  void testTimes() {
    // 90° CCW rotations around each axis
    double c = Math.cos(degreesToRadians(90.0) / 2.0);
    double s = Math.sin(degreesToRadians(90.0) / 2.0);
    var xRot = new Quaternion(c, s, 0.0, 0.0);
    var yRot = new Quaternion(c, 0.0, s, 0.0);
    var zRot = new Quaternion(c, 0.0, 0.0, s);

    // 90° CCW X rotation, 90° CCW Y rotation, and 90° CCW Z rotation should
    // produce a 90° CCW Y rotation
    var expected = yRot;
    final var actual = zRot.times(yRot).times(xRot);
    assertAll(
        () -> assertEquals(expected.w, actual.w, 1e-9),
        () -> assertEquals(expected.x, actual.x, 1e-9),
        () -> assertEquals(expected.y, actual.y, 1e-9),
        () -> assertEquals(expected.z, actual.z, 1e-9));

    // Identity
    var q =
        new Quaternion(
            0.7276068751089989, 0.29104275004359953, 0.38805700005813276, 0.48507125007266594);
    final var actual2 = q.times(q.inverse());
    assertAll(
        () -> assertEquals(1.0, actual2.w),
        () -> assertEquals(0.0, actual2.x),
        () -> assertEquals(0.0, actual2.y),
        () -> assertEquals(0.0, actual2.z));
  }

  @Test
  void testConjugate() {
    var q = new Quaternion(0.75, 0.3, 0.4, 0.5);
    var inv = q.conjugate();

    assertAll(
        () -> assertEquals(q.w, inv.w),
        () -> assertEquals(-q.x, inv.x),
        () -> assertEquals(-q.y, inv.y),
        () -> assertEquals(-q.z, inv.z));
  }

  @Test
  void testInverse() {
    var q = new Quaternion(0.75, 0.3, 0.4, 0.5);
    var inv = q.inverse();
    var norm = q.norm();

    assertAll(
        () -> assertEquals(q.w / (norm * norm), inv.w, 1e-10),
        () -> assertEquals(-q.x / (norm * norm), inv.x, 1e-10),
        () -> assertEquals(-q.y / (norm * norm), inv.y, 1e-10),
        () -> assertEquals(-q.z / (norm * norm), inv.z, 1e-10));
  }

  @Test
  void testNorm() {
    var q = new Quaternion(3, 4, 12, 84);

    // pythagorean triples (3, 4, 5), (5, 12, 13), (13, 84, 85)
    assertEquals(q.norm(), 85, 1e-10);
  }

  @Test
  void testExponential() {
    var q = new Quaternion(1.1, 2.2, 3.3, 4.4);
    var q_exp =
        new Quaternion(
            2.81211398529184, -0.392521193481878, -0.588781790222817, -0.785042386963756);

    assertEquals(q_exp, q.exp());
  }

  @Test
  void testLogarithm() {
    var q = new Quaternion(1.1, 2.2, 3.3, 4.4);
    var q_log =
        new Quaternion(1.7959088706354, 0.515190292664085, 0.772785438996128, 1.03038058532817);

    assertEquals(q_log, q.log());

    var zero = new Quaternion(0, 0, 0, 0);
    var one = new Quaternion();

    assertEquals(zero, zero.log());
    assertEquals(zero, one.log());

    var i = new Quaternion(0, 1, 0, 0);
    assertEquals(i.times(Math.PI / 2), i.log());

    var j = new Quaternion(0, 0, 1, 0);
    assertEquals(j.times(Math.PI / 2), j.log());

    var k = new Quaternion(0, 0, 0, 1);
    assertEquals(k.times(Math.PI / 2), k.log());
    assertEquals(i.times(-Math.PI), one.times(-1).log());

    var ln_half = Math.log(0.5);
    assertEquals(new Quaternion(ln_half, -Math.PI, 0, 0), one.times(-0.5).log());
  }

  @Test
  void testLogarithmIsInverseOfExponential() {
    var q = new Quaternion(1.1, 2.2, 3.3, 4.4);

    // These operations are order-dependent: ln(exp(q)) is congruent
    // but not necessarily equal to exp(ln(q)) due to the multi-valued nature of the complex
    // logarithm.

    var q_log_exp = q.log().exp();

    assertEquals(q, q_log_exp);

    var start = new Quaternion(1, 2, 3, 4);
    var expect = new Quaternion(5, 6, 7, 8);

    var twist = expect.times(start.inverse()).log();
    var actual = twist.exp().times(start);

    assertEquals(expect, actual);
  }

  @Test
  void testDotProduct() {
    var q = new Quaternion(1.1, 2.2, 3.3, 4.4);
    var p = new Quaternion(5.5, 6.6, 7.7, 8.8);

    assertEquals(q.w * p.w + q.x * p.x + q.y * p.y + q.z * p.z, q.dot(p));
  }

  @Test
  void testDotProductAsEquality() {
    var q = new Quaternion(1.1, 2.2, 3.3, 4.4);
    var q_conj = q.conjugate();

    assertAll(() -> assertEquals(q, q), () -> assertNotEquals(q, q_conj));
  }
}
