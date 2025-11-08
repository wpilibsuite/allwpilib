// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.wpilibj.UtilityClassTest;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

class DARETest extends UtilityClassTest<DARE> {
  DARETest() {
    super(DARE.class);
  }

  public static <R extends Num, C extends Num> void assertMatrixEqual(
      Matrix<R, C> A, Matrix<R, C> B) {
    for (int i = 0; i < A.getNumRows(); i++) {
      for (int j = 0; j < A.getNumCols(); j++) {
        assertEquals(A.get(i, j), B.get(i, j), 1e-4);
      }
    }
  }

  <States extends Num, Inputs extends Num> void assertDARESolution(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, States> X) {
    // Check that X is the solution to the DARE
    // Y = AᵀXA − X − AᵀXB(BᵀXB + R)⁻¹BᵀXA + Q = 0
    var Y =
        (A.transpose().times(X).times(A))
            .minus(X)
            .minus(
                (A.transpose().times(X).times(B))
                    .times((B.transpose().times(X).times(B).plus(R)).inv())
                    .times(B.transpose().times(X).times(A)))
            .plus(Q);
    assertMatrixEqual(new Matrix<>(new SimpleMatrix(Y.getNumRows(), Y.getNumCols())), Y);
  }

  <States extends Num, Inputs extends Num> void assertDARESolution(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, Inputs> N,
      Matrix<States, States> X) {
    // Check that X is the solution to the DARE
    // Y = AᵀXA − X − (AᵀXB + N)(BᵀXB + R)⁻¹(BᵀXA + Nᵀ) + Q = 0
    var Y =
        (A.transpose().times(X).times(A))
            .minus(X)
            .minus(
                (A.transpose().times(X).times(B).plus(N))
                    .times((B.transpose().times(X).times(B).plus(R)).inv())
                    .times(B.transpose().times(X).times(A).plus(N.transpose())))
            .plus(Q);
    assertMatrixEqual(new Matrix<>(new SimpleMatrix(Y.getNumRows(), Y.getNumCols())), Y);
  }

