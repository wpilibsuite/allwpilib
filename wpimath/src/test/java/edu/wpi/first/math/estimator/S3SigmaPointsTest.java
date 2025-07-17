// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.numbers.N2;
import org.junit.jupiter.api.Test;

class S3SigmaPointsTest {
  @Test
  void testSimplex() {
    double alpha = 1e-3;
    double beta = 2;
    Nat<N2> N = Nat.N2();

    var sigmaPoints = new S3SigmaPoints<>(N, alpha, beta);
    var points = sigmaPoints.squareRootSigmaPoints(new Vector<>(N), Matrix.eye(N));

    var v1 = new Vector<>(points.extractColumnVector(1));
    var v2 = new Vector<>(points.extractColumnVector(2));
    var v3 = new Vector<>(points.extractColumnVector(3));

    assertAll(
        () -> assertEquals(alpha * Math.sqrt(N.getNum()), v1.norm(), 1e-15),
        () -> assertEquals(alpha * Math.sqrt(N.getNum()), v2.norm(), 1e-15),
        () -> assertEquals(alpha * Math.sqrt(N.getNum()), v3.norm(), 1e-15),
        () -> assertEquals(v1.minus(v2).norm(), v1.minus(v3).norm(), 1e-15),
        () -> assertEquals(v1.minus(v2).norm(), v2.minus(v3).norm(), 1e-15));
  }

  @Test
  void testZeroMeanPoints() {
    var sigmaPoints = new S3SigmaPoints<>(Nat.N2());
    var points =
        sigmaPoints.squareRootSigmaPoints(
            VecBuilder.fill(0, 0), MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 1));

    assertTrue(
        points.isEqual(
            MatBuilder.fill(
                Nat.N2(),
                Nat.N4(),
                0.0,
                -0.00122474,
                0.00122474,
                0.0,
                0.0,
                -0.00070711,
                -0.00070711,
                0.00141421),
            1E-6));
  }

  @Test
  void testNonzeroMeanPoints() {
    var sigmaPoints = new S3SigmaPoints<>(Nat.N2());
    var points =
        sigmaPoints.squareRootSigmaPoints(
            VecBuilder.fill(1, 2), MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, Math.sqrt(10)));

    assertTrue(
        points.isEqual(
            MatBuilder.fill(
                Nat.N2(),
                Nat.N4(),
                1.0,
                0.99877526,
                1.00122474,
                1.0,
                2.0,
                1.99776393,
                1.99776393,
                2.00447214),
            1E-6));
  }
}
