// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

class DARETest {
  public static void assertMatrixEqual(SimpleMatrix A, SimpleMatrix B) {
    for (int i = 0; i < A.numRows(); i++) {
      for (int j = 0; j < A.numCols(); j++) {
        assertEquals(A.get(i, j), B.get(i, j), 1e-4);
      }
    }
  }

  void assertDARESolution(
      SimpleMatrix A, SimpleMatrix B, SimpleMatrix Q, SimpleMatrix R, SimpleMatrix X) {
    // Check that X is the solution to the DARE
    // Y = AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
    var Y =
        (A.transpose().mult(X).mult(A))
            .minus(X)
            .minus(
                (A.transpose().mult(X).mult(B))
                    .mult((B.transpose().mult(X).mult(B).plus(R)).invert())
                    .mult(B.transpose().mult(X).mult(A)))
            .plus(Q);
    assertMatrixEqual(new SimpleMatrix(Y.numRows(), Y.numCols()), Y);
  }

  void assertDARESolution(
      SimpleMatrix A,
      SimpleMatrix B,
      SimpleMatrix Q,
      SimpleMatrix R,
      SimpleMatrix N,
      SimpleMatrix X) {
    // Check that X is the solution to the DARE
    // Y = AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0
    var Y =
        (A.transpose().mult(X).mult(A))
            .minus(X)
            .minus(
                (A.transpose().mult(X).mult(B).plus(N))
                    .mult((B.transpose().mult(X).mult(B).plus(R)).invert())
                    .mult(B.transpose().mult(X).mult(A).plus(N.transpose())))
            .plus(Q);
    assertMatrixEqual(new SimpleMatrix(Y.numRows(), Y.numCols()), Y);
  }

  @Test
  void testNonInvertibleA_ABQR() {
    // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
    // Riccati Equation"

    var A =
        new SimpleMatrix(
            4, 4, true, new double[] {0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0});
    var B = new SimpleMatrix(4, 1, true, new double[] {0, 0, 0, 1});
    var Q =
        new SimpleMatrix(4, 4, true, new double[] {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    var R = new SimpleMatrix(1, 1, true, new double[] {0.25});

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testNonInvertibleA_ABQRN() {
    // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
    // Riccati Equation"

    var A =
        new SimpleMatrix(
            4, 4, true, new double[] {0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0});
    var B = new SimpleMatrix(4, 1, true, new double[] {0, 0, 0, 1});
    var Q =
        new SimpleMatrix(4, 4, true, new double[] {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    var R = new SimpleMatrix(1, 1, true, new double[] {0.25});

    var Aref =
        new SimpleMatrix(
            4, 4, true, new double[] {0.25, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0});
    Q = A.minus(Aref).transpose().mult(Q).mult(A.minus(Aref));
    R = B.transpose().mult(Q).mult(B).plus(R);
    var N = A.minus(Aref).transpose().mult(Q).mult(B);

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testInvertibleA_ABQR() {
    var A = new SimpleMatrix(2, 2, true, new double[] {1, 1, 0, 1});
    var B = new SimpleMatrix(2, 1, true, new double[] {0, 1});
    var Q = new SimpleMatrix(2, 2, true, new double[] {1, 0, 0, 0});
    var R = new SimpleMatrix(1, 1, true, new double[] {0.3});

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testInvertibleA_ABQRN() {
    var A = new SimpleMatrix(2, 2, true, new double[] {1, 1, 0, 1});
    var B = new SimpleMatrix(2, 1, true, new double[] {0, 1});
    var Q = new SimpleMatrix(2, 2, true, new double[] {1, 0, 0, 0});
    var R = new SimpleMatrix(1, 1, true, new double[] {0.3});

    var Aref = new SimpleMatrix(2, 2, true, new double[] {0.5, 1, 0, 1});
    Q = A.minus(Aref).transpose().mult(Q).mult(A.minus(Aref));
    R = B.transpose().mult(Q).mult(B).plus(R);
    var N = A.minus(Aref).transpose().mult(Q).mult(B);

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testFirstGeneralizedEigenvalueOfSTIsStable_ABQR() {
    // The first generalized eigenvalue of (S, T) is stable

    var A = new SimpleMatrix(2, 2, true, new double[] {0, 1, 0, 0});
    var B = new SimpleMatrix(2, 1, true, new double[] {0, 1});
    var Q = new SimpleMatrix(2, 2, true, new double[] {1, 0, 0, 1});
    var R = new SimpleMatrix(1, 1, true, new double[] {1});

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testFirstGeneralizedEigenvalueOfSTIsStable_ABQRN() {
    // The first generalized eigenvalue of (S, T) is stable

    var A = new SimpleMatrix(2, 2, true, new double[] {0, 1, 0, 0});
    var B = new SimpleMatrix(2, 1, true, new double[] {0, 1});
    var Q = new SimpleMatrix(2, 2, true, new double[] {1, 0, 0, 1});
    var R = new SimpleMatrix(1, 1, true, new double[] {1});

    var Aref = new SimpleMatrix(2, 2, true, new double[] {0, 0.5, 0, 0});
    Q = A.minus(Aref).transpose().mult(Q).mult(A.minus(Aref));
    R = B.transpose().mult(Q).mult(B).plus(R);
    var N = A.minus(Aref).transpose().mult(Q).mult(B);

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testIdentitySystem_ABQR() {
    var A = SimpleMatrix.identity(2);
    var B = SimpleMatrix.identity(2);
    var Q = SimpleMatrix.identity(2);
    var R = SimpleMatrix.identity(2);

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testIdentitySystem_ABQRN() {
    var A = SimpleMatrix.identity(2);
    var B = SimpleMatrix.identity(2);
    var Q = SimpleMatrix.identity(2);
    var R = SimpleMatrix.identity(2);
    var N = SimpleMatrix.identity(2);

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }
}