  @Test
  void testNonInvertibleA_ABQR() {
    // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
    // Riccati Equation"

    var A =
        new Matrix<>(
            Nat.N4(), Nat.N4(), new double[] {0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0});
    var B = new Matrix<>(Nat.N4(), Nat.N1(), new double[] {0, 0, 0, 1});
    var Q =
        new Matrix<>(
            Nat.N4(), Nat.N4(), new double[] {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.25});

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testNonInvertibleA_ABQRN() {
    // Example 2 of "On the Numerical Solution of the Discrete-Time Algebraic
    // Riccati Equation"

    var A =
        new Matrix<>(
            Nat.N4(), Nat.N4(), new double[] {0.5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0});
    var B = new Matrix<>(Nat.N4(), Nat.N1(), new double[] {0, 0, 0, 1});
    var Q =
        new Matrix<>(
            Nat.N4(), Nat.N4(), new double[] {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.25});

    var Aref =
        new Matrix<>(
            Nat.N4(), Nat.N4(), new double[] {0.25, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0});
    Q = A.minus(Aref).transpose().times(Q).times(A.minus(Aref));
    R = B.transpose().times(Q).times(B).plus(R);
    var N = A.minus(Aref).transpose().times(Q).times(B);

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testInvertibleA_ABQR() {
    var A = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 1, 0, 1});
    var B = new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0, 1});
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 0, 0, 0});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.3});

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testInvertibleA_ABQRN() {
    var A = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 1, 0, 1});
    var B = new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0, 1});
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 0, 0, 0});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.3});

    var Aref = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0.5, 1, 0, 1});
    Q = A.minus(Aref).transpose().times(Q).times(A.minus(Aref));
    R = B.transpose().times(Q).times(B).plus(R);
    var N = A.minus(Aref).transpose().times(Q).times(B);

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testFirstGeneralizedEigenvalueOfSTIsStable_ABQR() {
    // The first generalized eigenvalue of (S, T) is stable

    var A = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0, 1, 0, 0});
    var B = new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0, 1});
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 0, 0, 1});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {1});

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testFirstGeneralizedEigenvalueOfSTIsStable_ABQRN() {
    // The first generalized eigenvalue of (S, T) is stable

    var A = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0, 1, 0, 0});
    var B = new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0, 1});
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1, 0, 0, 1});
    var R = new Matrix<>(Nat.N1(), Nat.N1(), new double[] {1});

    var Aref = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0, 0.5, 0, 0});
    Q = A.minus(Aref).transpose().times(Q).times(A.minus(Aref));
    R = B.transpose().times(Q).times(B).plus(R);
    var N = A.minus(Aref).transpose().times(Q).times(B);

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testIdentitySystem_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testIdentitySystem_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());
    var N = Matrix.eye(Nat.N2());

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testMoreInputsThanStates_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = new Matrix<>(Nat.N2(), Nat.N3(), new double[] {1, 0, 0, 0, 0.5, 0.3});
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N3());

    var X = DARE.dare(A, B, Q, R);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, X);
  }

  @Test
  void testMoreInputsThanStates_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = new Matrix<>(Nat.N2(), Nat.N3(), new double[] {1, 0, 0, 0, 0.5, 0.3});
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N3());
    var N = new Matrix<>(Nat.N2(), Nat.N3(), new double[] {1, 0, 0, 0, 1, 0});

    var X = DARE.dare(A, B, Q, R, N);
    assertMatrixEqual(X, X.transpose());
    assertDARESolution(A, B, Q, R, N, X);
  }

  @Test
  void testQNotSymmetric_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1.0, 1.0, 0.0, 1.0});
    var R = Matrix.eye(Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R));
  }

  @Test
  void testQNotPositiveSemidefinite_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {-1.0, 0.0, 0.0, -1.0});
    var R = Matrix.eye(Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R));
  }

  @Test
  void testQNotSymmetric_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1.0, 1.0, 0.0, 1.0});
    var R = Matrix.eye(Nat.N2());
    var N = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {2.0, 0.0, 0.0, 2.0});

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R, N));
  }

  @Test
  void testQNotPositiveSemidefinite_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());
    var N = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {2.0, 0.0, 0.0, 2.0});

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R, N));
  }

  @Test
  void testRNotSymmetric_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1.0, 1.0, 0.0, 1.0});

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R));
  }

  @Test
  void testRNotPositiveDefinite_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());

    var R1 = new Matrix<>(Nat.N2(), Nat.N2());
    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R1));

    var R2 = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {-1.0, 0.0, 0.0, -1.0});
    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R2));
  }

  @Test
  void testRNotSymmetric_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var N = Matrix.eye(Nat.N2());
    var R = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1.0, 1.0, 0.0, 1.0});

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R, N));
  }

  @Test
  void testRNotPositiveDefinite_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var N = Matrix.eye(Nat.N2());

    var R1 = new Matrix<>(Nat.N2(), Nat.N2());
    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R1, N));

    var R2 = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {-1.0, 0.0, 0.0, -1.0});
    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R2, N));
  }

  @Test
  void testABNotStabilizable_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = new Matrix<>(Nat.N2(), Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R));
  }

  @Test
  void testABNotStabilizable_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = new Matrix<>(Nat.N2(), Nat.N2());
    var Q = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());
    var N = Matrix.eye(Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R, N));
  }

  @Test
  void testACNotDetectable_ABQR() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = new Matrix<>(Nat.N2(), Nat.N2());
    var R = Matrix.eye(Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R));
  }

  @Test
  void testACNotDetectable_ABQRN() {
    var A = Matrix.eye(Nat.N2());
    var B = Matrix.eye(Nat.N2());
    var Q = new Matrix<>(Nat.N2(), Nat.N2());
    var R = Matrix.eye(Nat.N2());
    var N = new Matrix<>(Nat.N2(), Nat.N2());

    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q, R, N));
  }

  @Test
  void testQDecomposition() {
    // Ensures the decomposition of Q into CᵀC is correct

    var A = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {1.0, 0.0, 0.0, 0.0});
    var B = Matrix.eye(Nat.N2());
    var R = Matrix.eye(Nat.N2());

    // (A, C₁) should be detectable pair
    var C_1 = new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0.0, 0.0, 1.0, 0.0});
    var Q_1 = C_1.transpose().times(C_1);
    assertDoesNotThrow(() -> DARE.dare(A, B, Q_1, R));

    // (A, C₂) shouldn't be detectable pair
    var C_2 = C_1.transpose();
    var Q_2 = C_2.transpose().times(C_2);
    assertThrows(IllegalArgumentException.class, () -> DARE.dare(A, B, Q_2, R));
  }
}
