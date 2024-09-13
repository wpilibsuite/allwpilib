// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class QuaternionTest {
  @Test
  void testInit() {
    // Identity
    var q1 = new Quaternion();
    assertAll(
        () -> assertEquals(1.0, q1.getW()),
        () -> assertEquals(0.0, q1.getX()),
        () -> assertEquals(0.0, q1.getY()),
        () -> assertEquals(0.0, q1.getZ()));

    // Normalized
    var q2 = new Quaternion(0.5, 0.5, 0.5, 0.5);
    assertAll(
        () -> assertEquals(0.5, q2.getW()),
        () -> assertEquals(0.5, q2.getX()),
        () -> assertEquals(0.5, q2.getY()),
        () -> assertEquals(0.5, q2.getZ()));

    // Unnormalized
    var q3 = new Quaternion(0.75, 0.3, 0.4, 0.5);
    assertAll(
        () -> assertEquals(0.75, q3.getW()),
        () -> assertEquals(0.3, q3.getX()),
        () -> assertEquals(0.4, q3.getY()),
        () -> assertEquals(0.5, q3.getZ()));

    var q3_norm = q3.normalize();
    double norm = Math.sqrt(0.75 * 0.75 + 0.3 * 0.3 + 0.4 * 0.4 + 0.5 * 0.5);
    assertAll(
        () -> assertEquals(0.75 / norm, q3_norm.getW()),
        () -> assertEquals(0.3 / norm, q3_norm.getX()),
        () -> assertEquals(0.4 / norm, q3_norm.getY()),
        () -> assertEquals(0.5 / norm, q3_norm.getZ()),
        () -> assertEquals(1.0, q3_norm.dot(q3_norm)));
  }

  @Test
  void testAddition() {
    var q = new Quaternion(0.1, 0.2, 0.3, 0.4);
    var p = new Quaternion(0.5, 0.6, 0.7, 0.8);

    var sum = q.plus(p);
    assertAll(
        () -> assertEquals(q.getW() + p.getW(), sum.getW()),
        () -> assertEquals(q.getX() + p.getX(), sum.getX()),
        () -> assertEquals(q.getY() + p.getY(), sum.getY()),
        () -> assertEquals(q.getZ() + p.getZ(), sum.getZ()));
  }

  @Test
  void testSubtraction() {
    var q = new Quaternion(0.1, 0.2, 0.3, 0.4);
    var p = new Quaternion(0.5, 0.6, 0.7, 0.8);

    var difference = q.minus(p);

    assertAll(
        () -> assertEquals(q.getW() - p.getW(), difference.getW()),
        () -> assertEquals(q.getX() - p.getX(), difference.getX()),
        () -> assertEquals(q.getY() - p.getY(), difference.getY()),
        () -> assertEquals(q.getZ() - p.getZ(), difference.getZ()));
  }

  @Test
  void testScalarMultiplication() {
    var q = new Quaternion(0.1, 0.2, 0.3, 0.4);
    var scalar = 2;

    var product = q.times(scalar);

    assertAll(
        () -> assertEquals(q.getW() * scalar, product.getW()),
        () -> assertEquals(q.getX() * scalar, product.getX()),
        () -> assertEquals(q.getY() * scalar, product.getY()),
        () -> assertEquals(q.getZ() * scalar, product.getZ()));
  }

  @Test
  void testScalarDivision() {
    var q = new Quaternion(0.1, 0.2, 0.3, 0.4);
    var scalar = 2;

    var product = q.divide(scalar);

    assertAll(
        () -> assertEquals(q.getW() / scalar, product.getW()),
        () -> assertEquals(q.getX() / scalar, product.getX()),
        () -> assertEquals(q.getY() / scalar, product.getY()),
        () -> assertEquals(q.getZ() / scalar, product.getZ()));
  }

  @Test
  void testTimes() {
    // 90° CCW rotations around each axis
    double c = Math.cos(Units.degreesToRadians(90.0) / 2.0);
    double s = Math.sin(Units.degreesToRadians(90.0) / 2.0);
    var xRot = new Quaternion(c, s, 0.0, 0.0);
    var yRot = new Quaternion(c, 0.0, s, 0.0);
    var zRot = new Quaternion(c, 0.0, 0.0, s);

    // 90° CCW X rotation, 90° CCW Y rotation, and 90° CCW Z rotation should
    // produce a 90° CCW Y rotation
    var expected = yRot;
    final var actual = zRot.times(yRot).times(xRot);
    assertAll(
        () -> assertEquals(expected.getW(), actual.getW(), 1e-9),
        () -> assertEquals(expected.getX(), actual.getX(), 1e-9),
        () -> assertEquals(expected.getY(), actual.getY(), 1e-9),
        () -> assertEquals(expected.getZ(), actual.getZ(), 1e-9));

    // Identity
    var q =
        new Quaternion(
            0.7276068751089989, 0.29104275004359953, 0.38805700005813276, 0.48507125007266594);
    final var actual2 = q.times(q.inverse());
    assertAll(
        () -> assertEquals(1.0, actual2.getW()),
        () -> assertEquals(0.0, actual2.getX()),
        () -> assertEquals(0.0, actual2.getY()),
        () -> assertEquals(0.0, actual2.getZ()));
  }

  @Test
  void testConjugate() {
    var q = new Quaternion(0.75, 0.3, 0.4, 0.5);
    var inv = q.conjugate();

    assertAll(
        () -> assertEquals(q.getW(), inv.getW()),
        () -> assertEquals(-q.getX(), inv.getX()),
        () -> assertEquals(-q.getY(), inv.getY()),
        () -> assertEquals(-q.getZ(), inv.getZ()));
  }

  @Test
  void testInverse() {
    var q = new Quaternion(0.75, 0.3, 0.4, 0.5);
    var inv = q.inverse();
    var norm = q.norm();

    assertAll(
        () -> assertEquals(q.getW() / (norm * norm), inv.getW(), 1e-10),
        () -> assertEquals(-q.getX() / (norm * norm), inv.getX(), 1e-10),
        () -> assertEquals(-q.getY() / (norm * norm), inv.getY(), 1e-10),
        () -> assertEquals(-q.getZ() / (norm * norm), inv.getZ(), 1e-10));
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

    var twist = start.log(expect);
    var actual = start.exp(twist);

    assertEquals(expect, actual);
  }

  @Test
  void testDotProduct() {
    var q = new Quaternion(1.1, 2.2, 3.3, 4.4);
    var p = new Quaternion(5.5, 6.6, 7.7, 8.8);

    assertEquals(
        q.getW() * p.getW() + q.getX() * p.getX() + q.getY() * p.getY() + q.getZ() * p.getZ(),
        q.dot(p));
  }

  @Test
  void testDotProductAsEquality() {
    var q = new Quaternion(1.1, 2.2, 3.3, 4.4);
    var q_conj = q.conjugate();

    assertAll(() -> assertEquals(q, q), () -> assertNotEquals(q, q_conj));
  }
}
